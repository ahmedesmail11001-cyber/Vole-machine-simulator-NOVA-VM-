#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QPushButton>
#include <QStandardPaths>
#include <QHeaderView>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QRadialGradient>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QStringList>
#include <QStyledItemDelegate>
#include <QTableWidgetItem>

namespace {

// "Run program" gives up after this many instructions (protects against infinite loops)
const int kMaxRunSteps = 100000;

// Planet logo: a purple sphere with a tilted ring, drawn with QPainter (no image file needed)
QPixmap makePlanetLogo(int size)
{
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(size / 2.0, size / 2.0);
    p.rotate(-25);                                     // tilt everything

    const double r = size * 0.28;                      // planet radius
    const QRectF ring(-size * 0.46, -size * 0.14, size * 0.92, size * 0.28);
    QPen ringPen(QColor("#c4b5fd"), size * 0.06);
    ringPen.setCapStyle(Qt::RoundCap);

    p.setPen(ringPen);                                 // 1) whole ring (goes behind the planet)
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(ring);

    QRadialGradient glow(-r * 0.3, -r * 0.3, r * 1.5); // 2) the planet itself
    glow.setColorAt(0.0, QColor("#c4b5fd"));
    glow.setColorAt(1.0, QColor("#6d5bd0"));
    p.setPen(Qt::NoPen);
    p.setBrush(glow);
    p.drawEllipse(QPointF(0, 0), r, r);

    p.setPen(ringPen);                                 // 3) lower half of the ring, in front of the planet
    p.setBrush(Qt::NoBrush);
    p.drawArc(ring, 180 * 16, 180 * 16);

    return pixmap;
}

// true when every instruction line has exactly 16 bits (only 0 and 1), so the file is in binary
bool looksBinary(const QString& text)
{
    int lines = 0;
    for (QString line : text.split('\n')) {
        const int comment = line.indexOf(';');
        if (comment >= 0)
            line = line.left(comment);
        line = line.trimmed();
        line.remove(' ');
        if (line.isEmpty())
            continue;
        if (line.size() != 16 || line.count('0') + line.count('1') != 16)
            return false;
        lines++;
    }
    return lines > 0;
}

// 6 -> "06"
QString hex2(int value)
{
    return QString("%1").arg(value, 2, 16, QLatin1Char('0')).toUpper();
}

// 6 -> "0000 0110"
QString binary8(int value)
{
    const QString bits = QString("%1").arg(value, 8, 2, QLatin1Char('0'));
    return bits.left(4) + " " + bits.mid(4);
}

QTableWidgetItem* makeItem(const QString& text, const QColor& color)
{
    QTableWidgetItem* item = new QTableWidgetItem(text);
    item->setFlags(Qt::ItemIsEnabled);                 // read only
    item->setForeground(color);
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    return item;
}

// settings shared by the register table and the memory table
void configureTable(QTableWidget* table)
{
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(30);
    table->horizontalHeader()->setHighlightSections(false);
    table->horizontalHeader()->setSectionsClickable(false);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    table->setShowGrid(false);
    table->setAlternatingRowColors(true);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setFocusPolicy(Qt::NoFocus);
    table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

// Base delegate for both tables: the row under the mouse gets a soft purple highlight and a
// purple bar on its left. MainWindow::eventFilter stores the row number in the "hoverRow"
// property of the table (-1 = the mouse is not on any row).
class RowHoverDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override
    {
        paintHover(painter, option, index);
        QStyledItemDelegate::paint(painter, option, index);
    }

protected:
    static void paintHover(QPainter* painter, const QStyleOptionViewItem& option,
                           const QModelIndex& index)
    {
        const QWidget* table = option.widget;
        if (!table || table->property("hoverRow").toInt() != index.row())
            return;

        painter->fillRect(option.rect, QColor(167, 139, 250, 40));
        if (index.column() == 0)
            painter->fillRect(QRect(option.rect.left(), option.rect.top(), 3, option.rect.height()),
                              QColor("#a78bfa"));
    }
};

// Draws the memory matrix the way the prototype does:
//  - the VALUE column is a small rounded "pill"
//  - the row of the program counter has a green tint, a green bar on the left and a dot on the right
class MemoryDelegate : public RowHoverDelegate
{
public:
    using RowHoverDelegate::RowHoverDelegate;

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override
    {
        const bool isPcRow = index.data(Qt::UserRole).toBool();
        const QRect r = option.rect;

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        paintHover(painter, option, index);

        if (isPcRow) {
            painter->fillRect(r, QColor(52, 211, 153, 34));
            if (index.column() == 0)
                painter->fillRect(QRect(r.left(), r.top(), 3, r.height()), QColor("#34d399"));
        }

        if (index.column() == 1) {
            const int pillHeight = qMin(28, r.height() - 8);
            const QRect pill(r.left() + 6, r.center().y() - pillHeight / 2, 48, pillHeight);
            painter->setBrush(QColor("#14141f"));
            painter->setPen(QPen(QColor("#2a2a3f"), 1));
            painter->drawRoundedRect(pill, 6, 6);
            painter->setPen(QColor("#ffffff"));
            painter->setFont(option.font);
            painter->drawText(pill, Qt::AlignCenter, index.data().toString());
        }
        else if (index.column() == 3) {
            if (isPcRow) {
                painter->setPen(Qt::NoPen);
                painter->setBrush(QColor("#34d399"));
                painter->drawEllipse(r.center(), 3, 3);
            }
        }
        else {
            QStyledItemDelegate::paint(painter, option, index);   // plain text (hover already painted above)
        }

        painter->restore();
    }
};

} // namespace

// ------------------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    machine(new Machine()),
    state(State::NoProgram),
    cycleCount(0),
    lastExecAddr(-1),
    startAddress(0),
    instructionCount(0),
    runSteps(0),
    errorShown(false)
{
    ui->setupUi(this);
    setMinimumSize(1180, 700);

    applyStyle();
    setupTables();

    // ----- logo: purple planet, then "NOVA" with "Virtual Machine" under it -----
    setWindowTitle("NOVA Virtual Machine");
    ui->tagLabel->hide();                          // remove "MACHINE LANGUAGE PLAYGROUND"
    ui->titleLabel->setText("NOVA");
    ui->subtitleLabel->setText("Virtual Machine");
    ui->headerLeftLayout->setSpacing(0);

    QFont novaFont = ui->titleLabel->font();       // wide letter spacing, like a logo
    novaFont.setLetterSpacing(QFont::AbsoluteSpacing, 8);
    ui->titleLabel->setFont(novaFont);
    QFont subFont = ui->subtitleLabel->font();
    subFont.setLetterSpacing(QFont::AbsoluteSpacing, 4);
    ui->subtitleLabel->setFont(subFont);

    QLabel* logo = new QLabel(this);               // the planet, placed before the text
    logo->setPixmap(makePlanetLogo(72));
    logo->setFixedSize(72, 72);
    ui->headerRow->insertWidget(0, logo, 0, Qt::AlignVCenter);
    ui->headerRow->insertSpacing(1, 16);

    ui->inputHeader->hide();                       // remove "Program input / Write one machine instruction..."
    ui->waveform->hide();                              // removes the bars on the right of the Stop button

    // the program that the prototype shows in the editor
    ui->codeEditor->setPlainText("2145\n3100\nC000");

    // ----- "Load file..." button, placed in the header row of the machine code box -----
    QPushButton* loadFileButton = new QPushButton("Load file...", this);
    loadFileButton->setObjectName("loadFileButton");
    loadFileButton->setCursor(Qt::PointingHandCursor);
    loadFileButton->setToolTip("Open a program file (for example from your Desktop)");
    ui->codeHeadLayout->addSpacing(10);
    ui->codeHeadLayout->addWidget(loadFileButton);
    connect(loadFileButton, &QPushButton::clicked, this, &MainWindow::onLoadFileClicked);

    // when the user edits the text by hand, the file name next to the box is not true anymore
    connect(ui->codeEditor, &QPlainTextEdit::textChanged, this, [this]() {
        ui->codeHint->setText("One value per line");
        ui->codeHint->setToolTip(QString());
    });

    // start address: at most two hex digits
    ui->addressEdit->setValidator(
        new QRegularExpressionValidator(QRegularExpression("[0-9A-Fa-f]{0,2}"), this));

    ui->runButton->setShortcut(QKeySequence(Qt::Key_F5));
    ui->stepButton->setShortcut(QKeySequence(Qt::Key_F10));
    ui->runButton->setToolTip("Run the whole program (F5)");
    ui->stepButton->setToolTip("Execute one instruction (F10)");

    connect(ui->binaryButton,  &QPushButton::toggled, this, &MainWindow::onFormatChanged);
    connect(ui->decimalButton, &QPushButton::toggled, this, &MainWindow::onFormatChanged);
    connect(ui->hexButton,     &QPushButton::toggled, this, &MainWindow::onFormatChanged);
    connect(ui->storeButton,   &QPushButton::clicked, this, &MainWindow::onStoreClicked);
    connect(ui->runButton,     &QPushButton::clicked, this, &MainWindow::onRunClicked);
    connect(ui->stepButton,    &QPushButton::clicked, this, &MainWindow::onStepClicked);
    connect(ui->stopButton,    &QPushButton::clicked, this, &MainWindow::onStopClicked);
    connect(&runTimer, &QTimer::timeout, this, &MainWindow::onRunTick);

    refreshAll();
    updateUi();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ---------------------------------------------------------------- look and feel

void MainWindow::applyStyle()
{
    QFile file(":/style.qss");
    if (file.open(QFile::ReadOnly))
        setStyleSheet(QString::fromUtf8(file.readAll()));

    // QSS cannot do letter spacing, so the small CAPTION labels get it here
    const QStringList spaced = { "cap", "tag", "footer", "badge", "livetext" };
    const QList<QLabel*> labels = findChildren<QLabel*>();
    for (QLabel* label : labels) {
        if (spaced.contains(label->property("role").toString())) {
            QFont f = label->font();
            f.setLetterSpacing(QFont::AbsoluteSpacing, 1.4);
            label->setFont(f);
        }
    }
}

void MainWindow::setupTables()
{
    // ---- CPU registers: REG | BINARY | HEX ----
    QTableWidget* reg = ui->registerTable;
    configureTable(reg);
    reg->setColumnCount(3);
    reg->setRowCount(16);
    reg->setHorizontalHeaderLabels({ "REG", "BINARY", "HEX" });
    reg->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    reg->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    reg->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    reg->setItemDelegate(new RowHoverDelegate(reg));       // highlight the row under the mouse
    reg->setMouseTracking(true);
    reg->viewport()->setMouseTracking(true);
    reg->viewport()->installEventFilter(this);
    reg->setProperty("hoverRow", -1);
    reg->setColumnWidth(0, 84);
    reg->setColumnWidth(2, 96);
    // the 16 rows share the whole height of the card, so there is no empty space under RF
    reg->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    reg->verticalHeader()->setMinimumSectionSize(24);

    for (int i = 0; i < 16; i++) {
        reg->setItem(i, 0, makeItem(QString("R%1").arg(i, 1, 16).toUpper(), QColor("#a78bfa")));
        reg->setItem(i, 1, makeItem("0000 0000", QColor("#c9c9dc")));
        reg->setItem(i, 2, makeItem("00", QColor("#ffffff")));
    }

    // ---- Memory matrix: ADDR | VALUE | BINARY | (dot on the program counter row) ----
    QTableWidget* mem = ui->memoryTable;
    configureTable(mem);
    mem->setColumnCount(4);
    mem->setRowCount(256);
    mem->setHorizontalHeaderLabels({ "ADDR", "VALUE", "BINARY", "" });
    mem->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    mem->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    mem->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    mem->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    mem->setColumnWidth(0, 86);
    mem->setColumnWidth(1, 106);
    mem->setColumnWidth(3, 34);
    mem->verticalHeader()->setDefaultSectionSize(38);      // taller rows for the bigger font
    mem->setItemDelegate(new MemoryDelegate(mem));         // pills + program counter row + hover
    mem->setMouseTracking(true);
    mem->viewport()->setMouseTracking(true);
    mem->viewport()->installEventFilter(this);
    mem->setProperty("hoverRow", -1);

    for (int a = 0; a < 256; a++) {
        mem->setItem(a, 0, makeItem(hex2(a), QColor("#d8d8e8")));
        mem->setItem(a, 1, makeItem("00", QColor("#ffffff")));
        mem->setItem(a, 2, makeItem("0000 0000", QColor("#9a9ab5")));
        mem->setItem(a, 3, makeItem("", QColor("#ffffff")));
    }
}

// ---------------------------------------------------------------- button slots

CodeFormat MainWindow::currentFormat() const
{
    if (ui->binaryButton->isChecked())  return CodeFormat::Binary;
    if (ui->decimalButton->isChecked()) return CodeFormat::Decimal;
    return CodeFormat::Hex;
}

// Remembers which row of the register / memory table the mouse is on, so the delegate can highlight it
bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    QTableWidget* table = nullptr;
    if (watched == ui->registerTable->viewport())
        table = ui->registerTable;
    else if (watched == ui->memoryTable->viewport())
        table = ui->memoryTable;

    if (table && (event->type() == QEvent::MouseMove || event->type() == QEvent::Leave)) {
        int row = -1;                                  // -1 = the mouse left the table
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent* mouse = static_cast<QMouseEvent*>(event);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            row = table->indexAt(mouse->position().toPoint()).row();
#else
            row = table->indexAt(mouse->pos()).row();
#endif
        }

        if (table->property("hoverRow").toInt() != row) {
            table->setProperty("hoverRow", row);


            table->viewport()->update();               // repaint so the highlight moves
        }
    }

    return QMainWindow::eventFilter(watched, event);   // never consume the event
}

void MainWindow::onFormatChanged()
{
    switch (currentFormat()) {
    case CodeFormat::Binary:  ui->formatBadge->setText("BIN"); break;
    case CodeFormat::Decimal: ui->formatBadge->setText("DEC"); break;
    case CodeFormat::Hex:     ui->formatBadge->setText("HEX"); break;
    }
}

void MainWindow::onStoreClicked()
{
    // 1) turn the text into 4-digit hex lines
    const ParseResult parsed = CodeParser::parse(ui->codeEditor->toPlainText(), currentFormat());
    if (!parsed.ok) {
        showError(parsed.error);
        return;
    }

    // 2) start address
    bool ok = false;
    const int address = ui->addressEdit->text().toInt(&ok, 16);
    if (!ok) {
        showError("Type the start address in hex, for example 00.");
        return;
    }

    // 3) load into a brand new machine (clean memory, registers and screen);
    //    the old machine is only replaced when loading succeeds
    const std::string program = parsed.hexLines.join("\n").toStdString();
    std::unique_ptr<Machine> fresh(new Machine());
    std::string error;
    if (!fresh->loadFromText(program, address, error)) {
        showError(QString::fromStdString(error));
        return;
    }

    machine = std::move(fresh);
    state = State::Ready;
    cycleCount = 0;
    lastExecAddr = -1;
    startAddress = address;
    instructionCount = parsed.hexLines.size();
    notice.clear();
    errorShown = false;

    refreshAll();
    updateUi();
    ui->memoryTable->scrollToItem(ui->memoryTable->item(address, 0), QAbstractItemView::PositionAtTop);
}

void MainWindow::onLoadFileClicked()
{
    if (state == State::Running)
        return;

    // the dialog opens on the Desktop folder
    const QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    const QString path = QFileDialog::getOpenFileName(
        this, "Load a Vole program", desktop,
        "Program files (*.txt *.vole *.hex);;All files (*)");

    if (!path.isEmpty())
        loadProgramFromFile(path);
}

void MainWindow::loadProgramFromFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        showError("Could not open file: " + path);
        return;
    }
    if (file.size() > 64 * 1024) {                 // a Vole program is tiny, so this is the wrong file
        showError("This file is too big to be a Vole program.");
        return;
    }

    QString text = QString::fromUtf8(file.readAll());
    if (text.startsWith(QChar(0xFEFF)))            // invisible mark that some editors add at the start
        text.remove(0, 1);

    ui->codeEditor->setPlainText(text);            // show the program in the editor
    if (looksBinary(text))                         // pick the format automatically
        ui->binaryButton->setChecked(true);
    else
        ui->hexButton->setChecked(true);           // normal Vole files: 4 hex digits per line

    // show the file name where "One value per line" was
    const QString name = QFileInfo(path).fileName();
    ui->codeHint->setText(ui->codeHint->fontMetrics().elidedText(name, Qt::ElideMiddle, 220));
    ui->codeHint->setToolTip(path);

    onStoreClicked();                              // and put it in memory right away
}

void MainWindow::onStepClicked()
{
    if (state == State::NoProgram)      // nothing stored yet: store what is in the editor first
        onStoreClicked();
    if (state != State::Ready)
        return;

    errorShown = false;
    notice.clear();
    doOneStep();
    refreshAll();
    updateUi();

    // keep the highlighted program counter row visible
    const int pc = machine->getCPU().getPC();
    if (pc >= 0 && pc < 256)
        ui->memoryTable->scrollToItem(ui->memoryTable->item(pc, 0), QAbstractItemView::EnsureVisible);
}

void MainWindow::onRunClicked()
{
    if (state == State::NoProgram)      // nothing stored yet: store what is in the editor first
        onStoreClicked();
    if (state != State::Ready)
        return;

    errorShown = false;
    notice.clear();
    state = State::Running;
    runSteps = 0;
    runTimer.start(15);
    updateUi();
}

void MainWindow::onStopClicked()
{
    if (state != State::Running)
        return;

    runTimer.stop();
    state = State::Ready;
    notice = "Stopped by the user.";
    refreshAll();
    updateUi();
}

void MainWindow::onRunTick()
{
    // a few hundred instructions per tick keeps the window responsive, so "Stop" always works
    const int stepsPerTick = 500;

    for (int i = 0; i < stepsPerTick && state == State::Running; i++) {
        if (runSteps >= kMaxRunSteps) {
            state = State::Ready;
            notice = QString("Paused after %1 instructions (maybe an endless loop). Press Run to continue.")
                         .arg(kMaxRunSteps);
            runSteps = 0;
            break;
        }
        doOneStep();      // changes state to Halted when the program ends
        runSteps++;
    }

    if (state != State::Running)
        runTimer.stop();

    refreshAll();
    updateUi();

    const int pc = machine->getCPU().getPC();
    if (pc >= 0 && pc < 256)
        ui->memoryTable->scrollToItem(ui->memoryTable->item(pc, 0), QAbstractItemView::EnsureVisible);
}

// ---------------------------------------------------------------- machine access

bool MainWindow::doOneStep()
{
    const int pcBefore = machine->getCPU().getPC();
    const bool executed = machine->step();

    if (executed) {
        cycleCount++;
        lastExecAddr = pcBefore;
    }

    if (machine->isHalted()) {
        state = State::Halted;
        if (executed)
            notice = "Program finished (halt instruction).";
        else
            notice = QString("Stopped at 0x%1: invalid instruction or PC out of range.").arg(hex2(pcBefore));
    }
    return executed;
}

// ---------------------------------------------------------------- redraw

void MainWindow::refreshAll()
{
    const CPU& cpu = machine->getCPU();
    const int pc = cpu.getPC();

    ui->pcValueLabel->setText(hex2(pc));
    ui->cycleValueLabel->setText(QString("%1").arg(cycleCount, 4, 10, QLatin1Char('0')));

    // registers
    ui->registerTable->setUpdatesEnabled(false);
    for (int i = 0; i < 16; i++) {
        const int value = cpu.getRegisters().read(i);
        ui->registerTable->item(i, 1)->setText(binary8(value));
        ui->registerTable->item(i, 2)->setText(hex2(value));
    }
    ui->registerTable->setUpdatesEnabled(true);

    // memory matrix (the row of the program counter is flagged for the delegate)
    int used = 0;
    ui->memoryTable->setUpdatesEnabled(false);
    for (int a = 0; a < 256; a++) {
        const int value = machine->getMemory().read(a);
        if (value != 0)
            used++;

        ui->memoryTable->item(a, 1)->setText(hex2(value));
        ui->memoryTable->item(a, 2)->setText(binary8(value));

        const bool isPcRow = (a == pc);
        for (int c = 0; c < 4; c++)
            ui->memoryTable->item(a, c)->setData(Qt::UserRole, isPcRow);
    }
    ui->memoryTable->setUpdatesEnabled(true);
    ui->memoryTable->viewport()->update();

    ui->usedCountLabel->setText(QString::number(used));
}

void MainWindow::updateUi()
{
    const bool running = (state == State::Running);
    const bool canRun = (state == State::NoProgram || state == State::Ready);

    ui->runButton->setEnabled(canRun);
    ui->stepButton->setEnabled(canRun);
    ui->stopButton->setEnabled(running);
    ui->storeButton->setEnabled(!running);
    if (QPushButton* loadButton = findChild<QPushButton*>("loadFileButton"))
        loadButton->setEnabled(!running);
    ui->waveform->setActive(running);

    // core status chip (top right)
    QString text = "STANDBY";
    QString color = "#ffffff";
    switch (state) {
    case State::NoProgram: text = "STANDBY"; color = "#ffffff"; break;
    case State::Ready:     text = "READY";   color = "#5eead4"; break;
    case State::Running:   text = "RUNNING"; color = "#a78bfa"; break;
    case State::Halted:    text = "HALTED";  color = "#f5b94a"; break;
    }
    ui->coreStatusLabel->setText(text);
    ui->coreStatusLabel->setStyleSheet(QString("color: %1; font-size: 14px; font-weight: 700;").arg(color));

    // output screen
    if (errorShown) {
        ui->terminalLabel->setText("ERROR  " + notice);
        ui->terminalLabel->setStyleSheet("color: #f87171;");
    } else {
        ui->terminalLabel->setText(buildTerminalText());
        ui->terminalLabel->setStyleSheet("");
    }
}

QString MainWindow::buildTerminalText() const
{
    QStringList lines;
    const int pc = machine->getCPU().getPC();

    if (state == State::NoProgram) {
        lines << "READY" << "Store a program to begin_";
    }
    else if (cycleCount == 0 && state != State::Halted) {
        lines << QString("LOADED  %1 instruction(s) at 0x%2").arg(instructionCount).arg(hex2(startAddress))
        << QString("PC 0x%1_").arg(hex2(pc));
    }
    else if (cycleCount == 0) {                 // halted before executing anything
        lines << QString("HALTED  PC 0x%1_").arg(hex2(pc));
    }
    else {
        lines << QString("EXECUTING  0x%1").arg(hex2(lastExecAddr))
        << QString("PC 0x%1 \u2192 0x%2_").arg(hex2(lastExecAddr)).arg(hex2(pc));
    }

    if (!notice.isEmpty())
        lines << notice;

    // everything the program wrote to address FF, shown as ASCII
    const vector<uint8_t>& buffer = machine->getScreen().getBuffer();
    if (!buffer.empty()) {
        QString ascii;
        for (uint8_t c : buffer)
            ascii += (c >= 32 && c < 127) ? QChar(QLatin1Char(static_cast<char>(c))) : QChar(0x00B7);
        lines << QString() << "SCREEN  " + ascii;
    }

    return lines.join("\n");
}

void MainWindow::showError(const QString& message)
{
    notice = message;
    errorShown = true;
    updateUi();
}
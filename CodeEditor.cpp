#include "CodeEditor.h"

#include <QPainter>
#include <QTextBlock>

// The small widget on the left that draws the line numbers.
// It only forwards its painting to the CodeEditor.
class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor* editor) : QWidget(editor), codeEditor(editor) {}

    QSize sizeHint() const override { return QSize(codeEditor->lineNumberAreaWidth(), 0); }

protected:
    void paintEvent(QPaintEvent* event) override { codeEditor->lineNumberAreaPaintEvent(event); }

private:
    CodeEditor* codeEditor;
};

CodeEditor::CodeEditor(QWidget* parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, &QPlainTextEdit::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest, this, &CodeEditor::updateLineNumberArea);

    setLineWrapMode(QPlainTextEdit::NoWrap);
    updateLineNumberAreaWidth();
}

int CodeEditor::lineNumberAreaWidth() const
{
    return 62;   // fixed width, enough for "01" .. "99" in the prototype style
}

void CodeEditor::updateLineNumberAreaWidth()
{
    // keep the text away from the gutter
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect& rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth();
}

void CodeEditor::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);

    const QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(lineNumberArea);

    // gutter background and the thin line on its right side
    painter.fillRect(event->rect(), QColor("#090912"));
    painter.setPen(QColor("#1a1a2a"));
    painter.drawLine(lineNumberArea->width() - 1, event->rect().top(),
                     lineNumberArea->width() - 1, event->rect().bottom());

    QFont numberFont = font();
    numberFont.setPixelSize(13);
    numberFont.setBold(false);
    painter.setFont(numberFont);
    painter.setPen(QColor("#4b4b66"));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const QString number = QString("%1").arg(blockNumber + 1, 2, 10, QLatin1Char('0'));
            painter.drawText(0, top, lineNumberArea->width() - 18, bottom - top,
                             Qt::AlignRight | Qt::AlignVCenter, number);
        }
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

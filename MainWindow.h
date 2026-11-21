#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/*
 Class responsibility:
 The main (and only) window of the NOVA VM GUI. It builds the screen from
 MainWindow.ui, applies the theme (style.qss) and connects the buttons to the
 existing simulator classes:
   Store in memory -> CodeParser + Machine::loadFromText
   Run program     -> repeats Machine::step() on a timer (so "Stop" always works)
   Step            -> one Machine::step()
 After every action it redraws the program counter, the registers, the memory
 matrix and the output screen from the machine's current state.
 */

#include <QMainWindow>
#include <QEvent>
#include <QTimer>
#include <QString>
#include <memory>

#include "Machine.h"
#include "CodeParser.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadProgramFromFile(const QString& path);   // used by the "Load file" button

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;   // finds the table row under the mouse

private slots:
    void onFormatChanged();   // Binary / Decimal / Hex buttons
    void onStoreClicked();    // "Store in memory"
    void onLoadFileClicked(); // "Load file..." (opens the file dialog)
    void onRunClicked();      // "Run program"  (runs everything at once)
    void onStepClicked();     // "Step"         (one instruction)
    void onStopClicked();     // "Stop"
    void onRunTick();         // called by the timer while running

private:
    enum class State { NoProgram, Ready, Running, Halted };

    void applyStyle();                  // loads style.qss
    void setupTables();                 // columns, row heights, first items
    CodeFormat currentFormat() const;

    bool doOneStep();                   // executes one instruction and updates the state
    void refreshAll();                  // PC, cycle, registers, memory matrix
    void updateUi();                    // buttons, status chip, output screen text
    QString buildTerminalText() const;
    void showError(const QString& message);

    Ui::MainWindow* ui;
    std::unique_ptr<Machine> machine;   // a new Machine is created every time a program is stored
    QTimer runTimer;

    State state;
    int cycleCount;       // executed instructions since the program was stored
    int lastExecAddr;     // address of the last executed instruction (-1 = none yet)
    int startAddress;     // where the program was stored
    int instructionCount; // how many instructions the program has
    int runSteps;         // steps done by the current "Run"
    QString notice;       // extra line shown in the output screen
    bool errorShown;      // true while the output screen shows an error (red text)
};

#endif // MAINWINDOW_H
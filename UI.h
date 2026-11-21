#ifndef UI_H
#define UI_H
#include <string>
#include "Machine.h"

class UI {
public:
    void run();

private:
    Machine machine;

    void showMenu();
    void showStatus();
    void loadProgram();
    void editRegister();
    void editMemory();
    void runProgram();
    void runAtOnce();
    void runStepByStep();
    bool askLoadAnotherOrExit();
    int readHex(const std::string& prompt);
};
#endif

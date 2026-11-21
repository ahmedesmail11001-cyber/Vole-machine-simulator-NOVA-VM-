#include "UI.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;

void UI::run() {
    int choice = 0;
    while (choice != 6) {
        showMenu();
        choice = readHex("Choice: ");
        switch (choice) {
            case 1: loadProgram(); break;
            case 2: editRegister(); break;
            case 3: editMemory(); break;
            case 4: showStatus(); break;
            case 5: runProgram();
                if (askLoadAnotherOrExit()) machine.reset();
                else choice = 6;
                break;
        }
    }
}

void UI::showMenu() {
    cout << "\n1.Load program\n2. Edit register\n3. Edit memory\n";
    cout << "4. Display status\n5. Run program\n6. Exit\n";
}

void UI::showStatus() {
    const CPU& cpu = machine.getCPU();
    cout << "\nMemory:\n";
    machine.getMemory().display();
    printf("PC: %02X\n", cpu.getPC());
    cout << "IR: " << cpu.getIR().toString() << "\nRegisters:\n";
    cpu.getRegisters().display();
    cout << "Screen:\n";
    machine.getScreen().display();
}

void UI::loadProgram() {
    string file, error;
    cout << "File name: ";
    getline(cin >> ws, file);
    int address = readHex("Start address: ");
    if (machine.loadFromFile(file, address, error)) showStatus();
    else cout << error << "\n";
}

void UI::editRegister() {
    int index = readHex("Register (0-F): ");
    int value = readHex("Value (00-FF): ");
    if (index >= 0 && index <= 15 && value >= 0)
        machine.setRegister(index, value);
}

void UI::editMemory() {
    int address = readHex("Address (00-FF): ");
    int value = readHex("Value (00-FF): ");
    if (address >= 0 && value >= 0)
        machine.setMemory(address, value);
}

void UI::runProgram() {
    if (readHex("1. Run at once  2. Step by step: ") == 1) runAtOnce();
    else runStepByStep();
}

void UI::runAtOnce() {
    machine.run();
    showStatus();
}

void UI::runStepByStep() {
    cin.ignore(1000, '\n');
    while (cin && !machine.isHalted()) {
        machine.step();
        showStatus();
        if (machine.isHalted()) break;
        cout << "Press Enter to continue...";
        cin.ignore(1000, '\n');
    }
}

bool UI::askLoadAnotherOrExit() {
    return readHex("1. Load another  2. Exit: ") != 2;
}

int UI::readHex(const string& prompt) {
    string text;
    cout << prompt;
    if (!(cin >> text)) exit(0);
    for (char c : text)
        if (!isxdigit((unsigned char)c)) return -1;
    return text.size() > 2 ? -1 : stoi(text, nullptr, 16);
}

#ifndef MACHINE_H
#define MACHINE_H
#include <cstdint>
#include <string>
#include "ProgramLoader.h"
#include "Memory.h"
#include "Screen.h"
#include "CPU.h"
using namespace std;


class Machine {
private:
    Memory memory;                  // 256 cells of memory 
    Screen screen;                 // buffer of the screen 
    CPU cpu;                      // the CPU of the machine

public:
    Machine();
    Machine(Memory& m, Screen& s);
    bool loadFromFile(const string& file,int start_address, string& error); 
    bool loadFromText(const string& text, int start_address, string& error);   // NEW: load a program typed in the GUI
    bool run();     // 
    bool step();     // 
    bool isHalted() const;   // return true if the program is halted
    void reset();    // reset the machine to the initial state
    void setRegister(int index, uint8_t newValue);   // set the value of the register at index to newValue
    void setMemory(int index, uint8_t newValue);     // set the value of the memory cell at index to newValue
    const CPU& getCPU() const;
    const Memory& getMemory() const;
    const Screen& getScreen() const;

};
#endif



// bool loadProgram(string &file, int &startAddress, string &errorMessage); // file name , start address to load in it , error message
// #include "CPU.h"
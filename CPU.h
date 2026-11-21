#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include "Instruction.h"
#include "RegisterFile.h"
#include "Memory.h"
#include "Screen.h"

class CPU {
public:
    static const int SCREEN_ADDRESS = 0xFF;

private:
    int pc;
    Instruction ir;
    RegisterFile registers;
    bool halted;
    Memory& memory;
    Screen& screen;

    bool fetch();
    void execute();

public:
    CPU(Memory& machineMemory, Screen& machineScreen);

    bool step();
    void start(int startAddr);
    void reset();
    void setRegister(int index, uint8_t value);

    bool isHalted() const;
    int getPC() const;
    const Instruction& getIR() const;
    const RegisterFile& getRegisters() const;
};
#endif

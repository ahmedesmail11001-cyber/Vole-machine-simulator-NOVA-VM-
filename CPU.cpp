#include "CPU.h"
#include "Alu.h"

    CPU::CPU(Memory& machineMemory, Screen& machineScreen)
    : pc(0), halted(false), memory(machineMemory), screen(machineScreen) {}

bool CPU::fetch() {
    if (pc < 0 || pc > 0xFE) {
        halted = true;
        return false;
    }

    ir = Instruction(memory.read(pc), memory.read(pc + 1));
    pc += 2;
    return true;
}

bool CPU::step() {
    if (halted || !fetch()) {
        return false;
    }

    if (!ir.isValid()) {
        halted = true;
        return false;
    }

    execute();
    return true;
}

void CPU::execute() {
    Alu alu;

    const int opcode = ir.getOpcode();
    const int r = ir.getR();
    const int s = ir.getS();
    const int t = ir.getT();
    const int xy = ir.getXY();

    const uint8_t valueR = registers.read(r);
    const uint8_t valueS = registers.read(s);
    const uint8_t valueT = registers.read(t);

    switch (opcode) {
        case 0x1:
            registers.write(r, memory.read(xy));
            break;

        case 0x2:
            registers.write(r, xy);
            break;

        case 0x3:
            if (xy == SCREEN_ADDRESS) {
                screen.write(valueR);
            } else {
                memory.write(xy, valueR);
            }
            break;

        case 0x4:
            registers.write(t, valueS);
            break;

        case 0x5:
            registers.write(r, alu.addInt(valueS, valueT));
            break;

        case 0x6:
            registers.write(r, alu.addFloat(valueS, valueT));
            break;

        case 0x7:
            registers.write(r, alu.bitOr(valueS, valueT));
            break;

        case 0x8:
            registers.write(r, alu.bitAnd(valueS, valueT));
            break;

        case 0x9:
            registers.write(r, alu.bitXor(valueS, valueT));
            break;

        case 0xA:
            registers.write(r, alu.rotateRight(valueR, t % 8));
            break;

        case 0xB:
            if (valueR == registers.read(0)) {
                pc = xy;
            }
            break;

        case 0xC:
        default:
            halted = true;
            break;
    }
}

void CPU::start(int startAddr) {
    pc = startAddr;
    halted = startAddr < 0 || startAddr > 0xFE;
}

void CPU::reset() {
    pc = 0;
    ir = Instruction();
    registers.clear();
    halted = false;
}

void CPU::setRegister(int index, uint8_t value) {
    registers.write(index, value);
}

bool CPU::isHalted() const {
    return halted;
}

int CPU::getPC() const {
    return pc;
}
// int currentAddress = cpu.getPC();

const Instruction& CPU::getIR() const {
    return ir;
}
// const Instruction& current = cpu.getIR();
const RegisterFile& CPU::getRegisters() const {
    return registers;
}

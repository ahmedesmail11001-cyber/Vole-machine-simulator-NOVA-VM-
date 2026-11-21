#include "Instruction.h"

using namespace std;

Instruction::Instruction() : highByte(0), lowByte(0) {}
Instruction::Instruction(uint8_t high,uint8_t low) {
    highByte = high;
    lowByte = low;
}

const int Instruction::getOpcode() const {
    return highByte / 16;
}

const int Instruction::getR() const {
    return highByte % 16;
}

const int Instruction::getS() const {
    return lowByte / 16;
}

const int Instruction::getT() const {
    return lowByte % 16;
}

const int Instruction::getXY() const {
    return lowByte;
}

bool Instruction::isValid() const {
    if (getOpcode() >= 1 && getOpcode() <= 12) {
        return true;
    }
    return false;
}

const std::string Instruction::toString() const {
    return std::to_string(highByte) + " " + std::to_string(lowByte);
}



#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>
#include <string>


class Instruction {
private:
    std::uint8_t highByte;          // opcode
    std::uint8_t lowByte;          // operand
public:
    Instruction();
    Instruction(std::uint8_t highByte, std::uint8_t lowByte); 

    const int getOpcode() const; //higherbyte = A1 = 
    const int getR() const;
    const int getS() const;
    const int getT() const;
    const int getXY() const;

    bool isValid() const;
    const std::string toString() const;
};

#endif
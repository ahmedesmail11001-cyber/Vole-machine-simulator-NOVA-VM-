#ifndef ALU_H
#define ALU_H
#include <cstdint>

class Alu {

public:
    uint8_t addInt(uint8_t a, uint8_t b);
    uint8_t addFloat(uint8_t a, uint8_t b);
    uint8_t bitOr(uint8_t a, uint8_t b);
    uint8_t bitAnd(uint8_t a, uint8_t b);
    uint8_t bitXor(uint8_t a, uint8_t b);
    uint8_t rotateRight(uint8_t a, uint8_t n);




};
#endif

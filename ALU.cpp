#include "Alu.h"

uint8_t Alu::addInt(uint8_t a, uint8_t b) // 07 + 15 8 bits + 8 bits  3201
{
    return a + b;
}

uint8_t Alu::addFloat(uint8_t a, uint8_t b)
{
    return (float)a + (float)b; //0 000 0101 + 0 000 0111
}

uint8_t Alu::bitOr(uint8_t a, uint8_t b)
{
    return a | b;
}

uint8_t Alu::bitAnd(uint8_t a, uint8_t b)
{
    return a & b;
}

uint8_t Alu::bitXor(uint8_t a, uint8_t b)
{
    return a ^ b;
}

uint8_t Alu::rotateRight(uint8_t a, uint8_t n)
{
    return (a >> n) | (a << (8 - n)); // 00001010 -> 00000001 | 01000000 = 01000001
}


/*0 100 1000  (0) positive (100) exponent = 0 (1000) mantissa = 0.5 -> 0.5
+
 0 101 0100    (0) positive (101) exponent = 1 (0100) mantissa = 0.25 -> 0.25 * 2^1 = 0.5
 ---------------
 0 101 1000  (1.0)  8 bits + 8 bits = 16 bits*/
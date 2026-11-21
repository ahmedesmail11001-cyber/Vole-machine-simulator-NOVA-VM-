#ifndef MEMORY_H
#define MEMORY_H
#include <cstdint>

class Memory {
private:
    uint8_t cells[256];

public:
    Memory();
    uint8_t read(int addr ) const;
    void write(int addr, uint8_t value);
    void clear();
    void display() const;
};
#endif
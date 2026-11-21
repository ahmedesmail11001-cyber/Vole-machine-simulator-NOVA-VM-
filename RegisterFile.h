#ifndef REGISTERFILE_H
#define REGISTERFILE_H
#include <cstdint>

class RegisterFile {
private:
 uint8_t cells[16];

public:
 RegisterFile();
 uint8_t read(int index) const;
 void write(int index, uint8_t value);
 void clear();
 void display() const;
};

#endif



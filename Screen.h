#ifndef SCREEN_H
#define SCREEN_H
#include <cstdint>
#include <vector>
#include <iostream>
using namespace std;

class Screen {
private:
    vector<uint8_t> buffer;
public:
    Screen();
    void write(uint8_t value);
    void clear();
    void display() const;
    const vector<uint8_t>& getBuffer() const;   // NEW: lets the GUI read what was written to the screen

};
#endif

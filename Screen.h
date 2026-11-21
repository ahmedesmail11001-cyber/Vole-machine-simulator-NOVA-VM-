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

};
#endif

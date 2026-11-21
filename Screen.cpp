#include "Screen.h"
using namespace std;

Screen::Screen()
{
    buffer.clear();
}

void Screen::write(uint8_t value)
{
    buffer.push_back(value);
}

void Screen::clear()
{
    buffer.clear();
}

void Screen::display() const
{
    for (auto x : buffer) {
        cout << x << " " ;
    }
}

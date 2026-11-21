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

// NEW: lets the GUI read what was written to the screen
const vector<uint8_t>& Screen::getBuffer() const
{
    return buffer;
}

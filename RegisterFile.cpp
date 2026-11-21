/*
Class responsibility:
 this class is made for the register to enable CPU to modify in the 16 cells of the register.
 It can read, write clear and display any cell in the register.

 */

#include "RegisterFile.h"
#include <iostream>
#include <iomanip>
using namespace std;

RegisterFile::RegisterFile() {
    clear();

}

uint8_t RegisterFile::read(int index) const {
    if (index<0 || index>=16) {
        return false;
    }
    return cells[index];
}

void RegisterFile::write(int index, uint8_t value) {
    if (index<0 || index>=16) {
        return;
    }
    cells[index] = value;
}

void RegisterFile::clear() {
    for (int i=0 ;i<16; i++) {
        this->cells[i] = 0;
    }
}

void RegisterFile::display() const {
    // just one for loop as it is not a grid
    for (int i=0; i<16; i++) {
        cout << hex << setw(2) << setfill('0') << static_cast<int>(this->cells[i]) <<' ';
    }
    cout << dec;
    cout << '\n';
}

/*
 Class responsibility:
 this class is made for the memory to create the 256 cell of the memory
 and make some important functions like read, write, clear and display cells
 to be used from the other classes:
 CPU -> reads and writes data from the memory
 ProgramLoader -> Write the program in the memory
 Machine -> the memory is a part from it
 */

#include "Memory.h"
#include <iostream>
#include <iomanip>
using namespace std;

Memory::Memory() {
  clear();
}

uint8_t Memory::read(int addr) const {
 if (addr<0 || addr>=256) {
  return false;
 }
 return cells[addr];

}

void Memory::write(int addr, uint8_t value) {
   if (addr<0 || addr>=256) {
     return;
   }

   cells[addr] = value;
}

void Memory::clear() {
 for (int i=0 ;i<256; i++) {
  cells[i] = 0; // reset every cell to 0
 }
}

void Memory::display() const {
   for (int i=0; i<16; i++) {
      for (int j=0; j<16; j++) {
        int index = i*16+j;  // convert the cell index into Hexadecimal

        // Making the Hexa formating for the 16x16 matrix
        cout << hex << setw(2) << setfill('0')
        << static_cast<int>(cells[index]) << ' ';
      }
      cout << dec;  // return the type to decimal
      cout << '\n'; // new line after the 16 digit row
    
   }
}


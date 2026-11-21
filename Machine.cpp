#include "Machine.h"
#include <sstream>
using namespace std;

Machine::Machine(): memory() , screen(), cpu(memory ,screen) {}

Machine::Machine(Memory& m, Screen& s) : memory(m), screen(s), cpu(memory,screen){}

bool Machine::loadFromFile(const string &file, int start_address, string &error)
{
    if(!ProgramLoader::loadFromFile(file, memory, start_address, error))
    {
        return false;
    }
    cpu.start(start_address);
    return true;
}

// NEW: same as loadFromFile, but the program text comes from the GUI instead of a file
bool Machine::loadFromText(const string &text, int start_address, string &error)
{
    istringstream in(text);
    if(!ProgramLoader::loadFromStream(in, memory, start_address, error))
    {
        return false;
    }
    cpu.start(start_address);
    return true;
}

bool Machine::run()  
{
    while (!cpu.isHalted())
    {
        if(!cpu.step()){
            return false;
        };
    }
    return true;
}

bool Machine::step() 
{
    return cpu.step();
}

void Machine::reset()
{
    memory.clear();
    screen.clear();
    cpu.reset();        

}

bool Machine::isHalted() const
{
    return cpu.isHalted();
}

void Machine::setRegister(int index, uint8_t newValue)
{
    cpu.setRegister(index, newValue);
}

void Machine::setMemory(int index, uint8_t newValue)
{
    memory.write(index, newValue);
}

const CPU &Machine::getCPU() const                  // getPC getIR getRegister 
{
    return cpu;
}

const Memory &Machine::getMemory() const         //getMemory 
{
   return memory;
}

const Screen &Machine::getScreen() const       //getScreen
{
    return screen;
}

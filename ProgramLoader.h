#ifndef LOADER_H
#define LOADER_H
#include <string>
#include <istream>
#include "Memory.h"

class ProgramLoader {
    
public:
    static bool loadFromFile(const std::string& file, Memory& memory,int start_address, std::string& error);
    // NEW: same job as loadFromFile but reads the lines from any stream (the GUI uses it)
    static bool loadFromStream(std::istream& in, Memory& memory, int start_address, std::string& error);

private:
    static bool isValidHexLine(const std::string& line);

};
#endif
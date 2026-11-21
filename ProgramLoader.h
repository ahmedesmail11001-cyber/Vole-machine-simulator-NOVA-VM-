#ifndef LOADER_H
#define LOADER_H
#include <string>
#include "Memory.h"

class ProgramLoader {
    
public:
    static bool loadFromFile(const std::string& file, Memory& memory,int start_address, std::string& error);

private:
    static bool isValidHexLine(const std::string& line);

};
#endif
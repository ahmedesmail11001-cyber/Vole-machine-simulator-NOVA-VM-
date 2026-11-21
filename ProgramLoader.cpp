#include "ProgramLoader.h"
#include <cctype>
#include <fstream>
#include <vector>
using namespace std;

bool ProgramLoader::isValidHexLine(const string& line) {
    if (line.size() < 4)
        return false;
    for (int i = 0; i < 4; i++)
        if (!isxdigit(static_cast<unsigned char>(line[i])))
            return false;
    return true;
}

bool ProgramLoader::loadFromFile(const string& file, Memory& memory, int start_address, string& error) {
    error.clear();
    if (start_address < 0 || start_address >= 0xFF) {
        error = "Start address must be between 00 and FE.";
        return false;
    }
    if (start_address % 2 != 0) {
        error="Start address must be an even number.";
        return false;
    }

    ifstream in(file);
    if (!in) {
        error = "Could not open file: " + file;
        return false;
    }

    vector<int> instructions;
    string line;
    for (int n = 1; getline(in, line); n++) {
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == string::npos || line[start] == ';')
            continue;
        line = line.substr(start);
        if (!isValidHexLine(line)) {
            error = "Invalid instruction at line " + to_string(n) + ": each instruction needs 4 hex digits.";
            return false;
        }
        instructions.push_back(stoi(line.substr(0, 4), nullptr, 16));
    }

    if (instructions.empty()) {
        error = "The file contains no instructions.";
        return false;
    }
    if (start_address + 2*static_cast<int>(instructions.size()) > 256) {
        error = "The program does not fit in memory at this start address.";
        return false;
    }

    int address = start_address;
    for (int inst : instructions) {
        memory.write(address++, inst >> 8);
        memory.write(address++, inst & 0xFF);
    }
    return true;
}
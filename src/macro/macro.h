#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <cstdint>
#include "../utils.h"

struct Macro {
    std::string name;
    std::vector<std::string> parameters;
    std::vector<std::string> body;
};

class MacroProcessor {
public:
    MacroProcessor(const std::string& inputFileName, const std::string& outputFileName);
    void process();

private:
    std::string inputFileName;
    std::string outputFileName;
    std::vector<std::string> program;

    std::unordered_map<std::string, Macro> MNT; // Macro Name Table
    std::set<std::string> macroNames; 

    void loadFile();
    void saveFile(const std::vector<std::string>& preprocessedProgram);

    void replaceAll(std::string& str, const std::string& target, const std::string& replacement);
    uint16_t serialCounter = 0;
};
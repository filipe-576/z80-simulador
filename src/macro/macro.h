#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
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

    std::unordered_map<std::string, Macro> MacroNameTable; // MNT
    std::set<std::string> macroNames; 

    void loadFile();
    void saveFile(const std::vector<std::string>& preprocessedProgram);

    // Funções auxiliares pra macros:
    // bool isMacroDef(const std::string& token); // Se é MCDEFN ou MCEND
    // std::string getMacroOpcode(const std::vector<std::string>& instruction); // Para macros salvas na MNT
    // std::string getMacroLabel(const std::vector<std::string>& instruction); // Para macros salvas na MNT
};
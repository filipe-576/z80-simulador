#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct MacroDefinition {
    std::vector<std::string> lines; // Esqueleto da macro (com marcadores #1, #2)
};

struct MacroNameEntry {
    int num_parameters;
    MacroDefinition definition;
};

// O contrato estrito entre Pass 1 e Pass 2
struct MacroContext {
    std::unordered_map<std::string, MacroNameEntry> macroTable;
};
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

struct MacroDefinition {
    std::vector<std::string> lines;
};


struct MacroNameEntry {
    uint16_t numParameters;
    MacroDefinition definition;
};


struct MacroContext {
    std::unordered_map<std::string, MacroNameEntry> macroTable;
};
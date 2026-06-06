#pragma once

#include <set>
#include <string>
#include <vector>
#include <unordered_map>

class Assembler {
public:
    Assembler(std::string fileName);

    void assemble();   
    
    void debug();

private:
    unsigned int locationCounter;
    std::unordered_map<std::string, unsigned int> symbolTable;
    std::vector<std::string> program;
    std::string fileName;
    const std::set<std::string> MACHINE_INSTRUCTIONS = {
        "LD", "ADD", "SUB", "INC", "DEC", "AND", "OR", "XOR", "CP", "JP", "JR",
        "CALL","RET", "PUSH", "POP", "NOP", "HALT"
    };
    const std::set<std::string> PSEUDO_INSTRUCTIONS = {
        "ORG", "END", "EQU"
    };


    void loadFile();

    void firstPass();
    
    void secondPass();

    std::string getLabel(std::string line);

    std::string getOpcode(std::string line);

    std::string getOperand(std::string line);

    unsigned int getOperandValue(std::string operand);

    void insertTable(std::string label, unsigned int value);

};

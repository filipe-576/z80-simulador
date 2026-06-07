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
        "ORG", "END", "EQU", "DS"
    };

    void loadFile();

    void firstPass();
    
    void secondPass();

    std::vector<std::string> tokenizeInstruction(std::string instruction);

    std::string getLabel(std::vector<std::string> instruction);

    std::string getOpcode(std::vector<std::string> instruction);

    std::string getOperand(std::vector<std::string> instruction, unsigned short index=0);

    unsigned int getOperandValue(std::string operand);

    unsigned int getInstructionSize(std::vector<std::string> instruction);

    void insertTable(std::string label, unsigned int value);

    int findTable(std::string label);

};

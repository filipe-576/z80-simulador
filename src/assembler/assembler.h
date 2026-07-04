#pragma once

#include <cstdint>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>

class Assembler {
public:
    Assembler(const std::string& fileName);

/** 
 * @brief   Executa os dois passos do montador e gera o arquivo objeto
 */
    void assemble();   
    
    void debug();

private:
    std::string fileName;
    std::vector<std::string> program;
    unsigned int locationCounter;

    std::unordered_map<std::string, unsigned int> symbolTable;
    typedef struct RelocationItem_t{
        unsigned int offset, size;
        std::string symbol;
        bool isExt;
    } RelocationItem;

    std::vector<RelocationItem> relocationTable;
    std::unordered_map<std::string, unsigned int> useTable;  // símbolos externos (INTUSE)
    std::unordered_map<std::string, unsigned int> defTable;  // símbolos exportados (INTDEF)

    std::vector<uint8_t> machineCode;
    int startAddress = -1;

    const std::set<std::string> MACHINE_INSTRUCTIONS = {
        "LD", "ADD", "SUB", "INC", "DEC", "AND", "OR", "XOR", "CP", "JP", "JR",
        "CALL","RET", "PUSH", "POP", "NOP", "HALT"
    };
    const std::set<std::string> PSEUDO_INSTRUCTIONS = {
        "END", "EQU", "DS", "DC", "INTUSE", "INTDEF"
    };

    void loadFile();

    void firstPass();
    
    void secondPass();

    int getOperandValue(const std::string& operand);

    void insertInTable(const std::string& label, unsigned int value);


/** 
 * @brief   Retorna o valor associado ao símbolo na Tabela de Símbolos.
 * Retorna -1 caso não encontre.
 */
    int findInTable(const std::string& label);

    void generateMachineCode(const std::vector<std::string>& instruction);

};

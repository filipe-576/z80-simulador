#pragma once

#include <cstdint>
#include <optional>
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

    enum class SymbolType{ ABSOLUTE, RELATIVE, NOT_RESOLVED };
    enum class SymbolScope{ LOCAL, GLOBAL, EXTERN };

    struct Symbol{
        std::optional<uint16_t> value;
        SymbolType type;
        SymbolScope scope;
    };
    std::unordered_map<std::string, Symbol> symbolTable;

    struct RelocationItem{
        unsigned int offset, size;
        std::string symbol;
        bool isExt;
    };

    std::vector<RelocationItem> relocationTable;
    std::vector<std::string> pendingExports; // símbolos externos para resolver depois
    std::unordered_map<std::string, unsigned int> usageTable;  // símbolos externos (INTUSE)
    std::unordered_map<std::string, unsigned int> defTable;  // símbolos exportados (INTDEF)

    struct GenerateCodeResult{
        std::vector<uint8_t> bytes;
        int operandOffset; // -1 se não tiver
    };
    std::vector<uint8_t> machineCode;
    int startAddress = -1;

    const std::set<std::string> MACHINE_INSTRUCTIONS = {
        "LD", "ADD", "SUB", "INC", "DEC", "AND", "OR", "XOR", "CP", "JP", "JR",
        "CALL","RET", "PUSH", "POP", "NOP", "HALT"
    };
    const std::set<std::string> PSEUDO_INSTRUCTIONS = {
        "END", "EQU", "DS", "DC", "INTUSE", "INTDEF"
    };
    const std::unordered_map<std::string, uint> OPERATOR_SIZE = {
        {"", 1}, {"", 2}
    };

    void loadFile();

    void firstPass();

    void secondPass();


/** 
 * @brief   Retorna o valor do símbolo na tabela de símbolos.
 * @return  Retorna -1 se não encontrar
 */
    int getOperandValue(const std::string& operand);


/** 
 * @brief   Retorna o valor associado ao símbolo na Tabela de Símbolos.
 * Retorna -1 caso não encontre.
 */
    Symbol findInTable(const std::string& label);

    GenerateCodeResult generateMachineCode(const std::vector<std::string>& instruction);

};

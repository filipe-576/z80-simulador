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
    Assembler(const std::string& fileName, const std::string& outputName);

/** 
 * @brief   Executa os dois passos do montador e gera o arquivo objeto
 */
    void assemble();   
    
    void debug();

private:
    std::string fileName;
    std::string outputName;
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

    std::vector<uint16_t> relocationMap;
    std::vector<std::string> pendingExports; // símbolos externos para resolver depois
    std::unordered_map<std::string, std::vector<unsigned int>> usageTable;  // símbolos externos (INTUSE)
    std::unordered_map<std::string, unsigned int> defTable;  // símbolos exportados (INTDEF)

    struct GenerateCodeResult{
        std::vector<uint8_t> bytes;
        std::string symbolOperand;
        int operandOffset; // -1 se não tiver
    };
    std::vector<uint8_t> machineCode;
    int entryPoint = -1;

    const std::set<std::string> MACHINE_INSTRUCTIONS = {
        "LD", "ADD", "SUB", "INC", "DEC", "AND", "OR", "XOR", "CP", "JP", "JR",
        "CALL","RET", "PUSH", "POP", "NOP", "HALT"
    };
    const std::set<std::string> PSEUDO_INSTRUCTIONS = {
        "END", "EQU", "DS", "DC", "INTUSE", "INTDEF"
    };

    void loadFile();

/**
 * @brief   Preenche a tabela de símbolos.
 */
    void firstPass();

/**
 * @brief   Gera o código de máquina, preenche o mapa de realocação e tabelas de uso e definição.
 */
    void secondPass();

    void generateObjectFile();

/**
 * @brief   Preenche a tabela de definição.
 */
    void resolvePendingExports();


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

    void insertInUsageTable(const std::string& symbol, uint16_t address);

/**
 * @brief   Gera o código de máquina referente à instrução.
 * @return  Os bytes da instrução e o offset para o operando.
 */
    GenerateCodeResult generateMachineCode(const std::vector<std::string>& instruction);

/**
 * @brief   Retorna o tamanho em bytes ocupado pela instrução.
 */
    unsigned int getInstructionSize(const std::vector<std::string>& instruction);

};

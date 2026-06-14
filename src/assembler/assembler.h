#pragma once

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
    unsigned int locationCounter;
    std::unordered_map<std::string, unsigned int> symbolTable;
    std::vector<std::string> program;
    std::string fileName;
    const std::set<std::string> MACHINE_INSTRUCTIONS = {
        "LD", "ADD", "SUB", "INC", "DEC", "AND", "OR", "XOR", "CP", "JP", "JR",
        "CALL","RET", "PUSH", "POP", "NOP", "HALT"
    };
    const std::set<std::string> PSEUDO_INSTRUCTIONS = {
        "ORG", "END", "EQU", "DS", "DC"
    };

    void loadFile();

    void firstPass();
    
    void secondPass();

/** 
 * @brief   Tokeniza a string da instrução.
 * Exemplo: "LD A, @1" retorna um vetor ["LD", "A", "@1"]
 *
 * @return  Vetor com os tokens da instrução 
 */
    std::vector<std::string> tokenizeInstruction(const std::string& instruction);

    std::string getLabel(const std::vector<std::string>& instruction);

    std::string getOpcode(const std::vector<std::string>& instruction);

    std::string getOperand(const std::vector<std::string>& instruction, unsigned short index=0);

    int getOperandValue(const std::string& operand);


/** 
 * @brief   Retorna o tamanho em bytes ocupado pela instrução.
 */
    unsigned int getInstructionSize(const std::vector<std::string>& instruction);

    void insertInTable(const std::string& label, unsigned int value);


/** 
 * @brief   Retorna o valor associado ao símbolo na Tabela de Símbolos.
 * Retorna -1 caso não encontre.
 */
    int findInTable(const std::string& label);

    bool isPseudoInstruction(const std::string& label);

    bool isMachineInstruction(const std::string& label);

};

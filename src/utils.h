#pragma once
#include <set>
#include <string>
#include <vector>


namespace utils {

    const std::set<std::string> MACHINE_INSTRUCTIONS = {
        "LD", "ADD", "SUB", "INC", "DEC", "AND", "OR", "XOR", "CP", "JP", "JR",
        "CALL","RET", "PUSH", "POP", "NOP", "HALT"
    };

    // Pseudo-instruções do assembler (INTUSE, INTDEF) e do processador de macros (ORG, MCDEFN, MCEND)
    const std::set<std::string> PSEUDO_INSTRUCTIONS = {
        "END", "EQU", "DS", "DC", "INTUSE", "INTDEF", "ORG", "MCDEFN", "MCEND"
    };


    bool isPseudoInstruction(const std::string& label);


    bool isMachineInstruction(const std::string& label);


/**
 * @brief   Retorna o operando da instrução
 *
 * @param   index 0 retorna o primeiro operando, 1 o segundo operando
 */
    std::string getOperand(const std::vector<std::string>& instruction, const unsigned short index=0);


    // additionalOpcodes contém as macros da MNT definidas pelo usuário.
    std::string getLabel(const std::vector<std::string>& instruction, const std::set<std::string>& additionalOpcodes = {});


    std::string getOpcode(const std::vector<std::string>& instruction, const std::set<std::string>& additionalOpcodes = {});


/**
 * @brief   Tokeniza a string da instrução.
 * Exemplo: "LD A, @1" retorna um vetor ["LD", "A", "@1"]
 *
 * @return  Vetor com os tokens da instrução
 */
    std::vector<std::string> tokenizeInstruction(const std::string& instruction);

/**
 * @brief   Retorna o tamanho em bytes ocupado pela instrução.
 */
    unsigned int getInstructionSize(const std::vector<std::string>& instruction);

}
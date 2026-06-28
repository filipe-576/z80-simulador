#include <set>
#include <string>
#include <vector>


namespace utils{
    const std::set<std::string> MACHINE_INSTRUCTIONS = {
        "LD", "ADD", "SUB", "INC", "DEC", "AND", "OR", "XOR", "CP", "JP", "JR",
        "CALL","RET", "PUSH", "POP", "NOP", "HALT"
    };
    const std::set<std::string> PSEUDO_INSTRUCTIONS = {
        "ORG", "END", "EQU", "DS", "DC"
    };


    bool isPseudoInstruction(const std::string& label);


    bool isMachineInstruction(const std::string& label);


/** 
 * @brief   Retorna o operando da instrução
 *
 * @param   index 0 retorna o primeiro operando, 1 o segundo operando
 */
    std::string getOperand(const std::vector<std::string>& instruction, const unsigned short index=0);


    std::string getLabel(const std::vector<std::string>& instruction);


    std::string getOpcode(const std::vector<std::string>& instruction);

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
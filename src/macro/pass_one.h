#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "macro_context.h"

class PassOne {
private:
    enum class State { NORMAL, DEFINITION };
    State currentState;
    
    std::string currentMacroName;
    std::vector<std::string> currentFormalParams;

    // Métodos utilitários encapsulados
    std::vector<std::string> extractFormalParameters(const std::string& operandsPart);
    std::string replaceFormalWithPositional(const std::string& line, const std::vector<std::string>& formalParams);

public:
    PassOne() : currentState(State::NORMAL) {}

    /**
     * @brief Executa a Passagem 1 consumindo uma stream de entrada e gerando uma stream intermediária.
     * @param input Stream do código-fonte original.
     * @param intermediateOutput Stream de saída para o código sem definições.
     * @param ctx Referência ao contexto de macros a ser populado.
     * @return true em caso de sucesso, false caso haja erro de sintaxe.
     */
    bool execute(std::istream& input, std::ostream& intermediateOutput, MacroContext& ctx);
};
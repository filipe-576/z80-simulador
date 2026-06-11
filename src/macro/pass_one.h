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

    std::vector<std::string> extractFormalParameters(const std::string& operandsPart);
    std::string replaceFormalWithPositional(const std::string& line, const std::vector<std::string>& formalParams);

public:
    PassOne() : currentState(State::NORMAL) {}

    /**
     * @brief  Mapeia as macros do programa, substituindo parâmetros formais por genéricos (#1, #2).
     *
     * @param  input               Stream do arquivo Assembly inicial.
     * @param  intermediateOutput  Stream gravando o código expurgado das diretivas de definição.
     * @param  ctx                 Dicionário estrutural que guardará os blocos (MNT/MDT).
     * @return                     Verdadeiro se não houveram quebras de bloco "ENDM", Falso para falhas de sintaxe.
     */
    bool execute(std::istream& input, std::ostream& intermediateOutput, MacroContext& ctx);
};
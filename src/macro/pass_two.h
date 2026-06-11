#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <cstdint>
#include "macro_context.h"

class PassTwo {
private:
    enum class State { NORMAL, EXPANSION };
    
    State currentState;
    uint16_t serialCounter;

    std::vector<std::string> ala;

    void buildAla(const std::string& operandsPart);
    std::string expandLine(const std::string& skeletonLine, const std::string& currentSerial);

public:
    PassTwo() : currentState(State::NORMAL), serialCounter(0) {}

    /**
     * @brief  Traduz chamadas de macros para código Assembly linear.
     *
     * @param  intermediateInput  Stream contendo as chamadas capturadas do Passo 1.
     * @param  finalOutput        Stream destinada a receber as expansões atômicas (resultado final puro).
     * @param  ctx                Referência ao dicionário MNT/MDT indexado no Passo 1.
     * @return                    Verdadeiro se o código linear pôde ser gerado; falso para falhas operacionais.
     */
    bool execute(std::istream& intermediateInput, std::ostream& finalOutput, const MacroContext& ctx);
};
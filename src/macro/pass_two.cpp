#include "pass_two.hpp"
#include "utils.hpp"
#include <fstream>
#include <sstream>

bool PassTwo::execute(std::istream& intermediateInput, std::ostream& finalOutput, const MacroContext& ctx) {
    std::string line;
    currentState = State::NORMAL;

    while (std::getline(intermediateInput, line)) {
        // Remove comentários e espaços para facilitar o parsing
        std::string cleanLine = Utils::trim(Utils::stripComments(line));
        if (cleanLine.empty()) {
            if (currentState == State::NORMAL) finalOutput << "\n";
            continue;
        }

        // Tokenização básica para identificar se a linha é uma chamada de macro
        std::stringstream ss(cleanLine);
        std::string firstToken, remaining;
        ss >> firstToken;
        std::getline(ss, remaining);
        remaining = Utils::trim(remaining);

        if (currentState == State::NORMAL) {
            // Verifica se o mnemônico está na MNT (é uma chamada de macro?)
            if (ctx.macroTable.find(firstToken) != ctx.macroTable.end()) {
                currentState = State::EXPANSION;
                
                // 1. Constrói a ALA (Argument List Array) para esta expansão
                buildALA(remaining);

                // 2. Expande o corpo da macro contido na MDT
                const auto& macroEntry = ctx.macroTable.at(firstToken);
                for (const auto& skeletonLine : macroEntry.definition.lines) {
                    finalOutput << expandLine(skeletonLine) << "\n";
                }

                // 3. Retorna ao estado normal após expansão
                currentState = State::NORMAL;
                ala.clear();
            } else {
                // Não é macro, copia para a saída final
                finalOutput << line << "\n";
            }
        }
    }
    return true;
}

void PassTwo::buildALA(const std::string& operandsPart) {
    ala = Utils::splitOperands(operandsPart, ',');
}

std::string PassTwo::expandLine(const std::string& skeletonLine) {
    std::string expandedLine = skeletonLine;

    // Substituição dos marcadores posicionais #1, #2, ... pelos argumentos da ALA
    for (size_t i = 0; i < ala.size(); ++i) {
        std::string target = "#" + std::to_string(i + 1);
        
        // Substitui todas as ocorrências de #i pelo argumento ala[i]
        // Usamos replaceToken para garantir que tokens como #10 não sejam 
        // confundidos com #1 seguido de '0'
        expandedLine = Utils::replaceToken(expandedLine, target, ala[i]);
    }

    // Processamento de rótulos serializados (ex: .SER)
    // Se a macro contiver um marcador de label serializado, substituímos pelo contador
    if (expandedLine.find(".SER") != std::string::npos) {
        std::string serial = std::to_string(++serialCounter);
        // Garante formatação com zero à esquerda (ex: 1 -> 001)
        while (serial.length() < 3) serial = "0" + serial;
        
        expandedLine = Utils::replaceToken(expandedLine, ".SER", serial);
    }

    return expandedLine;
}
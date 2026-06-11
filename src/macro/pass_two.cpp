#include "pass_two.h"
#include "utils.h"
#include <fstream>
#include <sstream>

bool PassTwo::execute(std::istream& intermediateInput, std::ostream& finalOutput, const MacroContext& ctx) {
    std::string line;
    currentState = State::NORMAL;

    while (std::getline(intermediateInput, line)) {
        // Remove comentários e espaços para facilitar o parsing
        std::string cleanLine = Utils::stripComments(line);
        if (Utils::trim(cleanLine).empty()) {
            if (currentState == State::NORMAL) finalOutput << "\n";
            continue;
        }

        // Tokenização básica para identificar se a linha é uma chamada de macro
        std::string label, opcode, operands;
        Utils::parseAssemblyLine(cleanLine, label, opcode, operands);

        if (currentState == State::NORMAL) {
            // Verifica se a instrução é uma chamada de macro
            if (!opcode.empty() && ctx.macroTable.find(opcode) != ctx.macroTable.end()) {
                currentState = State::EXPANSION;
                
                // Se o usuário chamou a macro com rótulo, salva o rótulo
                if (!label.empty()) {
                    finalOutput << label << ":\n"; 
                }

                buildALA(operands);
                const auto& macroEntry = ctx.macroTable.at(opcode);

                // Calcula o .SER uma vez para toda a macro
                std::string currentSerial = std::to_string(++serialCounter);
                while (currentSerial.length() < 3) currentSerial = "0" + currentSerial; // 001

                // Expande o esqueleto
                for (const auto& skeletonLine : macroEntry.definition.lines) {
                    finalOutput << expandLine(skeletonLine, currentSerial) << "\n";
                }

                currentState = State::NORMAL;
                ala.clear();
            } else {
                // Não é macro, copia a linha limpa original
                finalOutput << line << "\n";
            }
        }
    }
    return true;
}

void PassTwo::buildALA(const std::string& operandsPart) {
    ala = Utils::splitOperands(operandsPart, ',');
}

std::string PassTwo::expandLine(const std::string& skeletonLine, const std::string& currentSerial) {
    std::string expandedLine = skeletonLine;

    // Substituição dos marcadores posicionais #1, #2, ... pelos argumentos da ALA
    for (size_t i = 0; i < ala.size(); ++i) {
        std::string target = "#" + std::to_string(i + 1);

        // Substitui todas as ocorrências de #i pelo argumento ala[i]
        // replaceToken para garantir que tokens como #10 não sejam confundidos com #1 seguido de '0'
        expandedLine = Utils::replaceToken(expandedLine, target, ala[i]);
    }

    // 2. Substitui o .SER usando a string já calculada, sem incrementar
    if (expandedLine.find(".SER") != std::string::npos) {
        expandedLine = Utils::replaceToken(expandedLine, ".SER", currentSerial);
    }

    return expandedLine;
}
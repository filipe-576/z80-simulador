#include "pass_two.h"
#include "utils.h"
#include <fstream>
#include <sstream>
#include <cstdint>

bool PassTwo::execute(std::istream& intermediateInput, std::ostream& finalOutput, const MacroContext& ctx) {
    std::string line;
    currentState = State::NORMAL;

    while (std::getline(intermediateInput, line)) {
        std::string cleanLine = Utils::stripComments(line);
        
        if (Utils::trim(cleanLine).empty()) {
            if (currentState == State::NORMAL) {
                finalOutput << "\n";
            }
            continue;
        }

        std::string label, opcode, operands;
        Utils::parseAssemblyLine(cleanLine, label, opcode, operands);

        if (currentState == State::NORMAL) {
            
            // Confirma no dicionário se essa palavra reservada é uma chamada direta de uma macro registrada.
            if (!opcode.empty() && ctx.macroTable.find(opcode) != ctx.macroTable.end()) {
                currentState = State::EXPANSION;
                
                if (!label.empty()) {
                    finalOutput << label << ":\n"; 
                }

                buildAla(operands);
                const auto& macroEntry = ctx.macroTable.at(opcode);

                // O incremento para labels contínuos (.SER) ocorre por macro expandida,
                // e não por linha de código que existe nela, prevenindo quebra de labels internos.
                std::string currentSerial = std::to_string(++serialCounter);
                
                while (currentSerial.length() < 3) {
                    currentSerial = "0" + currentSerial;
                }

                for (const auto& skeletonLine : macroEntry.definition.lines) {
                    finalOutput << expandLine(skeletonLine, currentSerial) << "\n";
                }

                currentState = State::NORMAL;
                ala.clear();

            } else {
                finalOutput << line << "\n";
            }

        }
    }

    return true;
}


void PassTwo::buildAla(const std::string& operandsPart) {
    ala = Utils::splitOperands(operandsPart, ',');
}


std::string PassTwo::expandLine(const std::string& skeletonLine, const std::string& currentSerial) {
    std::string expandedLine = skeletonLine;

    // Injera argumentos reais capturados da interface direto nos vetores demarcadores (#X) da planta baixa.
    for (uint16_t i = 0; i < ala.size(); ++i) {
        std::string target = "#" + std::to_string(i + 1);
        expandedLine = Utils::replaceToken(expandedLine, target, ala[i]);
    }

    if (expandedLine.find(".SER") != std::string::npos) {
        expandedLine = Utils::replaceToken(expandedLine, ".SER", currentSerial);
    }

    return expandedLine;
}
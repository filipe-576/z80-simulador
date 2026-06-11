#include "pass_one.h"
#include "utils.h"
#include <sstream>
#include <cctype>
#include <cstdint>

bool PassOne::execute(std::istream& input, std::ostream& intermediateOutput, MacroContext& ctx) {
    std::string rawLine;
    currentState = State::NORMAL;

    while (std::getline(input, rawLine)) {
        std::string cleanLine = Utils::stripComments(rawLine);
        
        // Linhas em branco limpas não devem ser processadas por lógica, mas apenas copiadas
        // para garantir que numerações de erro de debug batam com o arquivo fonte real.
        if (Utils::trim(cleanLine).empty()) {
            if (currentState == State::NORMAL) {
                intermediateOutput << "\n";
            }
            continue;
        }

        std::string label, opcode, operands;
        Utils::parseAssemblyLine(cleanLine, label, opcode, operands);

        if (currentState == State::NORMAL) {
            
            if (opcode == "MACRO") {
                if (label.empty()) {
                    std::cerr << "[Erro - Passo 1] Definicao de macro sem rotulo.\n";
                    return false; 
                }

                currentState = State::DEFINITION;
                currentMacroName = label;
                currentFormalParams = extractFormalParameters(operands);

                MacroNameEntry entry;
                entry.numParameters = static_cast<uint16_t>(currentFormalParams.size());
                
                ctx.macroTable[currentMacroName] = entry; 
            } else {
                intermediateOutput << rawLine << "\n";
            }

        } else if (currentState == State::DEFINITION) {
            
            // Aceita o encerramento da definição tanto se o programador colocar espaço no início da linha,
            // quanto se colocar grudado à esquerda (o que o parser considera label).
            if (opcode == "ENDM" || label == "ENDM") {
                currentState = State::NORMAL;
                currentMacroName.clear();
                currentFormalParams.clear();
            } else {
                std::string processedLine = replaceFormalWithPositional(cleanLine, currentFormalParams);
                ctx.macroTable[currentMacroName].definition.lines.push_back(processedLine);
            }

        }
    }

    if (currentState == State::DEFINITION) {
        std::cerr << "[Erro - Passo 1] Arquivo terminou sem fechar a definicao da macro: " << currentMacroName << "\n";
        return false;
    }

    return true;
}


std::vector<std::string> PassOne::extractFormalParameters(const std::string& operandsPart) {
    std::vector<std::string> params;
    std::vector<std::string> rawParams = Utils::splitOperands(operandsPart, ',');
    
    for (const auto& parameter : rawParams) {
        std::string trimmed = Utils::trim(parameter);
        
        if (!trimmed.empty()) {
            params.push_back(trimmed);
        }
    }

    return params;
}


std::string PassOne::replaceFormalWithPositional(const std::string& line, const std::vector<std::string>& formalParams) {
    std::string result = line;

    // Converte &A, &B para tokens lógicos genéricos (#1, #2) no código guardado,
    // garantindo que não importa qual ordem ou nome o programador usou.
    for (uint16_t i = 0; i < formalParams.size(); ++i) {
        const std::string& target = formalParams[i];
        std::string replacement = "#" + std::to_string(i + 1);

        result = Utils::replaceToken(result, target, replacement);
    }

    return result;
}
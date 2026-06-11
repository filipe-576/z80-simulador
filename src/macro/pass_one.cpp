#include "pass_one.h"
#include "utils.h"
#include <sstream>
#include <cctype>

bool PassOne::execute(std::istream& input, std::ostream& intermediateOutput, MacroContext& ctx) {
    std::string rawLine;
    currentState = State::NORMAL;

    while (std::getline(input, rawLine)) {
        // Remove comentários e espaços excedentes nas pontas
        std::string cleanLine = Utils::stripComments(rawLine);
        
        // Linhas em branco (após remover comentários) podem ser ignoradas
        if (Utils::trim(cleanLine).empty()) {
            // imprime a linha vazia no intermediário para manter numeração de linhas
            if (currentState == State::NORMAL) {
                intermediateOutput << "\n";
            }
            continue;
        }

        std::string label, opcode, operands;
        Utils::parseAssemblyLine(cleanLine, label, opcode, operands);

        if (currentState == State::NORMAL) {
            if (opcode == "MACRO") {
                // Validação de sintaxe: macro precisa de um nome (rótulo)
                if (label.empty()) {
                    std::cerr << "[Erro - Passo 1] Definicao de macro sem rotulo.\n";
                    return false; 
                }

                // Transição de estado
                currentState = State::DEFINITION;
                currentMacroName = label;
                
                // Extrai e armazena os parâmetros formais
                currentFormalParams = extractFormalParameters(operands);

                // Inicializa a entrada na MNT (Macro Name Table)
                MacroNameEntry entry;
                entry.num_parameters = currentFormalParams.size();
                ctx.macroTable[currentMacroName] = entry; 
                // A MDT (definition.lines) começa vazia automaticamente
            } else {
                // Qualquer outra instrução é copiada exatamente como está para o arquivo intermediário
                intermediateOutput << rawLine << "\n";
            }
        } 
        else if (currentState == State::DEFINITION) {
            if (opcode == "ENDM" || label == "ENDM") {
                // Fim da definição, retorna ao modo de cópia (NORMAL)
                currentState = State::NORMAL;
                currentMacroName.clear();
                currentFormalParams.clear();
            } else {
                // Trata as linhas do corpo da macro (MDT)
                // Substitui os parâmetros formais por posicionais antes de salvar
                std::string processedLine = replaceFormalWithPositional(cleanLine, currentFormalParams);
                ctx.macroTable[currentMacroName].definition.lines.push_back(processedLine);
            }
        }
    }

    // Se o arquivo acabar e ainda estivermos esperando um ENDM, há um erro de sintaxe no ASM original
    if (currentState == State::DEFINITION) {
        std::cerr << "[Erro - Passo 1] Arquivo terminou sem fechar a definicao da macro: " << currentMacroName << "\n";
        return false;
    }

    return true;
}

std::vector<std::string> PassOne::extractFormalParameters(const std::string& operandsPart) {
    std::vector<std::string> params;
    
    // Utiliza a função do utils.hpp que separa por vírgula respeitando espaços
    std::vector<std::string> rawParams = Utils::splitOperands(operandsPart, ',');
    
    for (const auto& p : rawParams) {
        std::string trimmed = Utils::trim(p);
        if (!trimmed.empty()) {
            params.push_back(trimmed);
        }
    }
    return params;
}

std::string PassOne::replaceFormalWithPositional(const std::string& line, const std::vector<std::string>& formalParams) {
    std::string result = line;

    // Itera sobre a lista de parâmetros formais
    for (size_t i = 0; i < formalParams.size(); ++i) {
        const std::string& target = formalParams[i];
        
        // O marcador posicional será #1, #2, #3...
        std::string replacement = "#" + std::to_string(i + 1);

        // O replaceToken verifica word boundaries para não substituir "&AB" ao procurar por "&A"
        result = Utils::replaceToken(result, target, replacement);
    }

    return result;
}
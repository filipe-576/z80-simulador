#include "pass_one.hpp"
#include "utils.hpp" // Assumindo que você declarou as funções em namespace Utils
#include <sstream>
#include <cctype>

// Função auxiliar interna para separar Rótulo, Instrução e Operandos
// Uma regra clássica de montadores: se o primeiro caractere não é espaço, é um rótulo.
static void parseAssemblyLine(const std::string& line, std::string& label, std::string& opcode, std::string& operands) {
    label.clear();
    opcode.clear();
    operands.clear();

    if (line.empty()) return;

    size_t pos = 0;

    // 1. Extrai o Rótulo (se houver)
    if (!std::isspace(line[0])) {
        size_t endLabel = line.find_first_of(" \t");
        label = line.substr(0, endLabel);
        pos = endLabel;
    }

    // 2. Extrai o Opcode (Instrução)
    if (pos != std::string::npos) {
        size_t startOpcode = line.find_first_not_of(" \t", pos);
        if (startOpcode != std::string::npos) {
            size_t endOpcode = line.find_first_of(" \t", startOpcode);
            opcode = line.substr(startOpcode, endOpcode - startOpcode);
            pos = endOpcode;
        } else {
            pos = std::string::npos;
        }
    }

    // 3. Extrai os Operandos
    if (pos != std::string::npos) {
        size_t startOperands = line.find_first_not_of(" \t", pos);
        if (startOperands != std::string::npos) {
            operands = Utils::trim(line.substr(startOperands));
        }
    }

    // Normaliza o opcode para maiúsculas para evitar erros de case sensitivity (ex: "Macro" vs "MACRO")
    opcode = Utils::toUpperCase(opcode);
}

bool PassOne::execute(std::istream& input, std::ostream& intermediateOutput, MacroContext& ctx) {
    std::string rawLine;
    currentState = State::NORMAL;

    while (std::getline(input, rawLine)) {
        // Remove comentários e espaços excedentes nas pontas
        std::string cleanLine = Utils::trim(Utils::stripComments(rawLine));
        
        // Linhas em branco pufas (após remover comentários) podem ser ignoradas
        if (cleanLine.empty()) {
            // Opcional: imprimir a linha vazia no intermediário para manter numeração de linhas
            if (currentState == State::NORMAL) {
                intermediateOutput << "\n";
            }
            continue;
        }

        std::string label, opcode, operands;
        parseAssemblyLine(cleanLine, label, opcode, operands);

        if (currentState == State::NORMAL) {
            if (opcode == "MACRO") {
                // Validação de sintaxe crítica: Uma macro precisa de um nome (rótulo)
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
            if (opcode == "ENDM") {
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

        // O replaceToken DEVE verificar word boundaries para não substituir "&AB" ao procurar por "&A"
        result = Utils::replaceToken(result, target, replacement);
    }

    return result;
}
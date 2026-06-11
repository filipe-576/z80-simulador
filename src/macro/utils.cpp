#include "utils.h"
#include <algorithm>
#include <cctype>

namespace Utils {

    // Função interna para verificar se um caractere faz parte de um identificador/token.
    // Em Assembly de macros, consideramos letras, números, underscore e o '&'.
    static bool isWordChar(char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '&';
    }

    std::string trim(const std::string& str) {
        if (str.empty()) return str;

        auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char c) {
            return std::isspace(c);
        });

        if (start == str.end()) return ""; // String apenas com espaços

        auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char c) {
            return std::isspace(c);
        }).base();

        return std::string(start, end);
    }

    std::string toUpperCase(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
            return std::toupper(c);
        });
        return result;
    }

    std::string stripComments(const std::string& line) {
        bool inQuotes = false;
        for (size_t i = 0; i < line.length(); ++i) {
            if (line[i] == '\'') {
                inQuotes = !inQuotes; // Alterna o estado ao encontrar aspas simples
            } else if (line[i] == ';' && !inQuotes) {
                // Se encontrar um ';' fora de aspas, corta a string aqui
                return line.substr(0, i);
            }
        }
        return line;
    }

    std::vector<std::string> splitOperands(const std::string& operandString, char delimiter) {
        std::vector<std::string> tokens;
        std::string currentToken;
        bool inQuotes = false;

        for (char c : operandString) {
            if (c == '\'') {
                inQuotes = !inQuotes;
                currentToken += c;
            } else if (c == delimiter && !inQuotes) {
                // Delimitador encontrado fora de aspas: guarda o token atual e limpa
                tokens.push_back(trim(currentToken));
                currentToken.clear();
            } else {
                currentToken += c;
            }
        }

        // Adiciona o último token (se existir)
        std::string finalToken = trim(currentToken);
        if (!finalToken.empty() || !tokens.empty()) {
            tokens.push_back(finalToken);
        }

        return tokens;
    }

    std::string replaceToken(const std::string& source, const std::string& target, const std::string& replacement) {
        if (target.empty()) return source;

        std::string result = source;
        size_t pos = 0;

        while ((pos = result.find(target, pos)) != std::string::npos) {
            bool isExactMatch = true;

            // Verifica o limite esquerdo do token
            if (pos > 0) {
                char prevChar = result[pos - 1];
                if (isWordChar(prevChar)) {
                    isExactMatch = false; // O token encontrado é um sufixo de outra palavra
                }
            }

            // Verifica o limite direito do token
            size_t nextPos = pos + target.length();
            if (isExactMatch && nextPos < result.length()) {
                char nextChar = result[nextPos];
                if (isWordChar(nextChar)) {
                    isExactMatch = false; // O token encontrado é um prefixo de outra palavra
                }
            }

            // Substitui apenas se for uma palavra isolada
            if (isExactMatch) {
                result.replace(pos, target.length(), replacement);
                pos += replacement.length(); // Avança o tamanho do novo texto
            } else {
                pos += target.length(); // Avança para continuar a procura sem substituir
            }
        }

        return result;
    }

    // Função auxiliar interna para separar Rótulo, Instrução e Operandos
    // Uma regra clássica de montadores: se o primeiro caractere não é espaço, é um rótulo.
    void parseAssemblyLine(const std::string& line, std::string& label, std::string& opcode, std::string& operands) {
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

        // Normalização para evitar erros de case sensitivity (ex: "Macro" vs "MACRO")
        opcode = Utils::toUpperCase(opcode);
        label = Utils::toUpperCase(label);
    }

} // namespace Utils
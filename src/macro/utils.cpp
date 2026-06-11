#include "utils.h"
#include <algorithm>
#include <cctype>
#include <cstdint>

namespace Utils {

    bool isWordChar(char character) {
        // Z80 Assembly e o Processador usam letras, números, sublinhado e o caractere comercial para definir blocos.
        return std::isalnum(static_cast<unsigned char>(character)) || character == '_' || character == '&';
    }


    std::string trim(const std::string& str) {
        if (str.empty()) {
            return str;
        }

        auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char character) {
            return std::isspace(character);
        });

        if (start == str.end()) {
            return "";
        }

        auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char character) {
            return std::isspace(character);
        }).base();

        return std::string(start, end);
    }


    std::string toUpperCase(const std::string& str) {
        std::string result = str;

        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char character) {
            return std::toupper(character);
        });

        return result;
    }


    std::string stripComments(const std::string& line) {
        bool inQuotes = false;

        for (uint32_t i = 0; i < line.length(); ++i) {
            // Alterna o estado (toggle) para ignorar falsos positivos de comentários dentro de strings no Assembly.
            if (line[i] == '\'') {
                inQuotes = !inQuotes;
            } else if (line[i] == ';' && !inQuotes) {
                return line.substr(0, i);
            }
        }

        return line;
    }


    std::vector<std::string> splitOperands(const std::string& operandString, char delimiter) {
        std::vector<std::string> tokens;
        std::string currentToken;
        bool inQuotes = false;

        for (char character : operandString) {
            // Aspas protegem o delimitador de fatiar uma string contínua que faz parte da instrução.
            if (character == '\'') {
                inQuotes = !inQuotes;
                currentToken += character;
            } else if (character == delimiter && !inQuotes) {
                tokens.push_back(trim(currentToken));
                currentToken.clear();
            } else {
                currentToken += character;
            }
        }

        std::string finalToken = trim(currentToken);

        if (!finalToken.empty() || !tokens.empty()) {
            tokens.push_back(finalToken);
        }

        return tokens;
    }


    std::string replaceToken(const std::string& source, const std::string& target, const std::string& replacement) {
        if (target.empty()) {
            return source;
        }

        std::string result = source;
        size_t pos = 0;

        // Itera para substituir todas as ocorrências de um token posicional durante a expansão da macro.
        while ((pos = result.find(target, pos)) != std::string::npos) {
            bool isExactMatch = true;

            // Blinda contra "falsos positivos" (ex: substituir #1 em #10) checando os limites do token à esquerda.
            if (pos > 0) {
                char prevChar = result[pos - 1];
                if (isWordChar(prevChar)) {
                    isExactMatch = false;
                }
            }

            // Blinda checando limites à direita.
            size_t nextPos = pos + target.length();

            if (isExactMatch && nextPos < result.length()) {
                char nextChar = result[nextPos];
                if (isWordChar(nextChar)) {
                    isExactMatch = false;
                }
            }

            if (isExactMatch) {
                result.replace(pos, target.length(), replacement);
                pos += replacement.length();
            } else {
                pos += target.length();
            }
        }

        return result;
    }


    void parseAssemblyLine(const std::string& line, std::string& label, std::string& opcode, std::string& operands) {
        label.clear();
        opcode.clear();
        operands.clear();

        if (line.empty()) {
            return;
        }

        size_t pos = 0;

        // O Assembly determina que a coluna zero (sem espaços) pertence a rótulos.
        if (!std::isspace(line[0])) {
            size_t endLabel = line.find_first_of(" \t");
            label = line.substr(0, endLabel);
            pos = endLabel;
        }

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

        if (pos != std::string::npos) {
            size_t startOperands = line.find_first_not_of(" \t", pos);
            if (startOperands != std::string::npos) {
                operands = Utils::trim(line.substr(startOperands));
            }
        }

        opcode = Utils::toUpperCase(opcode);
        label = Utils::toUpperCase(label);
    }

}
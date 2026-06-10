#include "utils.hpp"
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

} // namespace Utils
#pragma once
#include <string>
#include <vector>

namespace Utils {
    std::string trim(const std::string& str);
    std::string toUpperCase(const std::string& str);
    
    // Essencial para remover "; comentarios" antes do split de parametros
    std::string stripComments(const std::string& line);
    
    // Quebra separando por delimitador, mas ignora espaços em branco
    std::vector<std::string> splitOperands(const std::string& operandString, char delimiter = ',');

    // Verifica limites de palavras para evitar que a substituição de &A afete &AB
    std::string replaceToken(const std::string& source, const std::string& target, const std::string& replacement);

    // Nova função global para ler a linha assembly corretamente
    void parseAssemblyLine(const std::string& line, std::string& label, std::string& opcode, std::string& operands);
}

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

class MacroProcessor {
public:
    /**
    * @brief Lê as definiçoes de macro e popula as MDT e MNT
    *
    * @param source_lines Código fonte
    * @return Código intermediario sem as definições de macro
    */
    std::vector<std::string> step1(const std::vector<std::string>& source_lines);


    /**
    * @brief   Remove espaços em branco do início e fim de uma string.
    *
    * @param   line  String de entrada.
    * @return  String sem espaços em branco no início e fim.
    */
    std::string trim(std::string line);


    /**
    * @brief   Divide uma string em tokens baseados em espaços em branco.
    *
    * @param   line  String de entrada.
    * @return  Vetor de strings contendo os tokens.
    */
    std::vector<std::string> tokenize(std::string line);


    /**
    * @brief   Substitui todas as ocorrências de uma substring por outra em uma string de destino.
    *
    * @param   str  String de destino.
    * @param   target  Substring a ser substituída.
    * @param   replacement  Substring substituta.
    */
    void replaceAll(std::string& str, const std::string& target, const std::string& replacement);

private:
    std::unordered_map<std::string, size_t> macroNameTable;

    std::vector<std::vector<std::string>> macroDefinitionTable;

    uint16_t serialCounter = 0;
};
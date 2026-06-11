#pragma once

#include <string>
#include <vector>

namespace Utils {

    /**
     * @brief  Remove espaços em branco isolando os tokens da string.
     *
     * @param  str  String original a ser processada.
     * @return      String limpa para facilitar o parsing da máquina de estados.
     */
    std::string trim(const std::string& str);


    /**
     * @brief  Padroniza a string inteira para maiúsculas.
     *
     * @param  str  String original.
     * @return      String processada para evitar erros de case sensitivity no Assembly.
     */
    std::string toUpperCase(const std::string& str);


    /**
     * @brief  Remove fragmentos de comentários após o ponto e vírgula (;).
     *
     * @param  line  Linha de código do arquivo fonte.
     * @return       Instrução pura que será interpretada pelo montador.
     */
    std::string stripComments(const std::string& line);


    /**
     * @brief  Segmenta a string de operandos com base no caractere delimitador.
     *
     * @param  operandString  Texto completo contendo os argumentos.
     * @param  delimiter      Símbolo usado para quebrar a string (padrão: vírgula).
     * @return                Vetor de tokens extraídos.
     */
    std::vector<std::string> splitOperands(const std::string& operandString, char delimiter = ',');


    /**
     * @brief  Substitui tokens específicos respeitando os limites da palavra (word boundaries).
     *
     * @param  source       String de origem.
     * @param  target       Padrão exato a ser encontrado.
     * @param  replacement  Conteúdo a ser injetado.
     * @return              Nova string com o token substituído sem afetar prefixos ou sufixos semelhantes.
     */
    std::string replaceToken(const std::string& source, const std::string& target, const std::string& replacement);


    /**
     * @brief  Identifica e particiona uma instrução nas suas três colunas morfológicas.
     *
     * @param  line      Instrução bruta do arquivo Assembly.
     * @param  label     Referência preenchida caso exista um rótulo no prefixo da instrução.
     * @param  opcode    Referência preenchida com o mnemônico ou diretiva.
     * @param  operands  Referência preenchida com as variáveis ou endereços de apoio da instrução.
     */
    void parseAssemblyLine(const std::string& line, std::string& label, std::string& opcode, std::string& operands);

}
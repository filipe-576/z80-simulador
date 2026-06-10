#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "macro_context.hpp"

/**
 * @brief Responsável pela segunda passagem do processador de macros.
 * * O PassTwo consome o código intermediário gerado pelo PassOne e, 
 * utilizando o MacroContext, expande as chamadas de macros para código 
 * Assembly puro, tratando parâmetros reais e serialização de rótulos.
 */
class PassTwo {
private:
    enum class State { NORMAL, EXPANSION };
    
    State currentState;
    int serialCounter; // Contador global para gerar rótulos únicos (ex: .SER -> 001)

    // ALA (Argument List Array) - Armazena os parâmetros reais da chamada atual.
    // Esta estrutura é transiente e recriada a cada expansão de macro.
    std::vector<std::string> ala;

    /**
     * @brief Constrói a ALA a partir da string de operandos de uma chamada de macro.
     * @param operandsPart String contendo os argumentos separados por vírgula.
     */
    void buildALA(const std::string& operandsPart);

    /**
     * @brief Substitui marcadores posicionais (#1, #2) por valores da ALA e rótulos (.SER).
     * @param skeletonLine Uma linha retirada da MDT (Tabela de Definição).
     * @return Linha processada pronta para o arquivo de saída.
     */
    std::string expandLine(const std::string& skeletonLine);

public:
    PassTwo() : currentState(State::NORMAL), serialCounter(0) {}

    /**
     * @brief Executa a segunda passagem.
     * * @param intermediateInput Stream contendo as chamadas de macro e código normal.
     * @param finalOutput Stream onde será gravado o código Assembly final.
     * @param ctx Referência ao contexto preenchido pela Passagem 1.
     * @return true se a expansão foi bem-sucedida, false em caso de erro.
     */
    bool execute(std::istream& intermediateInput, std::ostream& finalOutput, const MacroContext& ctx);
};
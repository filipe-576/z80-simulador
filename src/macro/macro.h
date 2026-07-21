#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <cstdint>

struct Macro {
    std::string name;
    std::vector<std::string> parameters;
    std::vector<std::string> body;
};

class MacroProcessor {
public:
    /**
     * @brief  Inicializa o processador de macros e carrega o arquivo fonte em memória.
     *
     * @param  inputFileName   Caminho para o arquivo assembly de entrada.
     * @param  outputFileName  Caminho para o arquivo de saída pré-processado.
     */
    MacroProcessor(const std::string& inputFileName, const std::string& outputFileName);

    /**
     * @brief  Executa o processamento de macros em passagem única sobre o código fonte.
     *
     * Utiliza uma fila dinâmica (deque) para processar definições e expandir chamadas
     * de macros na mesma passagem. Linhas expandidas são reinseridas na frente da fila,
     * permitindo o tratamento natural de macros aninhadas sem múltiplas passagens.
     * Ao final, grava o código expandido no arquivo de saída.
     */
    void process();

private:
    std::string inputFileName;
    std::string outputFileName;

    // Linhas do arquivo fonte carregadas em memória pelo loadFile()
    std::vector<std::string> program;

    // MNT (Macro Name Table): nome da macro -> struct Macro com seus dados
    std::unordered_map<std::string, Macro> MNT;

    // Conjunto dos nomes das macros definidas para busca rápida O(log n)
    std::set<std::string> macroNames;

    // Contador incremental para geração de rótulos únicos via .SER
    uint16_t serialCounter = 0;

    /**
     * @brief  Lê o arquivo de entrada linha por linha e popula o vetor program.
     */
    void loadFile();

    /**
     * @brief  Grava o código pré-processado no arquivo de saída.
     *
     * @param  preprocessedProgram  Vetor de linhas do código final expandido.
     */
    void saveFile(const std::vector<std::string>& preprocessedProgram);

    /**
     * @brief  Substitui todas as ocorrências de uma substring por outra dentro de uma string.
     *
     * Avança a posição de busca após cada substituição para evitar loops infinitos
     * em casos onde o substituto contém o próprio alvo.
     *
     * @param  str          String de destino modificada por referência.
     * @param  target       Substring a ser localizada e substituída.
     * @param  replacement  Substring que será inserida no lugar do target.
     */
    void replaceAll(std::string& str, const std::string& target, const std::string& replacement);
};
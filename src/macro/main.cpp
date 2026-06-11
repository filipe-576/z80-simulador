#include <iostream>
#include <fstream>
#include <sstream>
#include "macro_context.h"
#include "pass_one.h"
#include "pass_two.h"

// para testar: g++ main.cpp pass_one.cpp pass_two.cpp utils.cpp -o montador_macros -Wall -Wextra
int main() {
    /*
    // Abra o arquivo de texto que contém o código do programador
    std::ifstream inputFile("codigo_fonte.asm");
    if (!inputFile.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo de entrada.\n";
        return 1;
    }

    // Usamos stringstream para guardar o arquivo intermediário na RAM
    std::stringstream intermediateStream;
    std::ofstream outputFile("codigo_expandido.asm");

    MacroContext context;
    PassOne passOne;
    PassTwo passTwo;

    std::cout << "[INFO] Iniciando Passagem 1 (Extracao de Macros)...\n";
    if (!passOne.execute(inputFile, intermediateStream, context)) {
        std::cerr << "[FALHA] Erro na Passagem 1.\n";
        return 1;
    }

    // Retorna o "cursor" de leitura da memória para o início antes do Passo 2
    intermediateStream.clear();
    intermediateStream.seekg(0, std::ios::beg);

    std::cout << "[INFO] Iniciando Passagem 2 (Expansao)...\n";
    if (!passTwo.execute(intermediateStream, outputFile, context)) {
        std::cerr << "[FALHA] Erro na Passagem 2.\n";
        return 1;
    }

    std::cout << "[SUCESSO] Processamento de macros concluido. Arquivo gerado.\n";
    return 0;
    */
}
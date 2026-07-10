#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

class Linker {
    public:
        Linker(const std::string& file);
        
        /**
         * @brief Executa o primeiro e o segundo passo do ligador
         */
        void Link();

        /**
         * @brief Exibe o código de máquina após o segundo passo
         */
        void linkDebug();

        /**
         * @brief Exibe a Tabela Global de Simbolo
         */
        void printSymbolTable();

    private:
        struct Module {
            std::string name;
            int size;
            std::unordered_map<std::string, int> definitionTable; //INTDEF
            std::unordered_map<std::string, std::vector<int>> useTable; //INTUSE
            std::vector<int> machineCode;
            std::vector<int> realocationMap; 
        };

        std::vector<Module> listModules;
        std::unordered_map<std::string, int> globalSymbolTable; // TSG
        std::unordered_map<std::string, int> modulesOffsets; 

        void oPrimeiroPassoDosLinkerBoys();
        void oSegundoPassoDosLinkerBoys();

        /**
         * @brief Lê a saída do assembler, um json com informações dos códigos a serem ligados
         */
        void loadJson(const std::string& fileName); 
};

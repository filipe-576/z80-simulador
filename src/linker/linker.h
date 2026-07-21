#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

class Linker {
    public:
        Linker(const std::vector<std::string>& file, int baseAddress = 0);
        
        /**
         * @brief   Executa a ligação dos módulos objeto
         */
        void Link(const std::string& outputFileName);

        void linkDebug();

    private:
        struct Module {
            std::string name;
            int size;
            std::unordered_map<std::string, int> definitionTable;
            std::unordered_map<std::string, std::vector<int>> useTable;
            std::vector<int> machineCode;
            std::vector<int> relocationMap;
            std::uint16_t entryPoint;
        };

        int baseAddress;

        std::vector<Module> listModules;
        std::unordered_map<std::string, int> globalSymbolTable;
        std::unordered_map<std::string, int> modulesOffsets; 

        /**
         * @brief   Calcula o offset de cada módulo e constrói
         *          a tabela global de símbolos com endereços absolutos.
         */
        void oPrimeiroPassoDosLinkerBoys();

        /**
         * @brief   Aplica realocação nos endereços relativos e
         *          resolve referências externas usando a tabela global de símbolos.
         */
        void oSegundoPassoDosLinkerBoys();

        void loadJson(const std::string& fileName);

        void linkerBoysOut(const std::string& outputFileName);
};

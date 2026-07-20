#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

class Linker {
    public:
        Linker(const std::string& file, int baseAddress = 0);
        
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

        void oPrimeiroPassoDosLinkerBoys();
        void oSegundoPassoDosLinkerBoys();
        void loadJson(const std::string& fileName); 
        void linkerBoysOut(const std::string& outputFileName);
};

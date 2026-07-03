#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

class Linker {
    public:
        Linker(const std::string& file);
        
        void Link();
        void linkDebug();

    private:
        struct Module {
            std::string name;
            int size;
            std::unordered_map<std::string, int> definitionTable;
            std::unordered_map<std::string, std::vector<int>> useTable; 
            std::vector<int> machineCode;
            std::vector<int> realocationMap; 
        };

        std::vector<Module> listModules;
        std::unordered_map<std::string, int> globalSymbolTable; 
        std::unordered_map<std::string, int> modulesOffsets; 

        void oPrimeiroPassoDosLinkerBoys();
        void oSegundoPassoDosLinkerBoys();

        void loadJson(const std::string& fileName); 
}; 

#include "linker.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>     
#include "json.hpp"

using json = nlohmann::json;

Linker::Linker(const std::string& fileName){
    loadJson(fileName);
}

void Linker::Link() {
    oPrimeiroPassoDosLinkerBoys();
    oSegundoPassoDosLinkerBoys();
}

void Linker::linkDebug(){
    for (const auto& modulo : listModules) {
        std::cout << "Modulo " << modulo.name << ": [ ";
        for (int instrucao : modulo.machineCode) {
            std::cout << instrucao << " ";
        }
        std::cout << "]\n";
    }
}


void Linker::oPrimeiroPassoDosLinkerBoys(){

    int globalOffset = 0;

    for (const auto & module : listModules){
        modulesOffsets[module.name] = globalOffset;

        for (const auto & symbol : module.definitionTable){
            std::string symbolName = symbol.first;

            globalSymbolTable[symbolName] = symbol.second + globalOffset; //insere na TSG o endereço que tá na tabela de definição + o offset atual desde o ínicio da linkagem
        }

        globalOffset += module.size;
    
    }
}

void Linker::oSegundoPassoDosLinkerBoys(){

    for (auto & module : listModules){
        int offset = modulesOffsets[module.name];

        for (int index : module.realocationMap){
            module.machineCode[index] = module.machineCode[index] + offset; // atualiza endereços relativos
        }

        for (auto & useTablePair : module.useTable){
            std::string symbolName = useTablePair.first;
            std::vector<int> indexList = useTablePair.second;

            for (int useIndex : indexList){
                module.machineCode[useIndex] = globalSymbolTable[symbolName]; // coloca os endereços reais dos símbolos no código de máquina
            }
        }

    }
}

void Linker::loadJson(const std::string& fileName) {
    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o file " << fileName << "!\n";
        return;
    }

    json dados_json;
    file >> dados_json; 

    for (const auto& item : dados_json) {
        Module mod;
        mod.name = item["name"];
        mod.size = item["size"];

        for (auto& el : item["definitionTable"].items()) {
            mod.definitionTable[el.key()] = el.value();
        }

        for (auto& el : item["useTable"].items()) {
            mod.useTable[el.key()] = el.value().get<std::vector<int>>();
        }

        mod.machineCode = item["machineCode"].get<std::vector<int>>();
        mod.realocationMap = item["realocationMap"].get<std::vector<int>>();

        listModules.push_back(mod);
    }
}
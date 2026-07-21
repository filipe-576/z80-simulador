#include "linker.h"
#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>     
#include "../json.hpp"

using json = nlohmann::json;

Linker::Linker(const std::vector<std::string>& fileNames, int baseAddress)
    : baseAddress(baseAddress) {
    for (const auto& fileName : fileNames) {
        loadJson(fileName);
    }
}

void Linker::Link(const std::string& outputFileName) {
    oPrimeiroPassoDosLinkerBoys();
    oSegundoPassoDosLinkerBoys();
    linkerBoysOut(outputFileName);
}

void Linker::linkDebug(){
    std::cout << "\n======= Tabela Global de Simbolos =======\n";
    std::cout << "Símbolo\t\t|\tEndereço Absoluto\n";
    std::cout << "-----------------------------------------------\n";
    for (const auto& pair : globalSymbolTable) {
        std::cout << pair.first << "\t\t|\t" << pair.second << "\n";
    }

    std::cout << "===============================================\n\n";
    
    for (const auto& modulo : listModules) {
        std::cout << "Modulo " << modulo.name << ": [ ";
        for (int instrucao : modulo.machineCode) {
            std::cout << instrucao << " ";
        }
        std::cout << "]\n";
    }
}

void Linker::oPrimeiroPassoDosLinkerBoys(){
    int globalOffset = baseAddress;

    for (const auto & module : listModules){
        modulesOffsets[module.name] = globalOffset;

        for (const auto & symbol : module.definitionTable){
            std::string symbolName = symbol.first;
            globalSymbolTable[symbolName] = symbol.second + globalOffset;
        }

        globalOffset += module.size;
    }
}

void Linker::oSegundoPassoDosLinkerBoys(){
    for (auto & module : listModules){
        int offset = modulesOffsets[module.name];

        for (int index : module.relocationMap){
            uint16_t low = module.machineCode[index];
            uint16_t high = module.machineCode[index + 1];
            uint16_t address = (high << 8) | low;
            address += offset;
            module.machineCode[index] = address & 0xFF;
            module.machineCode[index + 1] = (address >> 8) & 0xFF;
        }

        for (auto & useTablePair : module.useTable){
            std::string symbolName = useTablePair.first;
            std::vector<int> indexList = useTablePair.second;

            for (int useIndex : indexList){
                uint16_t address = globalSymbolTable[symbolName];
                module.machineCode[useIndex]     = address & 0xFF;  
                module.machineCode[useIndex + 1] = (address >> 8) & 0xFF;
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

    Module mod;
    mod.name = dados_json["name"];
    mod.size = dados_json["size"];

    for (auto& el : dados_json["definitionTable"].items()) {
        mod.definitionTable[el.key()] = el.value();
    }

    for (auto& el : dados_json["useTable"].items()) {
        mod.useTable[el.key()] = el.value().get<std::vector<int>>();
    }
    
    mod.machineCode = dados_json["machineCode"].get<std::vector<int>>();
    mod.relocationMap = dados_json["relocationMap"].get<std::vector<int>>();
    mod.entryPoint = dados_json["entryPoint"];

    listModules.push_back(mod);
    
}

void Linker::linkerBoysOut(const std::string& outputFileName){
    using json = nlohmann::json;
    json out_json;

    std::vector<uint8_t> finalMachineCode;
    for( const auto& mod : listModules ){
        finalMachineCode.insert( finalMachineCode.end(), mod.machineCode.begin(), mod.machineCode.end() );
    }

    std::vector<uint16_t> finalRelocationMap;
    for( const auto& mod : listModules ){
        int offset = modulesOffsets[mod.name];

        for (int index : mod.relocationMap) {
            finalRelocationMap.push_back(index + offset);
        }

        for( const auto& useTablePair : mod.useTable ){
            for( int useIndex : useTablePair.second ){
                finalRelocationMap.push_back(useIndex + offset);
            }
        }
    }

    uint16_t finalEntryPoint = 0;
    for( const auto& mod : listModules ){
        if( mod.entryPoint > 0 ){
            finalEntryPoint = mod.entryPoint + modulesOffsets[mod.name];
            break;
        }
    }

    out_json["machineCode"] = finalMachineCode;
    out_json["entryPoint"] = finalEntryPoint;
    out_json["baseAddress"] = baseAddress;
    out_json["relocationMap"] = finalRelocationMap;

    std::ofstream file(outputFileName);
    if (file.is_open()) {
        file << out_json.dump(4);
        file.close();
        std::cout << "Arquivo executável '" << outputFileName << "' gerado com sucesso!\n";
    } else {
        std::cerr << "Erro: Não foi possível criar o arquivo de saída " << outputFileName << "\n";
    }
}

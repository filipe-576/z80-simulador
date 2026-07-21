#include "memory.h"
#include "z80/cpu/cpu.h"
#include "gui.h"
#include <cstdint>
#include "json.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <bitset>


static void invalidCommand() {
    std::cerr << "Uso: ./z80-simulador <arquivo.out> [-r <enderecoBase>]\n";
}

/**
 * @brief Carrega na memória o programa e realoca os endereços caso escolhido.
 * @param realocate Caso verdade, função executa no modo carregador relativo. Senão, executa como carregador absoluto.
 * @param baseAddress Endereço base da memória para inserção do programa.
 * Caso esteja no modo de carregador absoluto, é substituido pelo endereço base definido pelo ligador.
**/
static void load(Memory& mem, std::string fileName, uint16_t& baseAddress, uint16_t& entryPoint, bool realocate){
    using json = nlohmann::json;
    std::ifstream file(fileName);
    if ( !file.is_open() ){
        std::cerr << "Erro: Não foi posível abrir o arquivo " << fileName << std::endl;
        return;
    }
    json jsonData;
    file >> jsonData;
    std::vector<uint8_t> program = jsonData["machineCode"];
    entryPoint = jsonData["entryPoint"];
    if( !realocate ){
        baseAddress = jsonData["baseAddress"]; // Usa o endereço base passado pelo ligador realocador
    } else{
        std::vector<uint16_t> relocationMap = jsonData["relocationMap"];
        for( const auto& add: relocationMap ){
            uint8_t low = program[add];
            uint8_t high = program[add+1];
            uint16_t relocatedAddress = ((high << 8) | low) + baseAddress;
    
            program[add] = relocatedAddress & 0xFF;
            program[add+1] = relocatedAddress >> 8; 
        }
    }

    mem.load(program.data(), program.size(), baseAddress);
}


int main(int argc, char* argv[]) {
    std::string input_file;
    uint16_t baseAddress = 0;
    uint16_t entryPoint;
    bool realocate = false;

    for( int i = 1; i < argc; ++i ){
        std::string arg = argv[i];
        if( arg == "-r" ){
            realocate = true;
            if( i+1 > argc ) invalidCommand();
            try{
                baseAddress = std::stoi(argv[++i]);
            } catch( const std::invalid_argument& ){
                invalidCommand();
                return 1;
            }
        } else if( input_file.empty() ){
            input_file = arg;
        } else{
            invalidCommand();
            return 1;
        }
    }
    
    if( input_file.find(".out") == std::string::npos ){
        invalidCommand();
        return 1;
    }

    Memory mem;
    CPU cpu(mem);
    GUI gui;    

    load(mem, input_file, baseAddress, entryPoint, realocate);

    cpu.getRegisters().PC = baseAddress + entryPoint;

    return gui.run_interface(mem,cpu);

}
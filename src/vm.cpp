#include "cpu.h"
#include <ios>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>

std::vector<uint8_t> getProgram(const std::string& fileName){
    std::ifstream file(fileName, std::ios::binary);
    if( !file ){
        throw std::runtime_error("Arquivo não encontrado");
    }

    std::vector<uint8_t> buffer;
    char byte;
    while( file.get(byte) ){ // Funciona pois get precisa de tipo char e char tem 8 bits
        buffer.push_back(static_cast<uint8_t>(byte));
    }

    file.close();
    return buffer;
}


int main(){

    std::string fileName("src/program.bin");

    Memory mem;
    CPU cpu(mem);

    std::vector<uint8_t> program = getProgram(fileName);

    mem.load(program.data(), program.size());

    while( !cpu.isHalted() ){
        cpu.step();
    }
}
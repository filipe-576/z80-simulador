#include "cpu.h"
#include <ios>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <bitset>
#include <gui.h>


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
    GUI gui;
    gui.run_interface(mem);


    std::vector<uint8_t> program = getProgram(fileName);

    mem.load(program.data(), program.size());

    while( !cpu.isHalted() ){
        cpu.step();
        Registers& regis = cpu.getRegisters();
        std::cout << "Regis.A: "<< std::bitset<8>(regis.A) << std::endl;
        std::cout << "Regis.B: "<< std::bitset<8>(regis.B) << std::endl;
        std::cout << "Regis.C: "<< std::bitset<8>(regis.C) << std::endl;
        std::cout << "Regis.D: "<< std::bitset<8>(regis.D) << std::endl;
        std::cout << "Regis.E: "<< std::bitset<8>(regis.E) << std::endl;
        std::cout << "Regis.H: "<< std::bitset<8>(regis.H) << std::endl;
        std::cout << "Regis.L: "<< std::bitset<8>(regis.L) << std::endl;

        std::cin.get();

    }
}
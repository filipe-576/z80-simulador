#include "assembler.h"
#include <fstream>
#include <iostream>

Assembler::Assembler(std::string fileName): fileName(fileName){
    loadFile();
}


void Assembler::loadFile(){
    std::ifstream file(fileName);
    std::string line;

    while(std::getline(file, line)){
        program.push_back(line);
    }
}

void Assembler::debug(){
    std::cout << "Código" << std::endl;
    for(std::string line: program){
        std::cout << line << std::endl;
    }

    std::cout << "Tabela de símbolos:" << std::endl;
    for(auto it = symbolTable.begin(); it != symbolTable.end(); it++){
        std::cout << it->first << " " << it->second << std::endl;
    }
}
#include "assembler.h"
#include <iostream>
#ifndef PROJ_DIR
#define PROJ_DIR "."
#endif


int main(){
    std::cout << std::string(PROJ_DIR) + "/src/assembler/test.asm";
    Assembler assemb = Assembler(std::string(PROJ_DIR) + "/src/assembler/test.asm");
    assemb.assemble();
    assemb.debug();
}
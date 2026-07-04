#include "assembler.h"
#ifndef PROJ_DIR
#define PROJ_DIR "."
#endif


int main(){
    Assembler assemb = Assembler(std::string(PROJ_DIR) + "/src/assembler/test2.asm");
    assemb.assemble();
    assemb.debug();
}
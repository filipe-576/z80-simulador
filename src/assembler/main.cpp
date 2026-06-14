#include "assembler.h"

int main(){
    Assembler assemb = Assembler("../src/assembler/test.asm");
    assemb.assemble();
    assemb.debug();
}
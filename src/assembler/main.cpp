#include "assembler.h"

int main(){
    Assembler assemb = Assembler("z80-simulador/src/assembler/test.asm");
    assemb.assemble();
    assemb.debug();
}
#include "assembler.h"

int main(){
    Assembler assemb = Assembler("test.asm");
    assemb.assemble();
    assemb.debug();
}
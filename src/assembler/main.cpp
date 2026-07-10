#include "assembler.h"
#include <iostream>
#include <string>


static void invalidCommand() {
    std::cerr << "Uso: ./assembler <arquivo.asm> [-o <saida>]\n";
}

int main(int argc, char* argv[]) {
    std::string input_file;
    std::string output_file = "a.o";

    for ( int i = 1; i < argc; ++i ) {
        std::string arg = argv[i];
        if ( arg == "-o" ) {
            if( i+1 > argc ) invalidCommand();
            output_file = argv[++i];
        } else if ( input_file.empty() ) {
            input_file = arg;
        } else {
            invalidCommand();
            return 1;
        }
    }

    if ( input_file.empty() ) {
        invalidCommand();
        return 1;
    }

    Assembler assemb(input_file, output_file);
    assemb.assemble();
    // assemb.debug();

    return 0;
}
#include "macro.h"
#include <iostream>

static void invalidCommand() {
    std::cerr << "Uso: ./assembler <arquivo.asm> [-o <saida>]\n";
}

int main(int argc, char* argv[]) {
    std::string inputFile;
    std::string outputFile = "processado.asm";

    for ( int i = 1; i < argc; ++i ) {
        std::string arg = argv[i];
        if ( arg == "-o" ) {
            if( i+1 > argc ) invalidCommand();
            outputFile = argv[++i];
        } else if ( inputFile.empty() ) {
            inputFile = arg;
        } else {
            invalidCommand();
            return 1;
        }
    }

    if ( inputFile.empty() ) {
        invalidCommand();
        return 1;
    }

    MacroProcessor macroProcessor(inputFile, outputFile);
    macroProcessor.process();

    return 0;
}
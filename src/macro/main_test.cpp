#include "macro.h"
#include <iostream>

int main() {
    std::string inputFile = "test.asm";
    std::string outputFile = "test_preprocessed.asm";

    MacroProcessor macroProcessor(inputFile, outputFile);
    macroProcessor.process();

    return 0;
}
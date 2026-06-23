#include "macro.h"
#include <fstream>
#include <iostream>

MacroProcessor::MacroProcessor(const std::string& inputFileName, const std::string& outputFileName) {
    this->inputFileName = inputFileName;
    this->outputFileName = outputFileName;

    loadFile();
}

void MacroProcessor::loadFile() {
    std::ifstream inputFile(inputFileName);
    std::string line;

    while (std::getline(inputFile, line)) {
        program.insert(program.end(), line);
    }
    inputFile.close();
}

void MacroProcessor::saveFile(const std::vector<std::string>& preprocessedProgram) {
    std::ofstream outputFile(outputFileName);

    for (const std::string& line : preprocessedProgram) {
        outputFile << line << std::endl;
    }
    outputFile.close();
}

void MacroProcessor::process() {
    std::vector<std::string> preprocessedProgram;

    for (size_t i = 0; i < program.size(); i++) {
        preprocessedProgram.insert(preprocessedProgram.end(), program[i]);
    }

    saveFile(preprocessedProgram);
}


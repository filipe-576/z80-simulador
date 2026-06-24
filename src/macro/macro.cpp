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

/*
std::string MacroProcessor::getLabel(const std::vector<std::string>& instruction) {
    if (instruction.empty()) {
        return "";
    }

    return instruction[0];
}

std::string MacroProcessor::getOpcode(const std::vector<std::string>& instruction) {
    if (instruction.empty()) {
        return "";
    }

    for (size_t i = 0; i < instruction.size(); i++) {
        if (instruction[i] == "MCDEFN" || instruction[i] == "MCEND") {
            return instruction[i];
        }
    }

    if (instruction.size() == 1) { // HALT, NOP, etc. (instruções sem label)
        return instruction[0];
    }

    return instruction[1];
}
*/

void MacroProcessor::process() {
    std::vector<std::string> preprocessedProgram;

    for (size_t i = 0; i < program.size(); i++) {
        std::vector<std::string> tokens = utils::tokenizeInstruction(program[i]);

        if (tokens.empty()) {
            continue; // Ignora linhas vazias
        }

        std::string label = utils::getLabel(tokens, macroNames);
        std::string opcode = utils::getOpcode(tokens, macroNames);

        preprocessedProgram.insert(preprocessedProgram.end(), program[i]); // Alterar .insert() para .push_back() em todo código.
    }

    saveFile(preprocessedProgram);
}


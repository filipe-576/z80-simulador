#include "macro.h"
#include <fstream>
#include <iostream>

// Falta a expansão das macros e a macroLevel para as definições aninhadas

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

    bool isMacro = false;
    Macro currentMacro;

    for (size_t i = 0; i < program.size(); i++) {
        std::vector<std::string> tokens = utils::tokenizeInstruction(program[i]);

        if (tokens.empty()) {
            continue; // Ignora linhas vazias
        }

        std::string label = utils::getLabel(tokens, macroNames);
        std::string opcode = utils::getOpcode(tokens, macroNames);

        if (opcode == "MCDEFN") {
            isMacro = true;

            currentMacro = Macro(); // Limpa a macro atual para não utilizar dados de uma macro anterior de uma vez só
            // currentMacro.name = "";
            // currentMacro.parameters.clear();
            // currentMacro.body.clear();
            currentMacro.name = label;

            for (const std::string& token : tokens) { // (para cada token dentro de tokens) substitui:
            // for (size_t i = 0; i < tokens.size(); i++) {
                // std::string token = tokens[i];
                if (token[0] == '&') {
                    currentMacro.parameters.push_back(token);
                }
            }

            continue;
        }

        if (isMacro) {
            if (opcode == "MCEND") {
                isMacro = false;

                MNT[currentMacro.name] = currentMacro;
                macroNames.insert(currentMacro.name);

                continue;
            } else {
                currentMacro.body.push_back(program[i]);
            }

            continue;
        }

        preprocessedProgram.insert(preprocessedProgram.end(), program[i]); // Alterar .insert() para .push_back() em todo código.
    }

    saveFile(preprocessedProgram);
}


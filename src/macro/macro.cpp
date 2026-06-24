#include "macro.h"
#include <fstream>
#include <iostream>

// Proximas etapas:
// 1. reconhecer a chamada de uma macro e colar as linhas do corpo dela na saída
// 2. mapear e associar os parâmetros da macro. Por exemplo: associar "B" a "&REG"
// 3. expansão da macro: fazer a substituição dos parâmetros
// 4. macros aninhadas?
// 5. ?

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
    int macroLevel = 0;
    Macro currentMacro;

    for (size_t i = 0; i < program.size(); i++) {
        std::vector<std::string> tokens = utils::tokenizeInstruction(program[i]);

        if (tokens.empty()) {
            continue; // Ignora linhas vazias
        }

        std::string label = utils::getLabel(tokens, macroNames);
        std::string opcode = utils::getOpcode(tokens, macroNames);

        // MODO DE EXPANSÃO:
        if (isMacro) {
            if (opcode == "MCDEFN") {
                macroLevel++;
                currentMacro.body.push_back(program[i]);
            }
            else if (opcode == "MCEND") {
                if (macroLevel > 0) {
                    macroLevel--;
                    currentMacro.body.push_back(program[i]);
                } else {
                    isMacro = false;

                    MNT[currentMacro.name] = currentMacro;
                    macroNames.insert(currentMacro.name);
                }
                continue;

            } else {
                currentMacro.body.push_back(program[i]);
            }

            continue;
        }

        // MODO DE DEFINIÇÃO:
        if (opcode == "MCDEFN") {
            isMacro = true;
            macroLevel = 0;

            currentMacro = Macro(); // Limpa a macro atual (de uma vez só) para não utilizar dados de uma macro anterior. Substitui:
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

        // MODO NORMAL:
        preprocessedProgram.insert(preprocessedProgram.end(), program[i]); // Alterar .insert() para .push_back() em todo código.
    }

    saveFile(preprocessedProgram);
}


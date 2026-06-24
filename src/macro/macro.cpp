#include "macro.h"
#include <fstream>
#include <iostream>

// Proximas etapas:
// [x] 1. reconhecer a chamada de uma macro e colar as linhas do corpo dela na saída
// [x] 2. mapear e associar os parâmetros da macro. Por exemplo: associar "B" a "&REG"
// [x] 3. expansão da macro: fazer a substituição dos parâmetros
// [x] 4. macros aninhadas?
// [x] 5. rótulos únicos .SER

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
    std::vector<std::string> intermediateCode;

    bool isMacro = false;
    int macroLevel = 0;
    Macro currentMacro;

    // === PASSO 1: Coleta de Definições ===
    for (size_t i = 0; i < program.size(); i++) {
        std::vector<std::string> tokens = utils::tokenizeInstruction(program[i]);

        if (tokens.empty()) {
            if (!isMacro) {
                intermediateCode.push_back(program[i]);
            }
            continue;
        }

        std::string label = utils::getLabel(tokens, macroNames);
        std::string opcode = utils::getOpcode(tokens, macroNames);

        if (isMacro) {
            if (opcode == "MCDEFN") {
                macroLevel++;
                std::string processedLine = program[i];
                for (size_t j = 0; j < currentMacro.parameters.size(); j++) {
                    replaceAll(processedLine, currentMacro.parameters[j], "?" + std::to_string(j));
                }
                currentMacro.body.push_back(processedLine);
            }
            else if (opcode == "MCEND") {
                if (macroLevel > 0) {
                    macroLevel--;
                    std::string processedLine = program[i];
                    for (size_t j = 0; j < currentMacro.parameters.size(); j++) {
                        replaceAll(processedLine, currentMacro.parameters[j], "?" + std::to_string(j));
                    }
                    currentMacro.body.push_back(processedLine);
                } else {
                    isMacro = false;
                    MNT[currentMacro.name] = currentMacro;
                    macroNames.insert(currentMacro.name);
                }
                continue;
            } else {
                std::string processedLine = program[i];
                for (size_t j = 0; j < currentMacro.parameters.size(); j++) {
                    replaceAll(processedLine, currentMacro.parameters[j], "?" + std::to_string(j));
                }
                currentMacro.body.push_back(processedLine);
            }

            continue;
        }

        if (opcode == "MCDEFN") {
            isMacro = true;
            macroLevel = 0;

            currentMacro = Macro();
            currentMacro.name = label;
            if (!currentMacro.name.empty() && currentMacro.name.back() == ':') {
                currentMacro.name.pop_back();
            }

            for (const std::string& token : tokens) {
                if (token[0] == '&') {
                    currentMacro.parameters.push_back(token);
                }
            }

            continue;
        }

        // MODO NORMAL (Fora da definição):
        intermediateCode.push_back(program[i]);
    }

    // === PASSO 2: Expansão Iterativa ===
    std::vector<std::string> expandedProgram = intermediateCode;
    bool expandedAny = true;

    while (expandedAny) {
        expandedAny = false;
        std::vector<std::string> passOutput;

        for (size_t i = 0; i < expandedProgram.size(); i++) {
            std::vector<std::string> tokens = utils::tokenizeInstruction(expandedProgram[i]);

            if (tokens.empty()) {
                passOutput.push_back(expandedProgram[i]);
                continue;
            }

            std::string label = utils::getLabel(tokens, macroNames);
            std::string opcode = utils::getOpcode(tokens, macroNames);

            // Se for uma chamada de macro cadastrada
            if (!opcode.empty() && macroNames.find(opcode) != macroNames.end()) {
                expandedAny = true;
                serialCounter++;

                Macro expandMacro = MNT[opcode];

                // Coleta os argumentos reais da chamada
                size_t opcodeIdx = 0;
                for (size_t j = 0; j < tokens.size(); j++) {
                    if (tokens[j] == opcode) {
                        opcodeIdx = j;
                        break;
                    }
                }

                std::vector<std::string> actualArguments;
                for (size_t j = opcodeIdx + 1; j < tokens.size(); j++) {
                    actualArguments.push_back(tokens[j]);
                }

                // Expande as linhas
                for (size_t j = 0; j < expandMacro.body.size(); j++) {
                    std::string expandedLine = expandMacro.body[j];

                    // Substitui o rótulo serial .SER pelo contador
                    replaceAll(expandedLine, ".SER", std::to_string(serialCounter));

                    // Substitui marcadores posicionais por argumentos reais
                    for (size_t k = 0; k < actualArguments.size(); k++) {
                        replaceAll(expandedLine, "?" + std::to_string(k), actualArguments[k]);
                    }

                    // Se for a primeira linha da macro expandida e a chamada original tinha label, preserva o label
                    if (j == 0 && !label.empty()) {
                        expandedLine = label + " " + expandedLine;
                    }
                    passOutput.push_back(expandedLine);
                }
                continue;
            }

            passOutput.push_back(expandedProgram[i]);
        }
        expandedProgram = passOutput;
    }

    saveFile(expandedProgram);
}

void MacroProcessor::replaceAll(std::string& str, const std::string& target, const std::string& replacement) {
    if (target.empty()) return;
    
    size_t startPos = 0;
    while((startPos = str.find(target, startPos)) != std::string::npos)
    {
        str.replace(startPos, target.length(), replacement);
        startPos += replacement.length();
    }
}

#include "macro.h"
#include <fstream>
#include <iostream>
#include <deque>

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
    std::vector<std::string> finalOutput;
    
    // Fila contendo as linhas a serem processadas
    std::deque<std::string> inputQueue(program.begin(), program.end());
    
    bool isMacro = false;
    int macroLevel = 0;
    Macro currentMacro;

    // Loop de única passagem que processa e expande dinamicamente
    while (!inputQueue.empty()) {
        std::string currentLine = inputQueue.front();
        inputQueue.pop_front();

        std::vector<std::string> tokens = utils::tokenizeInstruction(currentLine);
        if (tokens.empty()) {
            if (!isMacro) {
                finalOutput.push_back(currentLine);
            }
            continue;
        }

        std::string label = utils::getLabel(tokens, macroNames);
        std::string opcode = utils::getOpcode(tokens, macroNames);

        // MODO DEFINIÇÃO
        if (isMacro) {
            if (opcode == "MCDEFN") {
                macroLevel++;
                std::string processedLine = currentLine;
                for (size_t j = 0; j < currentMacro.parameters.size(); j++) {
                    replaceAll(processedLine, currentMacro.parameters[j], "?" + std::to_string(j));
                }
                currentMacro.body.push_back(processedLine);
            }
            else if (opcode == "MCEND") {
                if (macroLevel > 0) {
                    macroLevel--;
                    std::string processedLine = currentLine;
                    for (size_t j = 0; j < currentMacro.parameters.size(); j++) {
                        replaceAll(processedLine, currentMacro.parameters[j], "?" + std::to_string(j));
                    }
                    currentMacro.body.push_back(processedLine);
                } else {
                    isMacro = false;
                    MNT[currentMacro.name] = currentMacro;
                    macroNames.insert(currentMacro.name);
                }
            } else {
                std::string processedLine = currentLine;
                for (size_t j = 0; j < currentMacro.parameters.size(); j++) {
                    replaceAll(processedLine, currentMacro.parameters[j], "?" + std::to_string(j));
                }
                currentMacro.body.push_back(processedLine);
            }
            continue;
        }

        // DETECTA NOVA MACRO
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

        // MODO EXPANSÃO: Chamada de Macro detectada
        if (!opcode.empty() && macroNames.find(opcode) != macroNames.end()) {
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

            // Expande o corpo da macro inserindo na frente da fila (ordem reversa)
            for (int j = (int)expandMacro.body.size() - 1; j >= 0; j--) {
                std::string expandedLine = expandMacro.body[j];

                // Substitui rótulos posicionais e seriais
                replaceAll(expandedLine, ".SER", std::to_string(serialCounter));
                for (size_t k = 0; k < actualArguments.size(); k++) {
                    replaceAll(expandedLine, "?" + std::to_string(k), actualArguments[k]);
                }

                // Preserva o label na primeira linha expandida
                if (j == 0 && !label.empty()) {
                    expandedLine = label + " " + expandedLine;
                }

                inputQueue.push_front(expandedLine); // Devolve para a fila
            }
            continue;
        }

        // MODO NORMAL (Copia para a saída final)
        finalOutput.push_back(currentLine);
    }

    saveFile(finalOutput);
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

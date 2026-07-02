#include "assembler.h"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include "../utils.h"


Assembler::Assembler(const std::string& fileName) : fileName(fileName) {
    loadFile();
}


void Assembler::loadFile() {
    std::ifstream file(fileName);
    std::string line;

    while (std::getline(file, line)) {
        program.push_back(line);
    }
}


void Assembler::assemble() {
    firstPass();
    secondPass();
}


void Assembler::firstPass() {
    locationCounter = 0;
    symbolTable = {};
    std::string label, opcode, operand;
    unsigned int length, value;

    for (std::string instructionString : program) {
        std::vector<std::string> instruction = utils::tokenizeInstruction(instructionString);
        if (instruction.empty()) continue;

        label   = utils::getLabel(instruction);
        opcode  = utils::getOpcode(instruction);
        operand = utils::getOperand(instruction);

        if (opcode.empty()) {
            if (!label.empty()) {
                insertInTable(label, locationCounter);
            }
            continue;
        }

        if (utils::isPseudoInstruction(opcode)) {
            if (opcode == "ORG") {
                locationCounter = getOperandValue(operand);
                length = 0;
            } else if (opcode == "END") {
                return;
            } else if (opcode == "EQU") {
                value  = getOperandValue(operand);
                length = 0;
            } else { // "DS" ou "DC"
                value  = locationCounter;
                length = utils::getInstructionSize(instruction);
            }

            if (!label.empty()) {
                insertInTable(label, value);
            }
            locationCounter += length;

        } else if (utils::isMachineInstruction(opcode)) {
            if (!label.empty()) {
                insertInTable(label, locationCounter);
            }
            length = utils::getInstructionSize(instruction);
            locationCounter += length;

        } else {
            throw std::runtime_error("Operador Inválido: " + opcode);
        }
    }
}


void Assembler::secondPass() {
    locationCounter = 0;
    std::string opcode, operand;
    unsigned int length;
    startAddress = -1;

    for (std::string instructionString : program) {
        std::vector<std::string> instruction = utils::tokenizeInstruction(instructionString);
        if (instruction.empty()) continue;

        opcode  = utils::getOpcode(instruction);
        operand = utils::getOperand(instruction);

        if (opcode == "EQU") {
            // Ignora — já resolvido na primeira fase
        } else if (opcode == "ORG") {
            locationCounter = getOperandValue(operand);
        } else if (opcode == "END") {
            startAddress = getOperandValue(operand);
            return;
        } else {
            length = utils::getInstructionSize(instruction);
            generateMachineCode(instruction);
            locationCounter += length;
        }
    }
}


void Assembler::generateMachineCode(const std::vector<std::string>& instruction) {
    std::string opcode = utils::getOpcode(instruction);
    if (opcode.empty()) return;

    // Pseudo-instruções de dados
    if (opcode == "DS") {
        unsigned int n = getOperandValue(utils::getOperand(instruction, 0));
        for (unsigned int i = 0; i < n; ++i) {
            machineCode.push_back(0x00);
        }
        return;
    }

    if (opcode == "DC") {
        machineCode.push_back(static_cast<uint8_t>(getOperandValue(utils::getOperand(instruction))));
        return;
    }

    // Instruções sem operandos
    if (opcode == "NOP")  { machineCode.push_back(0x00); return; }
    if (opcode == "HALT") { machineCode.push_back(0x76); return; }
    if (opcode == "RET")  { machineCode.push_back(0xC9); return; }

    // TODO: implementar codificação de bytes para LD, JP, JR, ADD, SUB, etc.
}


void Assembler::insertInTable(const std::string& label, unsigned int value) {
    symbolTable.insert({label, value});
}


int Assembler::getOperandValue(const std::string& operand) {
    if (operand.empty()) return -1;

    if (operand[0] == '@') {        // Decimal
        return std::stoi(operand.substr(1), 0, 10);
    } else if (operand[0] == '$') { // Hexadecimal
        return std::stoi(operand.substr(1), 0, 16);
    }

    return findInTable(operand);
}


int Assembler::findInTable(const std::string& label) {
    if (symbolTable.find(label) == symbolTable.end()) return -1;
    return symbolTable.at(label);
}


void Assembler::debug() {
    std::cout << "Código" << std::endl;
    for (std::string line : program) {
        std::cout << line << std::endl;
    }

    std::cout << "Tabela de símbolos:" << std::endl;
    for (auto it = symbolTable.begin(); it != symbolTable.end(); it++) {
        std::cout << it->first << "\t\t" << it->second << std::endl;
    }

    std::cout << "Código de Máquina" << std::endl;
    for (int byte : machineCode) {
        std::cout << byte << " ";
    }
}
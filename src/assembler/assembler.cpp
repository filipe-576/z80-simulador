#include "assembler.h"
#include <cstddef>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include "../utils.h"


Assembler::Assembler(const std::string& fileName): fileName(fileName){
    loadFile();
}


void Assembler::loadFile(){
    std::ifstream file(fileName);
    std::string line;

    while( std::getline(file, line )){
        program.push_back(line);
    }
}


void Assembler::assemble(){
    firstPass();
    // secondPass();
}


void Assembler::firstPass(){
    locationCounter = 0;
    symbolTable = {};
    std::string label, opcode, operand;
    unsigned int length, value;
    
    for( std::string instructionString: program ){
        std::vector<std::string> instruction = utils::tokenizeInstruction(instructionString);
        if( instruction.empty() ) continue;

        label = utils::getLabel(instruction);
        opcode = utils::getOpcode(instruction);
        operand = utils::getOperand(instruction);

        if( opcode.empty() ){
            if( !label.empty() ){
                insertInTable(label, locationCounter);
            }
            continue;
        }
        if( utils::isPseudoInstruction(opcode) ){
            if( opcode == "ORG" ){
                locationCounter = getOperandValue(operand);
                length = 0;
            } else if( opcode == "END" ){
                return;
            } else if( opcode == "EQU" ){
                value = getOperandValue(operand);
                length = 0;
            } else { // "DS" ou "DC"
                value = locationCounter;
                length = getInstructionSize(instruction);
            }

            if( !label.empty() ){
                insertInTable(label, value);
            }
            locationCounter += length;
        }
        else if( utils::isMachineInstruction(opcode) ){
            if( !label.empty() ){
                insertInTable(label, locationCounter);
            }
            length = getInstructionSize(instruction);
            locationCounter += length;
        } else{
            throw std::runtime_error("Operador Inválido: " + opcode);
        }

    }
}


unsigned int Assembler::getInstructionSize(const std::vector<std::string>& instruction){
    std::string opcode = utils::getOpcode(instruction);
    if (opcode.empty()) return 0;

    // Pseudo-instruções
    if (opcode == "ORG" || opcode == "EQU" || opcode == "END") {
        return 0;
    }
    
    if (opcode == "DS") {
        return getOperandValue(utils::getOperand(instruction, 0));
    }
    
    if (opcode == "DC") {
        size_t opcodePos = 0;
        bool found = false;
        for (size_t i = 0; i < instruction.size(); ++i) {
            if (instruction[i] == opcode) {
                opcodePos = i;
                found = true;
                break;
            }
        }

        if (found) {
            return instruction.size() - opcodePos - 1;
                } else {
            return 0;
            }      
        }

    // Instruções de máquina
    if (opcode == "NOP" || opcode == "HALT" || opcode == "RET") {
        return 1;
    }

    std::string op1 = utils::getOperand(instruction, 0);
    std::string op2 = utils::getOperand(instruction, 1);

    auto isReg8 = [](const std::string& op) {
        return op == "A" || op == "B" || op == "C" || op == "D" || op == "E" || op == "H" || op == "L";
    };

    auto isReg16 = [](const std::string& op) {
        return op == "BC" || op == "DE" || op == "HL" || op == "SP" || op == "IX" || op == "IY" || op == "AF";
    };

    auto isInd = [](const std::string& op) {
        return !op.empty() && op.front() == '(' && op.back() == ')';
    };

    auto isIdx = [](const std::string& op) {
        return !op.empty() && op.front() == '(' && op.back() == ')' && 
               (op.find("IX") != std::string::npos || op.find("IY") != std::string::npos);
    };

    if (opcode == "LD") {
        if (op1 == "I" || op1 == "R" || op2 == "I" || op2 == "R") return 2;

        if (op1 == "SP") {
            if (op2 == "HL") return 1;
            if (op2 == "IX" || op2 == "IY") return 2;
            return 3; // LD SP, nn
        }
        if (op1 == "IX" || op1 == "IY") {
            if (isInd(op2)) return 4; // LD IX, (nn)
            return 4; // LD IX, nn
        }
        if (isInd(op1)) {
            if (isIdx(op1)) {
                if (isReg8(op2)) return 3; // LD (IX+d), r
                return 4; // LD (IX+d), n
            }
            if (op1 == "(HL)") {
                if (isReg8(op2)) return 1; // LD (HL), r
                return 2; // LD (HL), n
            }
            if (op1 == "(BC)" || op1 == "(DE)") return 1; // LD (BC/DE), A
            // Endereçamento estendido (nn)
            if (op2 == "HL" || op2 == "A") return 3;
            return 4; // LD (nn), dd/IX/IY
        }
        if (isReg16(op1)) {
            if (isInd(op2)) {
                if (op1 == "HL") return 3; // LD HL, (nn)
                return 4; // LD dd, (nn)
            }
            return 3; // LD dd, nn
        }
        if (isReg8(op1)) {
            if (isReg8(op2)) return 1; // LD r, r'
            if (isInd(op2)) {
                if (isIdx(op2)) return 3; // LD r, (IX+d)
                if (op2 == "(HL)") return 1; // LD r, (HL)
                if (op2 == "(BC)" || op2 == "(DE)") return 1; // LD A, (BC/DE)
                return 3; // LD A, (nn)
            }
            return 2; // LD r, n
        }
    }

    if (opcode == "JR") {
        return 2;
    }

    if (opcode == "JP") {
        if (op1 == "(HL)") return 1;
        if (op1 == "(IX)" || op1 == "(IY)") return 2;
        return 3;
    }

    if (opcode == "CALL") {
        return 3;
    }

    if (opcode == "PUSH" || opcode == "POP") {
        if (op1 == "IX" || op1 == "IY") {
            return 2;
        } else {
            return 1;
        }
    }
    if (opcode == "INC" || opcode == "DEC") {
        if (isReg16(op1)) {
            if (op1 == "IX" || op1 == "IY") {
                return 2;
            } else {
                return 1;
            }
        }
        if (isInd(op1)) {
            if (isIdx(op1)) {
                return 3;
            } else {
                return 1;
            }
        }
        return 1;
    }

    if (opcode == "ADD" || opcode == "SUB" || opcode == "AND" || opcode == "OR" || opcode == "XOR" || opcode == "CP") {
        if (isReg16(op1)) {
            if (op1 == "HL") return 1;
            if (op1 == "IX" || op1 == "IY") return 2;
        }
        std::string source = op2.empty() ? op1 : op2;
        if (isReg8(source)) return 1;
        if (isInd(source)) {
            if (isIdx(source)) {
                return 3;
            } else {
                return 1;
            }
        }
        return 2;
    }

    return 1;
}


void Assembler::insertInTable(const std::string& label, unsigned int value){
    symbolTable.insert({label, value});
}


int Assembler::getOperandValue(const std::string& operand){
    if( operand.empty() ) return -1;
    
    if( operand[0] == '@' ){        // Dec
        return std::stoi(operand.substr(1), 0, 10);
    } else if( operand[0] == '$' ){ // Hexa
        return std::stoi(operand.substr(1), 0, 16);
    }
    return findInTable(operand);
}


int Assembler::findInTable(const std::string& label){
    if( symbolTable.find(label) == symbolTable.end() ) return -1;
    return symbolTable.at(label);
}

void Assembler::debug(){
    std::cout << "Código" << std::endl;
    for( std::string line: program ){
        std::cout << line << std::endl;
    }

    std::cout << "Tabela de símbolos:" << std::endl;
    for( auto it = symbolTable.begin(); it != symbolTable.end(); it++ ){
        std::cout << it->first << "\t\t" << it->second << std::endl;
    }
}
#include "assembler.h"
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>


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
        std::vector<std::string> instruction = tokenizeInstruction(instructionString);
        if( instruction.empty() ) continue;

        label = getLabel(instruction);
        opcode = getOpcode(instruction);
        operand = getOperand(instruction);

        if( opcode.empty() ){
            if( !label.empty() ){
                insertInTable(label, locationCounter);
            }
            continue;
        }
        if( isPseudoInstruction(opcode) ){
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
        else if( isMachineInstruction(opcode) ){
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
    // Não feito
    return 1;
}



void Assembler::insertInTable(const std::string& label, unsigned int value){
    symbolTable.insert({label, value});
}


unsigned int Assembler::getOperandValue(const std::string& operand){
    if( !operand.empty() && operand[0] == '@' ){
        return std::stoi(operand.substr(1));
    }
    return findInTable(operand);
}


int Assembler::findInTable(const std::string& label){
    if( symbolTable.find(label) == symbolTable.end() ) return -1;
    return symbolTable.at(label);
}


bool Assembler::isPseudoInstruction(const std::string& label){
    if( PSEUDO_INSTRUCTIONS.find(label) != PSEUDO_INSTRUCTIONS.end() ){
        return true;
    }
    return false;
}


bool Assembler::isMachineInstruction(const std::string& label){
    if( MACHINE_INSTRUCTIONS.find(label) != MACHINE_INSTRUCTIONS.end() ){
        return true;
    }
    return false;
}


std::string Assembler::getOperand(const std::vector<std::string>& instruction, const unsigned short index){
    if( instruction.size() == 0 ) return "";
    if( index > 1 ) throw std::invalid_argument("Índice inválido");

    size_t i;
    for( i = 0; i < instruction.size(); i++ ){
        if( isPseudoInstruction(instruction[i]) || isMachineInstruction(instruction[i])){
            break;
        }
    }

    if( instruction[i] == "NOP" || instruction[i] == "HALT" || instruction[i] == "RET" ){
        return "";
    }
    if( i + 1 + index >= instruction.size() ){
        return "";
    }
    
    return instruction[i + 1 + index];
}


std::string Assembler::getLabel(const std::vector<std::string>& instruction){

    for( size_t i = 0; i < instruction.size(); i++ ){
        if( isPseudoInstruction(instruction[i]) || isMachineInstruction(instruction[i])){
            if( i == 0 ) return "";
            return instruction[i-1];
        }
    }
    if (!instruction.empty()) return instruction[0];
    return ""; 
}


std::string Assembler::getOpcode(const std::vector<std::string>& instruction){
    for( size_t i = 0; i < instruction.size(); i++ ){

        if( isPseudoInstruction(instruction[i]) || isMachineInstruction(instruction[i])){
            return instruction[i];
        }
    }
    return "";
}


std::vector<std::string> Assembler::tokenizeInstruction(const std::string& instruction){
    std::vector<std::string> tokenizedInstruction;
    std::string rawInstruction(instruction);
    std::string token;

    for( int i = 0; i < rawInstruction.size(); ++i ){
        char c = rawInstruction[i];
        
        if( c == ',' ) rawInstruction[i] = ' ';
        if( c == ';' ){
            rawInstruction.erase(i);
            break;
        }
    }

    std::stringstream ss(rawInstruction);
    while( ss >> token ){
        tokenizedInstruction.push_back(token);
    }

    return tokenizedInstruction;
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
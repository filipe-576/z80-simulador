#include "assembler.h"
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

Assembler::Assembler(std::string fileName): fileName(fileName){
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
    
    for( std::string instruction: program ){
        std::vector<std::string> tokenizedInstruction = tokenizeInstruction(instruction);
        if( tokenizedInstruction.empty() ) continue;

        label = getLabel(tokenizedInstruction);

        opcode = getOpcode(tokenizedInstruction);

        if( !opcode.empty() && opcode != "NOP" && opcode != "HALT" ){
            operand = getOperand(tokenizedInstruction);
        } else{
            operand = "";
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
            } else {
                value = locationCounter;
                length = getInstructionSize(tokenizedInstruction);
            }

            if( !label.empty() ){
                insertTable(label, value);
            }
            locationCounter += length;
        }
        else if( isMachineInstruction(opcode) ){
            length = getInstructionSize(tokenizedInstruction);
            if( !label.empty() ){
                insertTable(label, locationCounter);
            }
            locationCounter += length;
        } else{
            throw std::string("Operador Inválido");
        }

    }
}


unsigned int Assembler::getInstructionSize(std::vector<std::string> instruction){
    return 1;
}


void Assembler::insertTable(std::string label, unsigned int value){
    symbolTable.insert({label, value});
}


unsigned int Assembler::getOperandValue(std::string operand){
    if( operand[0] == '@' ){
        operand.erase(0, 1);
        return std::stoi(operand);
    }
    return findTable(operand);

}


int Assembler::findTable(std::string label){
    if( symbolTable.find(label) == symbolTable.end() ) return -1;
    return symbolTable.at(label);
}

bool Assembler::isPseudoInstruction(std::string label){
    if( PSEUDO_INSTRUCTIONS.find(label) != PSEUDO_INSTRUCTIONS.end() ){
        return true;
    }
    return false;
}


bool Assembler::isMachineInstruction(std::string label){
    if( MACHINE_INSTRUCTIONS.find(label) != MACHINE_INSTRUCTIONS.end() ){
        return true;
    }
    return false;
}


std::string Assembler::getOperand(std::vector<std::string> instruction, unsigned short index){
    if( instruction.size() == 0 ) return "";
    if( index > 1 ) throw std::invalid_argument("Índice inválido");

    size_t i;
    for( i = 0; i < instruction.size(); i++ ){
        if( isPseudoInstruction(instruction[i]) || isMachineInstruction(instruction[i])){
            break;
        }
    }

    return instruction[i + 1 + index];
}


std::string Assembler::getLabel(std::vector<std::string> instruction){

    for( size_t i = 0; i < instruction.size(); i++ ){

        if( !isPseudoInstruction(instruction[i]) && !isMachineInstruction(instruction[i])){
            continue;
        }

        if( i == 0 ) return "";

        return instruction[i-1];
    }
    return "";
}


std::string Assembler::getOpcode(std::vector<std::string> instruction){
    for( size_t i = 0; i < instruction.size(); i++ ){

        if( isPseudoInstruction(instruction[i]) || isMachineInstruction(instruction[i])){
            return instruction[i];
        }
    }
    return "";
}


std::vector<std::string> Assembler::tokenizeInstruction(std::string instruction){
    std::vector<std::string> elements;

    std::stringstream ss(instruction);
    std::string token;
    short int commaIndex;

    while( ss >> token ){
        if(token[0] == ';'){
            break;
        }

        commaIndex = token.find(',');
        if ( commaIndex != -1 ){
            token.erase(commaIndex);
        }
        elements.push_back(token);
    }

    return elements;
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
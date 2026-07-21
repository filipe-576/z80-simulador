#include "utils.h"
#include <stdexcept>
#include <sstream>


bool utils::isPseudoInstruction(const std::string& label){
    return PSEUDO_INSTRUCTIONS.count(label) > 0;
}


bool utils::isMachineInstruction(const std::string& label){
    return MACHINE_INSTRUCTIONS.count(label) > 0;
}


std::string utils::getOperand(const std::vector<std::string>& instruction, const unsigned short index){
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


std::string utils::getLabel(const std::vector<std::string>& instruction, const std::set<std::string>& additionalOpcodes){
    // Remove ':' final que pode aparecer em labels como "LOOP:"
    auto stripColon = [](std::string s) -> std::string {
        if (!s.empty() && s.back() == ':') s.pop_back();
        return s;
    };
    for( size_t i = 0; i < instruction.size(); i++ ){
        if( isPseudoInstruction(instruction[i]) || isMachineInstruction(instruction[i]) ||
            additionalOpcodes.find(instruction[i]) != additionalOpcodes.end() ){ // Para procurar macros definidas pelo usuário na MNT
            if( i == 0 ) return "";
            return stripColon(instruction[i-1]);
        }
    }
    if (!instruction.empty()) return stripColon(instruction[0]);
    return "";
}


std::string utils::getOpcode(const std::vector<std::string>& instruction, const std::set<std::string>& additionalOpcodes){
    for( size_t i = 0; i < instruction.size(); i++ ){

        if( isPseudoInstruction(instruction[i]) || isMachineInstruction(instruction[i]) ||
        additionalOpcodes.find(instruction[i]) != additionalOpcodes.end() ){ // Para procurar macros definidas pelo usuário na MNT
            return instruction[i];
        }
    }
    return "";
}


std::vector<std::string> utils::tokenizeInstruction(const std::string& instruction){
    std::vector<std::string> tokenizedInstruction;
    std::string rawInstruction(instruction);
    std::string token;

    for( size_t i = 0; i < rawInstruction.size(); ++i ){
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
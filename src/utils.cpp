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


std::string utils::getLabel(const std::vector<std::string>& instruction){
    // Remove ':' final que pode aparecer em labels como "LOOP:"
    auto stripColon = [](std::string s) -> std::string {
        if (!s.empty() && s.back() == ':') s.pop_back();
        return s;
    };
    for( size_t i = 0; i < instruction.size(); i++ ){
        if( isPseudoInstruction(instruction[i]) || isMachineInstruction(instruction[i])){
            if( i == 0 ) return "";
            return stripColon(instruction[i-1]);
        }
    }
    if (!instruction.empty()) return stripColon(instruction[0]);
    return ""; 
}


std::string utils::getOpcode(const std::vector<std::string>& instruction){
    for( size_t i = 0; i < instruction.size(); i++ ){

        if( isPseudoInstruction(instruction[i]) || isMachineInstruction(instruction[i])){
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


// Analisa o valor numérico de um operando literal (@decimal ou $hex)
static unsigned int parseOperandValue(const std::string& operand) {
    if (operand.empty()) return 0;
    try {
        if (operand[0] == '@') return std::stoi(operand.substr(1), nullptr, 10);
        if (operand[0] == '$') return std::stoi(operand.substr(1), nullptr, 16);
        return std::stoi(operand);
    } catch (...) {
        return 0;
    }
}


unsigned int utils::getInstructionSize(const std::vector<std::string>& instruction){
    std::string opcode = getOpcode(instruction);
    if (opcode.empty()) return 0;

    // Pseudo-instruções que não ocupam memória
    if (opcode == "ORG" || opcode == "EQU" || opcode == "END") return 0;

    if (opcode == "DS") {
        return parseOperandValue(getOperand(instruction, 0));
    }
    if (opcode == "DC") {
        for (size_t i = 0; i < instruction.size(); ++i) {
            if (instruction[i] == "DC") return instruction.size() - i - 1;
        }
        return 0;
    }

    // Instruções sem operandos
    if (opcode == "NOP" || opcode == "HALT" || opcode == "RET") return 1;

    std::string op1 = getOperand(instruction, 0);
    std::string op2 = getOperand(instruction, 1);

    auto isReg8 = [](const std::string& op) {
        return op=="A"||op=="B"||op=="C"||op=="D"||op=="E"||op=="H"||op=="L";
    };
    auto isReg16 = [](const std::string& op) {
        return op=="BC"||op=="DE"||op=="HL"||op=="SP"||op=="IX"||op=="IY"||op=="AF";
    };
    auto isInd = [](const std::string& op) {
        return !op.empty() && op.front()=='(' && op.back()==')';
    };
    auto isIdx = [](const std::string& op) {
        return !op.empty() && op.front()=='(' && op.back()==')' &&
               (op.find("IX")!=std::string::npos || op.find("IY")!=std::string::npos);
    };

    if (opcode == "LD") {
        if (op1=="I"||op1=="R"||op2=="I"||op2=="R") return 2;
        if (op1=="SP") { return (op2=="HL") ? 1 : (op2=="IX"||op2=="IY") ? 2 : 3; }
        if (op1=="IX"||op1=="IY") return 4;
        if (isInd(op1)) {
            if (isIdx(op1)) return isReg8(op2) ? 3 : 4;
            if (op1=="(HL)") return isReg8(op2) ? 1 : 2;
            if (op1=="(BC)"||op1=="(DE)") return 1;
            return (op2=="HL"||op2=="A") ? 3 : 4;
        }
        if (isReg16(op1)) return isInd(op2) ? (op1=="HL" ? 3 : 4) : 3;
        if (isReg8(op1)) {
            if (isReg8(op2)) return 1;
            if (isInd(op2)) {
                if (isIdx(op2)) return 3;
                if (op2=="(HL)"||op2=="(BC)"||op2=="(DE)") return 1;
                return 3;
            }
            return 2;
        }
    }
    if (opcode=="JR") return 2;
    if (opcode=="JP") {
        if (op1=="(HL)") return 1;
        if (op1=="(IX)"||op1=="(IY)") return 2;
        return 3;
    }
    if (opcode=="CALL") return 3;
    if (opcode=="PUSH"||opcode=="POP") return (op1=="IX"||op1=="IY") ? 2 : 1;
    if (opcode=="INC"||opcode=="DEC") {
        if (isReg16(op1)) return (op1=="IX"||op1=="IY") ? 2 : 1;
        if (isInd(op1)) return isIdx(op1) ? 3 : 1;
        return 1;
    }
    if (opcode=="ADD"||opcode=="SUB"||opcode=="AND"||opcode=="OR"||opcode=="XOR"||opcode=="CP") {
        if (isReg16(op1)) {
            if (op1=="HL") return 1;
            if (op1=="IX"||op1=="IY") return 2;
        }
        std::string src = op2.empty() ? op1 : op2;
        if (isReg8(src)) return 1;
        if (isInd(src)) return isIdx(src) ? 3 : 1;
        return 2;
    }
    return 1;
}
#include "assembler.h"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include "../utils.h"
#include "../json.hpp"


Assembler::Assembler(const std::string& fileName, const std::string& outputName) : fileName(fileName), outputName(outputName) {
    loadFile();
}

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
    resolvePendingExports();
    secondPass();
    generateObjectFile();
}


void Assembler::firstPass() {
    locationCounter = 0;
    symbolTable     = {};
    usageTable        = {};
    defTable        = {};
    pendingExports  = {};
    std::string label, opcode, operand;
    unsigned int length;
    std::optional<uint16_t> value;
    
    for( std::string instructionString: program ){
        std::vector<std::string> instruction = utils::tokenizeInstruction(instructionString);
        if (instruction.empty()) continue;

        label   = utils::getLabel(instruction);
        opcode  = utils::getOpcode(instruction);
        operand = utils::getOperand(instruction);
        SymbolType type = SymbolType::RELATIVE;
        SymbolScope scope = SymbolScope::LOCAL;

        // Linha com rótulo sem operador
        if (opcode.empty()) {
            if (!label.empty()) {
                symbolTable.insert({label, {locationCounter, type, scope}});
            }
            continue;
        }

        // Pseudo Instruções
        if( utils::isPseudoInstruction(opcode) ){
            if( opcode == "END" ){
                // Altera o escopo dos símbolos que foram utilizados com INTDEF para global na tabela de símbolos
                for( auto& symbol: pendingExports ){
                    symbolTable.at(symbol).scope = SymbolScope::GLOBAL;
                }
                return;
            }
            else if (opcode == "EQU") {
                value  = getOperandValue(operand);
                type = SymbolType::ABSOLUTE;
                length = 0;
            }
            else if (opcode == "INTUSE") {
                scope = SymbolScope::EXTERN;
                type = SymbolType::NOT_RESOLVED;
                value = std::nullopt;
                length = 0;
                
            }
            else if (opcode == "INTDEF") {
                pendingExports.push_back(operand);
                length = 0;
                
            }
            else { // "DS" ou "DC"
                value  = locationCounter;
                length = utils::getInstructionSize(instruction);
            }

            if (!label.empty()) {
                symbolTable.insert({label, {value, type, scope}});
            }
            locationCounter += length;

        // Instruções de Máquina
        } else if (utils::isMachineInstruction(opcode)) {
            if (!label.empty()) {
                symbolTable.insert({label, {locationCounter, type, scope}});
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
    unsigned int length, instructionOffset, operandTotalOffset;
    entryPoint = 0;
    
    for( std::string instructionString: program ){
        std::vector<std::string> instruction = utils::tokenizeInstruction(instructionString);
        if (instruction.empty()) continue;

        opcode  = utils::getOpcode(instruction);
        operand = utils::getOperand(instruction);

        if( opcode == "EQU" || opcode == "INTUSE" || opcode == "INTDEF"){} // Ignora
        else if( opcode == "END" ){
            if( operand.empty() ) return;
            entryPoint = getOperandValue(operand);
            return;
        } else {
            length = utils::getInstructionSize(instruction);
            instructionOffset = machineCode.size();

            GenerateCodeResult result = generateMachineCode(instruction);
            if( result.operandOffset != -1 ){
                operandTotalOffset = instructionOffset + result.operandOffset;
                Symbol symbol = findInTable(result.symbolOperand);

                if( symbol.type == SymbolType::ABSOLUTE ); // Literais

                else if( symbol.scope == SymbolScope::EXTERN ){ // Símbolos externos
                    insertInUsageTable(result.symbolOperand, operandTotalOffset);
                } else{ // Símbolos locais
                    relocationMap.push_back(operandTotalOffset);
                }

            }
            machineCode.insert(machineCode.end(), result.bytes.begin(), result.bytes.end());
            locationCounter += length;
        }
    }
}


void Assembler::resolvePendingExports(){
    unsigned int value;
    for( auto symbol: pendingExports ){
        value = symbolTable.at(symbol).value.value();
        defTable.insert({symbol, value});
    }
}


void Assembler::generateObjectFile(){
    using json = nlohmann::json;

    json json_data;

    json_data["name"] = fileName;
    json_data["size"] = machineCode.size();
    json_data["definitionTable"] = defTable;
    json_data["useTable"] = usageTable;
    json_data["machineCode"] = machineCode;
    json_data["relocationMap"] = relocationMap;
    json_data["entryPoint"] = entryPoint;
    json_data["baseAddress"] = 0;

    std::ofstream file(outputName);

    if( file.is_open() ){
        file << json_data.dump(4);

        file.close();
    }
}


void Assembler::insertInUsageTable(const std::string& symbol, uint16_t address){
    if( usageTable.find(symbol) == usageTable.end() ){
        usageTable.insert({symbol, {address}});
    } else{
        usageTable.at(symbol).push_back(address);
    }
}

Assembler::GenerateCodeResult Assembler::generateMachineCode(const std::vector<std::string>& instruction) {
    std::string opcode = utils::getOpcode(instruction);
    std::vector<uint8_t> bytes;

    if ( opcode.empty() ) return {bytes, "", -1};


    auto reg8Code = [](const std::string& r) -> uint8_t {
        if ( r == "B" ) return 0;
        if ( r == "C" ) return 0b001;
        if ( r == "D" ) return 0b010;
        if ( r == "E" ) return 0b011;
        if ( r == "H" ) return 0b100;
        if ( r == "L" ) return 0b101;
        if ( r == "A" ) return 0b111;
        return 0b110; // (HL)
    };

    auto isReg8 = [](const std::string& op) {
        return op == "A" || op == "B" || op == "C" || op == "D" || op == "E" || op == "H" || op == "L";
    };

    auto isInd = [](const std::string& op) {
        return !op.empty() && op.front() == '(' && op.back() == ')';
    };

    auto isIdx = [](const std::string& op) {
        return !op.empty() && op.front() == '(' && op.back() == ')' &&
               (op.find("IX") != std::string::npos || op.find("IY") != std::string::npos);
    };

    // Retorna o deslocamento de (IX/Y+d)
    auto parseIdx = [&](const std::string& op) -> int8_t {
        size_t plusPos = op.find('+');
        size_t minusPos = op.find('-');
        if( plusPos != std::string::npos ) {
            std::string val = op.substr(plusPos + 1);
            val.pop_back(); // Remove )
            return static_cast<int8_t>(getOperandValue(val));
        } else if( minusPos != std::string::npos ) {
            std::string val = op.substr(minusPos + 1);
            val.pop_back();
            return static_cast<int8_t>(-getOperandValue(val));
        }
        return 0;
    };

    // Gera o código de máquina de 16 bits a partir de um valor imediato
    auto generateImediate16 = [&](uint16_t val) {
        bytes.push_back(val & 0b0000000011111111);
        bytes.push_back((val >> 8) & 0b0000000011111111);
    };

    // Gera o código de máquina de 16 bits a partir de um símbolo
    auto generateOperand16 = [&](const std::string& operand) {
        Symbol symbol = findInTable(operand);

        if( symbol.scope == SymbolScope::EXTERN ) {
            generateImediate16(0);
        } else {
            generateImediate16(static_cast<uint16_t>(getOperandValue(operand)));
        }
        
    };

    // Gera o código de máquina de 8 bits a partir de um operando
    auto generateOperand8 = [&](const std::string& operand) {
        Symbol symbol = findInTable(operand);
        if( symbol.scope == SymbolScope::EXTERN ) {
            bytes.push_back(0);
        } else {
            bytes.push_back(static_cast<uint8_t>(getOperandValue(operand)));
        }
    };

    // Retorna o valor de um operando com parênteses
    auto stripParens = [](const std::string& op) -> std::string {
        if( op.size() >= 2 && op.front() == '(' && op.back() == ')' ){
            return op.substr(1, op.size() - 2);
        }
        return op;
    };

    // Prefixo IX/IY
    auto ixiyPrefix = [](const std::string& op) -> uint8_t {
        if( op.find("IX") != std::string::npos ) return 0b11011101;
        return 0b11111101;
    };


    // Pseudo instruções

    if( opcode == "DS"){
        unsigned int count = getOperandValue(utils::getOperand(instruction, 0));
        for( unsigned int i = 0; i < count; ++i ){
            bytes.push_back(0);
        }
        return {bytes, "", -1};
    }

    if( opcode == "DC" ){
        size_t opcodePos = 0;
        for ( size_t i = 0; i < instruction.size(); ++i ){
            if( instruction[i] == opcode ){
                opcodePos = i;
                break;
            }
        }
        for ( size_t i = opcodePos + 1; i < instruction.size(); ++i ){
            generateOperand8(instruction[i]);
        }
        return {bytes, "", -1};
    }


    if( opcode == "NOP" ){
        bytes.push_back(0b00000000);
        return {bytes, "", -1};
    }


    if( opcode == "HALT" ){
        bytes.push_back(0b01110110);
        return {bytes, "", -1};
    }


    if( opcode == "RET" ){
        bytes.push_back(0b11001001);
        return {bytes, "", -1};
    }
    
    std::string op1 = utils::getOperand(instruction, 0);
    std::string op2 = utils::getOperand(instruction, 1);

    // LDs
    if( opcode == "LD" ){

        if( op1 == "IX" || op1 == "IY" ){
            uint8_t prefix = ixiyPrefix(op1);
            if( isInd(op2) ){
                // LD IX, (nn)
                bytes.push_back(prefix);
                bytes.push_back(0b00101010);
                generateOperand16(stripParens(op2));
            } else {
                // LD IX, nn
                bytes.push_back(prefix);
                bytes.push_back(0b00100001);
                generateOperand16(op2);
            }
            return {bytes, op2, 2}; // prefix + opcode = 2 bytes
        }

        // LD (algo), ... 
        if( isInd(op1) ){
            // LD (IX+d), r  |  LD (IX+d), n
            if( isIdx(op1) ){
                uint8_t prefix = ixiyPrefix(op1);
                int8_t d = parseIdx(op1);
                if( isReg8(op2) ){
                    // LD (IX+d), r
                    bytes.push_back(prefix);
                    bytes.push_back(0b01110000 | reg8Code(op2));
                    bytes.push_back(static_cast<uint8_t>(d));
                    return {bytes, "", -1};
                } else {
                    // LD (IX+d), n
                    bytes.push_back(prefix);
                    bytes.push_back(0b00110110);
                    bytes.push_back(static_cast<uint8_t>(d));
                    generateOperand8(op2);
                    return {bytes, op2, 3}; // prefix + opcode + d = 3
                }
            }

            // LD (HL), r  |  LD (HL), n
            if( op1 == "(HL)" ){
                if( isReg8(op2) ){
                    // LD (HL), r
                    bytes.push_back(0b01110000 | reg8Code(op2));
                    return {bytes, "", -1};
                } else {
                    // LD (HL), n
                    bytes.push_back(0b00110110);
                    generateOperand8(op2);
                    return {bytes, op2, 1};
                }
            }

            // LD (nn), A
            if( op2 == "A" ){
                bytes.push_back(0b00110010);
                generateOperand16(stripParens(op1));
                return {bytes, stripParens(op1), 1};
            }

            // LD (nn), IX | LD (nn), IY
            if( op2 == "IX" || op2 == "IY" ){
                bytes.push_back(ixiyPrefix(op2));
                bytes.push_back(0b00100010);
                generateOperand16(stripParens(op1));
                return {bytes, stripParens(op1), 2}; // prefix + opcode = 2
            }

            return {bytes, "", -1}; // LD (nn), tipo não reconhecido
        }

        // LD r, ...
        if( isReg8(op1) ){
            uint8_t dst = reg8Code(op1);

            // LD r, r'
            if( isReg8(op2) ){
                bytes.push_back(0b01000000 | (dst << 3) | reg8Code(op2)); // 01 ddd sss
                return {bytes, "", -1}; // LD r, r'
            }

            if( isInd(op2) ){
                // LD r, (IX+d) / LD r, (IY+d)
                if( isIdx(op2) ){
                    uint8_t prefix = ixiyPrefix(op2);
                    int8_t d = parseIdx(op2);
                    bytes.push_back(prefix);
                    bytes.push_back(0b01000110 | (dst << 3)); // 01 ddd 110
                    bytes.push_back(static_cast<uint8_t>(d));
                    return {bytes, "", -1}; // LD r, (IX+d)
                }

                // LD r, (HL)
                if( op2 == "(HL)" ){
                    bytes.push_back(0b01000110 | (dst << 3)); // 01 ddd 110
                    return {bytes, "", -1}; // LD r, (HL)
                }

                // LD A, (nn)
                bytes.push_back(0b00111010);
                generateOperand16(stripParens(op2));
                return {bytes, stripParens(op2), 1};
            }

            // LD r, n
            bytes.push_back(0b00000110 | (dst << 3)); // 00 ddd 110
            generateOperand8(op2);
            return {bytes, op2, 1};
        }
        return {bytes, "", -1}; // Erro
    }

    // JRs
    if( opcode == "JR" ){
        if( op2.empty() ){
            // JR e
            bytes.push_back(0b00011000);
        } else {
            // JR NZ, e
            // JR Z, e
            uint8_t c = op1 == "NZ" ? 0b100 : 0b101; // 0b100 -> JR NZ; 0b101 -> JR Z
            bytes.push_back(c << 3); // 00 ccc 000
        }
        int displacement = getOperandValue(op2);
        int8_t offset = static_cast<int8_t>(displacement - (locationCounter + 2));
        bytes.push_back(static_cast<uint8_t>(offset));
        return {bytes, op2, -1};
    }

    // JP nn
    if( opcode == "JP" ){
        if( op2.empty() ){ // JP nn
            bytes.push_back(0b11000011);
            generateOperand16(op1);
        } else{ // JP cc, nn
            uint8_t cc;
            if( op1 == "NZ" ) cc = 0;
            else if( op1 == "Z" ) cc = 0b001;
            else if( op1 == "NC" ) cc = 0b010;
            else if( op1 == "C" ) cc = 0b011;
            else if( op1 == "PO" ) cc = 0b100;
            else if( op1 == "PE" ) cc = 0b101;
            else if( op1 == "P" ) cc = 0b110;
            else cc = 0b111; // M
            bytes.push_back(0b11000010 | (cc << 3)); // 11ccc010
            generateOperand16(op2);
        }
        return {bytes, op2.empty() ? op1 : op2, 1};
    }

    // CALL nn
    if( opcode == "CALL" ){
        bytes.push_back(0b11001101);
        generateOperand16(op1);
        return {bytes, op1, 1};
    }

    // PUSH qq
    if( opcode == "PUSH" ){
        uint8_t qq;
        if( op1 == "BC" ) qq = 0; 
        else if( op1 == "DE" ) qq = 1;
        else if( op1 == "HL" ) qq = 2;
        else qq = 3; // AF
        bytes.push_back(0b11000101 | (qq << 4)); // 11qq0101
        return {bytes, "", -1}; // PUSH
    }

    // POP qq
    if( opcode == "POP" ){
        uint8_t qq;
        if( op1 == "BC" ) qq = 0;
        else if( op1 == "DE" ) qq = 1;
        else if( op1 == "HL" ) qq = 2;
        else qq = 3; // AF
        bytes.push_back(0b11000001 | (qq << 4)); // 11qq0001
        return {bytes, "", -1}; // POP
    }

    // INC r | INC (HL)
    if( opcode == "INC" ){
        uint8_t r = op1 == "(HL)" ? 6 : reg8Code(op1);
        bytes.push_back((r << 3) | 0b00000100); // 00rrr100
        return {bytes, "", -1}; // INC
    }

    // DEC r | DEC (HL)
    if( opcode == "DEC" ){
        uint8_t r = op1 == "(HL)" ? 6 : reg8Code(op1);
        bytes.push_back((r << 3) | 0b00000101); // 00rrr101
        return {bytes, "", -1}; // DEC
    }

    // Aritméticos e lógicos

    if( opcode == "ADD" || opcode == "SUB" || opcode == "AND" ||
        opcode == "OR"  || opcode == "XOR" || opcode == "CP" ) {

        uint8_t aluCode;
        if( opcode == "ADD" ) aluCode = 0;
        else if( opcode == "SUB" ) aluCode = 0b010;
        else if( opcode == "AND" ) aluCode = 0b100;
        else if( opcode == "XOR" ) aluCode = 0b101;
        else if( opcode == "OR" )  aluCode = 0b110;
        else aluCode = 0b111; // CP

        std::string source = op2.empty() ? op1 : op2;

        // alu r -> 10 yyy rrr
        // alu (HL) -> 10 yyy 110
        uint8_t r;
        if( source == "(HL)") r = 0b110;
        else r = reg8Code(source);

        bytes.push_back(0b10000000 | (aluCode << 3) | r);
        return {bytes, "", -1}; // ALU r/(HL)
    }
    return {bytes, "", -1};
}


int Assembler::getOperandValue(const std::string& operand) {
    if (operand.empty()) return -1;

    if (operand[0] == '@') {        // Decimal
        return std::stoi(operand.substr(1), 0, 10);
    } else if (operand[0] == '$') { // Hexadecimal
        return std::stoi(operand.substr(1), 0, 16);
    }

    Symbol symbol = findInTable(operand);

    // Se é externo, deve ser resolvido pelo ligador posteriormente
    if( symbol.scope == SymbolScope::EXTERN ) return -1;

    return symbol.value.value();
}


Assembler::Symbol Assembler::findInTable(const std::string& label) {
    if (symbolTable.find(label) == symbolTable.end()) return {};

    return symbolTable.at(label);
}


void Assembler::debug() {
    // std::cout << "Código" << std::endl;
    // for (std::string line : program) {
    //     std::cout << line << std::endl;
    // }

    std::cout << "\nTabela de símbolos:" << std::endl;
    std::cout << "Símbolo" << "\t\t" << "Valor" << "\t" << "Tipo" << "\t" << "Escopo" << std::endl;
    for (auto it = symbolTable.begin(); it != symbolTable.end(); it++) {
        std::string type;
        std::string scope;
        switch( it->second.type ){
            case SymbolType::ABSOLUTE:
                type = "ABS";
                break;
            case SymbolType::RELATIVE:
                type = "RELAT";
                break;
            case SymbolType::NOT_RESOLVED:
                type = "NOT_RES";
                break;
        }
        
        switch( it->second.scope ){
            case SymbolScope::LOCAL:
                scope = "LOC";
                break;
            case SymbolScope::GLOBAL:
                scope = "GLOB";
                break;
            case SymbolScope::EXTERN:
                scope = "EXT";
                break;
        }

        if( it->second.value.has_value() ){
            std::cout << it->first << "\t\t" << *it->second.value << "\t"
            << type << "\t" << scope << std::endl;
        }else{
            std::cout << it->first << "\t\t" << "null" << "\t"
            << type << "\t" << scope << std::endl;
        }
    }

    std::cout << "\n INTUSE:" << std::endl;
    for (const auto& sym : usageTable) {
        std::cout << sym.first << "\t\t";
        for( const auto& i: sym.second ){
            std::cout << i << " ";
        }
    }
    std::cout << std::endl;
    
    std::cout << "\n INTDEF:" << std::endl;
    for (const auto& sym : defTable) {
        std::cout << sym.first << "\t\t" << sym.second << std::endl;
    }

    std::cout << "\nTabela de Realocação:" << std::endl;
    for (auto it: relocationMap) {
        std::cout << it << " ";
    }
    std::cout << std::endl;

    // std::cout << "\nCódigo de Máquina" << std::endl;
    // for( int byte: machineCode ){
    //     std::cout << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << byte << " ";
    // }
}

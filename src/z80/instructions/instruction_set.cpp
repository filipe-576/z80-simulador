#include "instruction_set.h"
#include "../cpu/cpu.h"
#include <iostream>
#include <bitset>

bool getParity(uint8_t val);

uint8_t* registerFromByte(uint8_t regIndex, Registers* registradores);

uint8_t readRegOrHL(CPU& cpu, uint8_t index) {
    if (index == 0b110) {
        return cpu.mem.read(cpu.getRegisters().HL());
    }
    uint8_t* reg = registerFromByte(index, &cpu.getRegisters());
    return *reg;
}

void writeRegOrHL(CPU& cpu, uint8_t index, uint8_t value) {
    if (index == 0b110) {
        cpu.mem.write(cpu.getRegisters().HL(), value);
    } else {
        uint8_t* reg = registerFromByte(index, &cpu.getRegisters());
        *reg = value;
    }
}

void executeInstruction(CPU &cpu, uint8_t byte){

    Registers& regis = cpu.getRegisters();

    uint8_t op = (byte & 0b11000000) >> 6;
    uint8_t y = (byte & 0b00111000) >> 3;
    uint8_t z = (byte & 0b00000111);

    std::cout << std::bitset<8>(op) << '\n';
    std::cout << std::bitset<8>(y) << '\n';
    std::cout << std::bitset<8>(z) << std::endl;

    uint8_t* reg, *reg2;

    switch(op) {
        case 0b10:
            switch(y) {
                case 0b100: { // AND r
                    uint8_t val = readRegOrHL(cpu, z);                   
                    regis.A &= val;

                    regis.F.S = (regis.A & 0x80) != 0;
                    regis.F.Z = (regis.A == 0);
                    regis.F.H = true;
                    regis.F.PV = getParity(regis.A);
                    regis.F.N = false;
                    regis.F.C = false;
                    break;
                }
                case 0b110: { // OR r
                    uint8_t val = readRegOrHL(cpu, z);                  
                    regis.A |= val;

                    regis.F.S = (regis.A & 0x80) != 0;
                    regis.F.Z = (regis.A == 0);
                    regis.F.H = false;
                    regis.F.PV = getParity(regis.A);
                    regis.F.N = false;
                    regis.F.C = false;
                    break;
                }

                case 0b111: { // CP r
                    uint8_t val = readRegOrHL(cpu, z);
                    uint16_t res = regis.A - val;

                    regis.F.S = (res & 0x80) != 0;
                    regis.F.Z = ((uint8_t)res == 0);
                    regis.F.H = ((regis.A & 0x0F) < (val & 0x0F));
                    regis.F.PV = checkOverflowAdd(regis.A, val, res);
                    regis.F.N = true;
                    regis.F.C = (regis.A < val);
                    break;
                }
                
                case 0b101: { // XOR r
                    uint8_t val = readRegOrHL(cpu, z);                 
                    regis.A ^= val;

                    regis.F.S = (regis.A & 0x80) != 0;
                    regis.F.Z = (regis.A == 0);
                    regis.F.H = false;
                    regis.F.PV = getParity(regis.A);
                    regis.F.N = false;
                    regis.F.C = false;
                    break;
                }
                
                case 0b000: { // ADD A, r
                    uint8_t val = readRegOrHL(cpu, z);                  
                    uint16_t res = regis.A + val;

                    regis.F.S = (res & 0x80) != 0;
                    regis.F.Z = ((uint8_t)res == 0);
                    regis.F.H = ((regis.A & 0x0F) + (val & 0x0F)) > 0x0F;
                    regis.F.PV = checkOverflowAdd(regis.A, val, res);
                    regis.F.N = false;
                    regis.F.C = (res > 0xFF);

                    regis.A = (uint8_t)res;
                    break;
                }

                case 0b010: { // SUB A, r
                    uint8_t val = readRegOrHL(cpu, z);                  
                    uint16_t res = regis.A - val;

                    regis.F.S = (res & 0x80) != 0;
                    regis.F.Z = ((uint8_t)res == 0);
                    regis.F.H = ((regis.A & 0x0F) < (val & 0x0F));
                    regis.F.PV = checkOverflowSub(regis.A, val, res);
                    regis.F.N = true;
                    regis.F.C = (regis.A < val);

                    regis.A = (uint8_t)res;
                    break;
                }

            }

            break;
        case 0b01:
            if ( y == 0b110 && z == 0b110) { // HALT
                cpu.setHalted(true);
            }

            else if ( y == 0b110 ) { // LD (HL), r
                uint16_t hl = regis.HL();
                reg = registerFromByte(z, &regis);
                cpu.mem.write(hl, *reg);
            }

            else if ( z == 0b110 ) { // LD r, (HL)
               uint16_t hl = regis.HL();
               reg = registerFromByte(y, &regis);
               *reg = cpu.mem.read(hl);
            }

            else { // LD r, r'
                reg = registerFromByte(y, &regis);
                reg2 = registerFromByte(z, &regis);

                if (reg && reg2) {
                    *reg = *reg2;
                }
            }
            break;

        case 0b00:
            switch (y) {
                case 0b000: // NOP
                    break;

                case 0b011: 
                    if (z == 0b000) {} // TODO JR offset
                    break;
            }
            
            switch (z) {
                case 0b110: // LD r, n
                    reg = registerFromByte(y, &regis);
                    *reg = cpu.fetch8(); 
                    break;

                case 0b100: { // INC r
                    uint8_t oldVal = readRegOrHL(cpu, y);
                    uint8_t newVal = oldVal + 1;
                    writeRegOrHL(cpu, y, newVal);
                    
                    regis.F.S = (*reg & 0x80) != 0;
                    regis.F.Z = (*reg == 0);
                    regis.F.H = (oldVal & 0x0F) == 0x0F;
                    regis.F.PV = (oldVal == 0x7F);
                    regis.F.N = false;
                    break;
                }

                case 0b101: { // DEC r
                    uint8_t oldVal = readRegOrHL(cpu, y);
                    uint8_t newVal = oldVal - 1;
                    writeRegOrHL(cpu, y, newVal);

                    regis.F.S = (*reg & 0x80) != 0;
                    regis.F.Z = (*reg == 0);
                    regis.F.H = (oldVal & 0x0F) == 0x00;
                    regis.F.PV = (oldVal == 0x80);
                    regis.F.N = true;
                    break;
                }
            }

            break;

        case 0b11:
            switch(z) {
                case 0b101:
                    if (y == 0b001) { // CALL addr
                        uint16_t addr = cpu.fetch16();
                        cpu.push(regis.PC);
                        regis.PC = addr;
                    } 
                    else if ((y & 0b001) == 0) { // PUSH rp
                        uint16_t val;
                        if (y == 0)      val = regis.BC();
                        else if (y == 2) val = regis.DE();
                        else if (y == 4) val = regis.HL();
                        else             val = regis.AF(); // y=6
                        cpu.push(val);
                    }
                    break;
                    
                case 0b001: // z=1: POP rp ou RET
                    if (y == 0b001) { // RET
                        regis.PC = cpu.pop();
                    } 
                    else if ((y & 0b001) == 0) { // POP rp
                        uint16_t val = cpu.pop();
                        if (y == 0)      regis.setBC(val);
                        else if (y == 2) regis.setDE(val);
                        else if (y == 4) regis.setHL(val);
                        else { // y=6 -> POP AF
                            regis.A = (val >> 8);
                            regis.F.fromByte(val & 0xFF);
                        }
                    }
                    break;
                
                case 0b011: // z=3: JP addr
                    if (y == 0) { // JP addr (C3)
                        regis.PC = cpu.fetch16();
                    }
                    break;
            }
            break;

    }
}

uint8_t* registerFromByte(uint8_t regIndex, Registers* registradores){
    switch(regIndex) {
        case 0b000: return &(registradores->B);
        case 0b001: return &(registradores->C);
        case 0b010: return &(registradores->D);
        case 0b011: return &(registradores->E);
        case 0b100: return &(registradores->H);
        case 0b101: return &(registradores->L);
        case 0b111: return &(registradores->A);
        default: return nullptr;
    }
}

 bool getParity(uint8_t val){
    int count = 0;
    for (int i = 0; i < 8; i++) {
        if (val & (1 << i)) count++;
    }
    return (count % 2 == 0);
}

bool checkOverflowAdd( uint8_t a, uint8_t value, uint16_t res ) {
    return ( (a ^ res) & (value ^ res) & 0x80 ) != 0;
}

bool checkOverflowSub( uint8_t a, uint8_t value, uint16_t res ) {
    return ( (a ^ value) & (a ^ res) & 0x80 ) != 0;
}

#include "instruction_set.h"
#include "cpu.h"
#include <iostream>
#include <bitset>


uint8_t* regHelper(uint8_t regIndex, Registers* registradores);

void instrucao(CPU &cpu, uint8_t byte){

    Registers& regis = cpu.getRegisters();

    uint8_t op = (byte & 0b11000000) >> 6;

    uint8_t y = (byte & 0b00111000) >> 3;

    uint8_t z = (byte & 0b00000111);

    std::cout << std::bitset<8>(op) << '\n';
    std::cout << std::bitset<8>(y) << '\n';
    std::cout << std::bitset<8>(z) << std::endl;

    uint8_t* reg, *reg2;
    regis.A = 0b00000111;

    switch(op) {
        case 0b10:
            switch(y) {
                case 0b100: // AND r
                    reg = regHelper( z, &regis );                   
                    regis.A = regis.A & (*reg);

                    break;
                case 0b110: // OR r
                    reg = regHelper( z, &regis );                   
                    regis.A = regis.A | (*reg);

                    break;

                case 0b111: // CP r
                    

                    break;
                
                case 0b101: // XOR r
                    reg = regHelper( z, &regis );                   
                    regis.A = regis.A ^ (*reg);
                    break;
                
                case 0b000: // ADD A, r
                    reg = regHelper( z, &regis );                   
                    regis.A = regis.A + (*reg);
                    std::cout << std::bitset<8>(regis.A);
                    break;

                case 0b010: // SUB A, r
                    reg = regHelper( z, &regis );                   
                    regis.A = regis.A - (*reg);
                    std::cout << std::bitset<8>(regis.A);
                    break;

            }

            break;
        case 0b01:
            switch(y) {
                case 0b110: // LD (HL), r ou HALT
                    if (z == 0b110){ // HALT
                        
                        
                    }
                    else{ // LD (HL), r
                    
                    }
            }

            if (z == 0b110){ // LD r, (HL)
                

            } 

            else{  // LD r, r'
                reg = regHelper( y, &regis ); 
                reg2 = regHelper( z, &regis ); 

                *reg = *reg2;}

            break;

        case 0b00:
            switch (y) {
                case 0b000: // NOP

                    break;

                case 0b011: 
                    if (z == 0b000){} // JR offset
                }
                    break;
            switch (z) {
                case 0b110: // LD r, n
                    reg = regHelper(y, &regis);
                    *reg = byte;

                    break;

                case 0b100: // INC r
                    reg = regHelper( y, &regis);
                    *reg += 1;

                    break;

                case 0b101: // DEC r
                    reg = regHelper( y, &regis);
                    *reg -= 1;

                    break;
            }

            break;

        case 0b11:
            uint8_t zDeQuatro = (byte & 0b00001111);

            switch(z){
                case 0b101: // CALL aadr
                    break;
                    
                case 0b001: // RET
                    break;
            }
            if (y == 0b000 && z == 0b011){} // JP addr

            if (zDeQuatro == 0b0101){} // PUSH rp

            if (zDeQuatro == 0b0001){} // POP rp

            break;

            }
}

uint8_t* regHelper(uint8_t regIndex, Registers* registradores){
    switch(regIndex) {
        case 0b000: return &(registradores->B);
        case 0b001: return &(registradores->C);
        case 0b010: return &(registradores->D);
        case 0b011: return &(registradores->E);
        case 0b100: return &(registradores->H);
        case 0b101: return &(registradores->L);
        case 0b110: return nullptr;             // (HL) - Memória
        case 0b111: return &(registradores->A);
        default: return nullptr;
    }
}

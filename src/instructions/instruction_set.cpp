#include <cstdint>
#include <iostream>
#include <bitset>
//#include "../cpu/registers.h"



struct Registers {
    uint8_t A = 0;
    uint8_t B = 0; 
    uint8_t C = 0;
    uint8_t D = 0;
    uint8_t E = 0;
    uint8_t H = 0;
    uint8_t L = 0;

    uint16_t PC = 0x0000;
    uint16_t SP = 0xFFFF;
    uint16_t IX = 0x0000;
    uint16_t IY = 0x0000;

    uint16_t AF() const;
    uint16_t BC() const;
    uint16_t DE() const;
    uint16_t HL() const;

    void setBC(uint16_t value);
    void setDE(uint16_t value);
    void setHL(uint16_t value);

    void reset();
};

uint8_t* funcaoFelip(uint8_t regIndex, Registers* registradores);

int main(){
    Registers regis;

    uint8_t byte = 0b10110111; // EXEMPLO

    uint8_t op = (byte & 0b11000000) >> 6;

    uint8_t y = (byte & 0b00111000) >> 3;

    uint8_t z = (byte & 0b00000111);

    std::cout << std::bitset<8>(op) << '\n';
    std::cout << std::bitset<8>(y) << '\n';
    std::cout << std::bitset<8>(z) << std::endl;

    uint8_t* reg;
    regis.A = 0b11000111;

    switch(op) {
        case 0b10:
            switch(y) {
                case 0b100: // AND r
                    reg = funcaoFelip( z, &regis );                   
                    regis.A = regis.A & (*reg);

                    break;
                case 0b110: // OR r
                    reg = funcaoFelip( z, &regis );                   
                    regis.A = regis.A | (*reg);

                    std::cout << std::bitset<8>(regis.A);

                    break;

                case 0b111: // CP r
                    

                    break;
                
                case 0b101: // XOR r
                    break;
                
                case 0b000: // ADD A, r
                    break;

                case 0b010: // SUB A, r
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

            if (z == 0b110){} // LD r, (HL)

            else{} // LD r, r'

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
                    break;

                case 0b100: // INC r
                    break;

                case 0b101: // DEC r
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

uint8_t* funcaoFelip(uint8_t regIndex, Registers* registradores){
    registradores->H = 0b00110001;
    return &(registradores->H);
}
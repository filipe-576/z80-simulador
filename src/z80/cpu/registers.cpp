#include "registers.h"

void Flags::reset(){
    S = false;
    Z = false;
    H = false;
    PV = false;
    N = false;
    C = false;
}


uint8_t Flags::toByte() const{
    bool flags[6] = {S, Z, H, PV, N, C};
    uint8_t byte = 0;

    for( bool i: flags ){
        byte = byte << 1;
        if( i ){
            byte += 1;
        }
    }
    return byte;
}


void Flags::fromByte(uint8_t byte){
    S = (byte &  0b100000) != 0;
    Z = (byte &  0b010000) != 0;
    H = (byte &  0b001000) != 0;
    PV = (byte & 0b000100) != 0;
    N = (byte &  0b000010) != 0;
    C = (byte &  0b000001) != 0;
}


void Registers::reset(){
    A = 0;
    B = 0; 
    C = 0;
    D = 0;
    E = 0;
    H = 0;
    L = 0;
    PC = 0x0000;
    SP = 0xFFFF;
    IX = 0x0000;
    IY = 0x0000;

    F.reset();
}


uint16_t Registers::AF() const{
    uint16_t pair = A;
    pair = pair << 8;
    pair += F.toByte();

    return pair;
}


uint16_t Registers::BC() const{
    uint16_t pair = B;
    pair = pair << 8;
    pair += C;

    return pair;
}


uint16_t Registers::DE() const{
    uint16_t pair = D;
    pair = pair << 8;
    pair += E;

    return pair;
}


uint16_t Registers::HL() const{
    uint16_t pair = H;
    pair = pair << 8;
    pair += L;

    return pair;
}


void Registers::setBC(uint16_t value){
    B = (value & 0b1111111100000000) >> 8;
    C = value & 0b0000000011111111;
}


void Registers::setDE(uint16_t value){
    D = (value & 0b1111111100000000) >> 8;
    E = value & 0b0000000011111111;
}


void Registers::setHL(uint16_t value){
    H = (value & 0b1111111100000000) >> 8;
    L = value & 0b0000000011111111;   
}

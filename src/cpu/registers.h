#pragma once

#include <cstdint>

struct Flags {
    bool S  = false;
    bool Z  = false;
    bool H  = false;
    bool PV = false;
    bool N  = false;
    bool C  = false;

    void reset();
    uint8_t toByte() const;
    void fromByte(uint8_t byte);
};

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

    Flags F;

    uint16_t AF() const;
    uint16_t BC() const;
    uint16_t DE() const;
    uint16_t HL() const;

    void setBC(uint16_t value);
    void setDE(uint16_t value);
    void setHL(uint16_t value);

    void reset();
};
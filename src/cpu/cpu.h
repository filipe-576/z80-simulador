#pragma once

#include <cstdint>
#include "../memory/memory.h"
#include "registers.h"

class CPU {
public:
    CPU(Memory& memory);

    void reset();
    void step();

    Registers& getRegisters();

private:
    Memory& mem;
    Registers regs;

    uint8_t fetch8();
    uint16_t fetch16();
    void execute(uint8_t opcode);
};
#include "cpu.h"


void CPU::push(uint16_t value)
{
    uint8_t highByte = static_cast<uint8_t>(value >> 8);
    uint8_t lowByte = value & 0xFF;

    regs.SP--;
    mem.write(regs.SP, highByte);
    regs.SP--;
    mem.write(regs.SP, lowByte);
}


uint16_t CPU::pop()
{
    uint8_t lowByte = mem.read(regs.SP);
    regs.SP++;
    uint8_t highByte = mem.read(regs.SP);
    regs.SP++;

    uint16_t value = static_cast<uint16_t>(highByte) << 8;
    value = value | lowByte;

    return value;
}
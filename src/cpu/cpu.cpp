#include <cstdint>
#include "cpu.h"

/** 
 * @brief Armazena um valor na pilha e decrementa o SP
 * 
 * @param value Valor de 16 bits a ser armaznado.
 */
void CPU::push( uint16_t value)
{
    uint8_t highByte = static_cast<uint8_t>(value >> 8);
    uint8_t lowByte = value & 0xFF;

    regs.SP--;
    mem.write(regs.SP, highByte);
    regs.SP--;
    mem.write(regs.SP, lowByte);
}


/**
 * @brief Retorna o valor do topo da pilha e incrementa o SP
 * 
 * @return Valor de 16 bits que estava no topo da pilha
 */
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
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


/** 
 * @brief Armazena um valor na pilha e decrementa o SP
 * 
 * @param value Valor de 16 bits a ser armaznado.
 */
    void push(uint16_t value);


/**
 * @brief Retorna o valor do topo da pilha e incrementa o SP
 * 
 * @return Valor de 16 bits que estava no topo da pilha
 */
    uint16_t pop();
};
#pragma once

#include <cstdint>
#include "../memory/memory.h"
#include "../instructions/instruction_set.h"
#include "registers.h"

class CPU {
public:
    CPU(Memory& memory);
    Memory& mem;

/** 
 * @brief   Reseta os registradores e memória
 * 
 */
    void reset();


/** 
 * @brief   Executa um passo do programa. Pega o próximo byte e executa a instrução correspondente.
 * 
 */
    void step();

    bool isHalted() const;
    
    /** 
    * @brief   Interrompe a execução do programa pela CPU.
    * 
    */
    void setHalted(bool value);
    
    Registers& getRegisters();

    /** 
 * @brief   Retorna o pŕoximo byte do programa e incrementa o PC
 * 
 * @return  O pŕoximo byte do programa
 */
    uint8_t fetch8();
    
    
/**
 * @brief   Retorna os próximos 2 bytes do programa e incrementa o PC
 * 
 * @return  Os próximos 2 bytes do programa
 */
    uint16_t fetch16();


/** 
 * @brief   Armazena um valor na pilha e decrementa o SP
 * 
 * @param   value Valor de 16 bits a ser armaznado.
 */
    void push(uint16_t value);


/**
 * @brief   Retorna o valor do topo da pilha e incrementa o SP
 * 
 * @return  Valor de 16 bits que estava no topo da pilha
 */
    uint16_t pop();


    private:

    Registers regs;
    bool halted = false;


};
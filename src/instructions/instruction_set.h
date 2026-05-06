#pragma once

#include <cstdint>
#include "../cpu/cpu.h"
#include "../cpu/registers.h"

/**
 * @brief Decodifica um byte da memória e executa a instrução
 *
 * @param cpu
 * @param byte
 */
void instrucao(CPU &cpu, uint8_t byte);

/**
 * @brief Recebe o opcode e retorna uma referência do registrador correspondente
 *
 * @param regIndex
 * @param registradores
 * @return retorna a referência do registrador
 */
uint8_t* registerFromByte(uint8_t regIndex, Registers* registradores);

/**
 * @brief Verifica a paridade do byte
 *
 * @param val
 * @return Retorna se o byte é par ou não
 */
bool getParity(uint8_t val);

/**
 * @brief Verifica overflow em operações de adição
 *
 * @param a Acumulador
 * @param value Valor a ser somado
 * @param res Resultado da soma
 * @return True se houve overflow
 */
bool checkOverflowAdd( uint8_t a, uint8_t value, uint8_t res );

/**
 * @brief Verifica overflow em operações de subtração
 *
 * @param a Acumulador
 * @param value Valor a ser subtraído
 * @param res Resultado da subtração
 * @return True se houve overflow
 */
bool checkOverflowSub( uint8_t a, uint8_t value, uint8_t res );

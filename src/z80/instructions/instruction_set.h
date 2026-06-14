#pragma once

#include <cstdint>
#include "../cpu/cpu.h"
#include "../cpu/registers.h"

/**
 * @brief Decodifica um byte lido da memória para determinar e executar a operação correspondente na CPU.
 *
 * @param cpu   Referência para a instância principal da CPU (acesso à memória e pilha).
 * @param byte  Opcode de 8 bits a ser decodificado.
 */
void executeInstruction(CPU& cpu, uint8_t byte);


/**
 * @brief Mapeia o índice numérico (0 a 7) proveniente do opcode para o ponteiro do registrador físico do Z80.
 *
 * @param regIndex      Identificador de 3 bits do registrador alvo.
 * @param registradores Ponteiro para a struct que contém os registradores da CPU.
 * @return              Ponteiro para o registrador de 8 bits associado (ou nullptr se for índice indireto).
 */
uint8_t* registerFromByte(uint8_t regIndex, Registers* registradores);


/**
 * @brief Conta a quantidade de bits ativos para definir a flag de paridade.
 *
 * @param val  Valor de 8 bits resultante de uma operação anterior.
 * @return     Verdadeiro se o número de bits em nível lógico alto for par.
 */
bool getParity(uint8_t val);


/**
 * @brief Verifica se houve transbordo (overflow) do limite de 8 bits com sinal durante uma soma (usado pela Flag PV).
 *
 * @param a      Acumulador original (parcela 1).
 * @param value  Valor somado (parcela 2).
 * @param res    Resultado final de 16 bits da operação.
 * @return       Verdadeiro caso o sinal numérico tenha se corrompido no limite superior.
 */
bool checkOverflowAdd(uint8_t a, uint8_t value, uint16_t res);


/**
 * @brief Verifica se houve transbordo (overflow) do limite de 8 bits com sinal durante uma subtração (usado pela Flag PV).
 *
 * @param a      Acumulador original (minuendo).
 * @param value  Valor subtraído (subtraendo).
 * @param res    Resultado final de 16 bits da operação.
 * @return       Verdadeiro caso o sinal numérico tenha se corrompido no limite inferior.
 */
bool checkOverflowSub(uint8_t a, uint8_t value, uint16_t res);


/**
 * @brief Abstrai a leitura, identificando se a operação exige leitura direta de registrador ou indireta da memória via (HL).
 *
 * @param cpu    Referência para a CPU.
 * @param index  Índice de 3 bits extraído do opcode.
 * @return       O valor contido no registrador ou no endereço de memória apontado por HL.
 */
uint8_t readRegOrHL(CPU& cpu, uint8_t index);


/**
 * @brief Abstrai a escrita, gravando no registrador correspondente ou indiretamente na memória apontada por HL.
 *
 * @param cpu    Referência para a CPU.
 * @param index  Índice de 3 bits alvo da escrita.
 * @param value  Valor a ser gravado.
 */
void writeRegOrHL(CPU& cpu, uint8_t index, uint8_t value);
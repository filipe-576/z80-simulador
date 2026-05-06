#include "../cpu/registers.h"
#include "../cpu/cpu.h"
#include <cstdint>
#include <iostream>
#include <bitset>

class CPU;

void instrucao(CPU &cpu, uint8_t byte);
uint8_t* registerFromByte(uint8_t regIndex, Registers* registradores);
bool getParity(uint8_t val);
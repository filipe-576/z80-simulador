#include <algorithm>
#include <cstdint>
#include "memory.h"

Memory::Memory()
{
    reset();
}


/**
 * @brief   Preenche a memória com 0's
 */
void Memory::reset()
{
    std::fill(mem.begin(), mem.end(), 0);
}


/**
 * @brief   Lê o byte do endereço passado como argumento
 *
 * @param   address  Endereço de 16 bits.
 * @return  Byte armazenado naquele endereço.
 */
uint8_t Memory::read(uint16_t address) const
{
    return mem[address];
}


/**
 * @brief   Escreve um valor no byte do especificado
 *
 * @param   address  Endereço de 16 bits.
 * @param   value    Valor de 8 bits.
 */
void Memory::write(uint16_t address, uint8_t value)
{
    mem[address] = value;
}


/**
 * @brief   Carrega a memória com o conteudo de 'data'
 *
 * @param   data  Ponteiro para o conteudo a ser carregado na memória.
 * @param   length  Tamanho do conteudo apontado por 'data'.
 */
void Memory::load(const uint8_t* data, int length)
{
    if(length <= 0 || data == nullptr) return;

    int memSize = Memory::SIZE; //tive que fazer isso pois SIZE é const e nao tem como ser passada por ref para std::min

    int size = std::min(length, memSize);
    
    std::copy(data, data + size, mem.begin());
}


/**
 * @brief   Retorna um ponteiro com todo o conteúdo da memória
 *
 * @return  Ponteiro para a posição 0 da memória.
 */
const uint8_t* Memory::get_array() const
{
    return mem.data();
}

#pragma once

#include <cstdint>
#include <array>

class Memory {
public:
    static const int SIZE = 65536;
    
    Memory();

/**
 * @brief   Preenche a memória com 0's
 */
    void reset();


/**
 * @brief   Lê o byte do endereço passado como argumento
 *
 * @param   address  Endereço de 16 bits.
 * @return  Byte armazenado naquele endereço.
 */
    uint8_t read(uint16_t address) const;


/**
 * @brief   Escreve um valor no byte do especificado
 *
 * @param   address  Endereço de 16 bits.
 * @param   value    Valor de 8 bits.
 */
    void write(uint16_t address, uint8_t value);


/**
 * @brief   Carrega a memória com o conteudo de 'data'
 *
 * @param   data  Ponteiro para o conteudo a ser carregado na memória.
 * @param   length  Tamanho do conteudo apontado por 'data'.
 */
    void load(const uint8_t* data, int length);


/**
 * @brief   Retorna um ponteiro com todo o conteúdo da memória
 *
 * @return  Ponteiro para a posição 0 da memória.
 */
    const uint8_t* get_array() const;

    uint8_t* Outro_get_array();

    
private:
    std::array<uint8_t, SIZE> mem;
};
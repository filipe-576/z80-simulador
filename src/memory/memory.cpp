#include <algorithm>
#include <cstdint>
#include "memory.h"

Memory::Memory()
{
    reset();
}


void Memory::reset()
{
    std::fill(mem.begin(), mem.end(), 0);
}


uint8_t Memory::read(uint16_t address) const
{
    return mem[address];
}


void Memory::write(uint16_t address, uint8_t value)
{
    mem[address] = value;
}


void Memory::load(const uint8_t* data, int length)
{
    if(length <= 0 || data == nullptr) return;

    int memSize = Memory::SIZE; //tive que fazer isso pois SIZE é const e nao tem como ser passada por ref para std::min

    int size = std::min(length, memSize);
    
    std::copy(data, data + size, mem.begin());
}


const uint8_t* Memory::get_array() const
{
    return mem.data();
}



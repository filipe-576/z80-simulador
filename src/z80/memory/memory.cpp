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


void Memory::load(const uint8_t* data, int length, uint16_t address)
{
    if(length <= 0 || data == nullptr) return;

    if( length + address > Memory::SIZE ) return;
    
    std::copy(data, data + length, mem.begin() + address);
}


const uint8_t* Memory::get_array() const
{
    return mem.data();
}

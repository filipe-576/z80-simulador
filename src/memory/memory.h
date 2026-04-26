#pragma once

#include <cstdint>
#include <array>

class Memory {
public:
    static const int SIZE = 65536;
    
    Memory();
    
    void reset();
    
    uint8_t read(uint16_t address) const;
    void write(uint16_t address, uint8_t value);
    void load(const uint8_t* data, int length);

    const uint8_t* get_array() const;
    
private:
    std::array<uint8_t, SIZE> mem;
};
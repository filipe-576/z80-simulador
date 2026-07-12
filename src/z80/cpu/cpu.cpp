#include "cpu.h"
#include "../memory/memory.h"
#include "../instructions/instruction_set.h"
#include <cstdint>

CPU::CPU(Memory& memory): mem(memory){
}


void CPU::reset(){
    regs.reset();
    mem.reset();
    halted = false;
}
//Para ter um boatao de rest na interface
void CPU::reset_Gui(){
    regs.reset();
    halted = false;
}


Registers& CPU::getRegisters(){
    return regs;
}


bool CPU::isHalted() const{
    return halted;
}


void CPU::setHalted(bool value){
    halted = value;
}


void CPU::step(){
    
     if (halted) {
        return;
    }

    uint8_t byte = fetch8();

    executeInstruction(*this, byte);
}


uint8_t CPU::fetch8(){
    uint16_t address = getRegisters().PC;
    ++getRegisters().PC;

    return mem.read(address);
}

uint16_t CPU::fetch16(){
    uint16_t address = getRegisters().PC;
    getRegisters().PC += 2;

    uint16_t lowByte = mem.read(address);
    uint16_t highByte = mem.read(address+1);

    return (highByte << 8) | lowByte;
}




void CPU::push(uint16_t value)
{
    uint8_t highByte = static_cast<uint8_t>(value >> 8);
    uint8_t lowByte = value & 0xFF;

    regs.SP--;
    mem.write(regs.SP, highByte);
    regs.SP--;
    mem.write(regs.SP, lowByte);
}


uint16_t CPU::pop()
{
    uint8_t lowByte = mem.read(regs.SP);
    regs.SP++;
    uint8_t highByte = mem.read(regs.SP);
    regs.SP++;

    uint16_t value = static_cast<uint16_t>(highByte) << 8;
    value = value | lowByte;

    return value;
}

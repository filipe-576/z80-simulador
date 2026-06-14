#include "instruction_set.h"
#include "cpu.h"
#include <iostream>
#include <bitset>
#include <cstdint>

uint8_t* registerFromByte(uint8_t regIndex, Registers* registradores) {
    switch (regIndex) {
        case 0b000: return &(registradores->B);
        case 0b001: return &(registradores->C);
        case 0b010: return &(registradores->D);
        case 0b011: return &(registradores->E);
        case 0b100: return &(registradores->H);
        case 0b101: return &(registradores->L);
        case 0b111: return &(registradores->A);
        default: return nullptr;
    }
}


bool getParity(uint8_t val) {
    uint8_t count = 0;

    for (uint8_t i = 0; i < 8; i++) {
        if (val & (1 << i)) {
            count++;
        }
    }

    return (count % 2 == 0);
}


bool checkOverflowAdd(uint8_t a, uint8_t value, uint16_t res) {
    // O overflow de adição no Z80 ocorre quando o sinal dos operandos é igual, mas diferente do sinal do resultado.
    return ((a ^ res) & (value ^ res) & 0x80) != 0;
}


bool checkOverflowSub(uint8_t a, uint8_t value, uint16_t res) {
    // O overflow de subtração ocorre quando os sinais dos operandos são diferentes e o sinal do resultado difere do minuendo.
    return ((a ^ value) & (a ^ res) & 0x80) != 0;
}


uint8_t readRegOrHL(CPU& cpu, uint8_t index) {
    // O índice 6 (0b110) não mapeia para um registrador interno, mas sim para o dado na memória apontada por HL.
    if (index == 0b110) {
        return cpu.mem.read(cpu.getRegisters().HL());
    }

    uint8_t* reg = registerFromByte(index, &cpu.getRegisters());

    return *reg;
}


void writeRegOrHL(CPU& cpu, uint8_t index, uint8_t value) {
    // Mantém a transparência do barramento: desvia a escrita para a RAM se o índice alvo for o 6.
    if (index == 0b110) {
        cpu.mem.write(cpu.getRegisters().HL(), value);
    } else {
        uint8_t* reg = registerFromByte(index, &cpu.getRegisters());
        *reg = value;
    }
}


void executeInstruction(CPU& cpu, uint8_t byte) {
    Registers& regis = cpu.getRegisters();

    // A estrutura binária do opcode do Z80 (xx yyy zzz) categoriza a classe, registrador alvo e operação.
    uint8_t op = (byte & 0b11000000) >> 6;
    uint8_t y = (byte & 0b00111000) >> 3;
    uint8_t z = (byte & 0b00000111);

    // Impressão visual dos campos extraídos para auxiliar no log de depuração do emulador.
    std::cout << std::bitset<8>(op) << '\n';
    std::cout << std::bitset<8>(y) << '\n';
    std::cout << std::bitset<8>(z) << std::endl;

    uint8_t* reg;
    uint8_t* reg2;

    switch (op) {
        case 0b10:
            switch (y) {
                case 0b100: {
                    // Zera os bits configurados na máscara (AND) para realizar testes lógicos no Acumulador.
                    uint8_t val = readRegOrHL(cpu, z);                   
                    regis.A &= val;

                    regis.F.S = (regis.A & 0x80) != 0;
                    regis.F.Z = (regis.A == 0);
                    regis.F.H = true;
                    regis.F.PV = getParity(regis.A);
                    regis.F.N = false;
                    regis.F.C = false;

                    break;
                }

                case 0b110: {
                    // Combina bits através de OR lógico para setar flags lógicas sem afetar o transporte (Carry).
                    uint8_t val = readRegOrHL(cpu, z);                   
                    regis.A |= val;

                    regis.F.S = (regis.A & 0x80) != 0;
                    regis.F.Z = (regis.A == 0);
                    regis.F.H = false;
                    regis.F.PV = getParity(regis.A);
                    regis.F.N = false;
                    regis.F.C = false;

                    break;
                }

                case 0b111: {
                    // Executa subtração virtual para comparar valores (CP), atualizando as flags sem alterar o Acumulador.
                    uint8_t val = readRegOrHL(cpu, z);
                    uint16_t res = regis.A - val;

                    regis.F.S = (res & 0x80) != 0;
                    regis.F.Z = ((uint8_t)res == 0);
                    regis.F.H = ((regis.A & 0x0F) < (val & 0x0F));
                    regis.F.PV = checkOverflowSub(regis.A, val, res);
                    regis.F.N = true;
                    regis.F.C = (regis.A < val);

                    break;
                }
                
                case 0b101: {
                    // Operador XOR inverte os bits, sendo uma técnica eficiente e clássica para zerar o Acumulador.
                    uint8_t val = readRegOrHL(cpu, z);                   
                    regis.A ^= val;

                    regis.F.S = (regis.A & 0x80) != 0;
                    regis.F.Z = (regis.A == 0);
                    regis.F.H = false;
                    regis.F.PV = getParity(regis.A);
                    regis.F.N = false;
                    regis.F.C = false;

                    break;
                }
                
                case 0b000: {
                    // Adição aritmética: atualiza o acumulador e registra transporte (Carry) ou transbordo (Overflow).                 
                    uint8_t val = readRegOrHL(cpu, z);
                    uint16_t res = regis.A + val;

                    regis.F.S = (res & 0x80) != 0;
                    regis.F.Z = ((uint8_t)res == 0);
                    regis.F.H = ((regis.A & 0x0F) + (val & 0x0F)) > 0x0F;
                    regis.F.PV = checkOverflowAdd(regis.A, val, res);
                    regis.F.N = false;
                    regis.F.C = (res > 0xFF);

                    regis.A = static_cast<uint8_t>(res);

                    break;
                }

                case 0b010: {
                    // Subtração aritmética real, diferente do CP que apenas simula o resultado.
                    uint8_t val = readRegOrHL(cpu, z);
                    uint16_t res = regis.A - val;

                    regis.F.S = (res & 0x80) != 0;
                    regis.F.Z = ((uint8_t)res == 0);
                    regis.F.H = ((regis.A & 0x0F) < (val & 0x0F));
                    regis.F.PV = checkOverflowSub(regis.A, val, res);
                    regis.F.N = true;
                    regis.F.C = (regis.A < val);

                    regis.A = static_cast<uint8_t>(res);

                    break;
                }
            }
            break;

        case 0b01:
            // Interrompe o processador intencionalmente, congelando o pipeline até o próximo interrupt.
            if (y == 0b110 && z == 0b110) {
                cpu.setHalted(true);
            }
            // Move os dados do registrador para o endereço físico mapeado no par HL.
            else if (y == 0b110) {
                uint16_t hl = regis.HL();
                reg = registerFromByte(z, &regis);
                cpu.mem.write(hl, *reg);
            }
            // Lê um byte do endereço de memória salvo em HL e alimenta o registrador.
            else if (z == 0b110) {
               uint16_t hl = regis.HL();
               reg = registerFromByte(y, &regis);
               *reg = cpu.mem.read(hl);
            }
            // Movimentação trivial (cópia de barramento interno) entre registradores base.
            else {
                reg = registerFromByte(y, &regis);
                reg2 = registerFromByte(z, &regis);

                if (reg && reg2) {
                    *reg = *reg2;
                }
            }
            break;

        case 0b00:
            // Carga imediata: insere a constante (n) que acompanha o opcode no destino designado.
            if (z == 0b110) { 
                uint8_t n = cpu.fetch8();
                writeRegOrHL(cpu, y, n);
            }
            // Incrementa o alvo em 1 de forma atômica (útil para loops em registradores contadores).
            else if (z == 0b100) { 
                uint8_t oldVal = readRegOrHL(cpu, y);
                uint8_t newVal = oldVal + 1;
                writeRegOrHL(cpu, y, newVal);
                
                regis.F.S = (newVal & 0x80) != 0;
                regis.F.Z = (newVal == 0);
                regis.F.H = (oldVal & 0x0F) == 0x0F;
                regis.F.PV = (oldVal == 0x7F);
                regis.F.N = false;
            }
            // Decrementa o alvo, geralmente utilizado na finalização de iterações do assembly (ex: DJNZ).
            else if (z == 0b101) { 
                uint8_t oldVal = readRegOrHL(cpu, y);
                uint8_t newVal = oldVal - 1;
                writeRegOrHL(cpu, y, newVal);

                regis.F.S = (newVal & 0x80) != 0;
                regis.F.Z = (newVal == 0);
                regis.F.H = (oldVal & 0x0F) == 0x00;
                regis.F.PV = (oldVal == 0x80);
                regis.F.N = true;
            }
            // Opcode que demanda salto no fluxo (Jump) ou Nenhuma Operação (ciclos de delay).
            else if (z == 0b000) {
                switch (y) {
                    case 0b000:
                        // NOP: Não altera registradores nem a memória, mas consome tempo de relógio.
                        break;

                    case 0b011: {
                        // JR: Salto relativo, usa um signed offset para pular código para frente ou trás.
                        int8_t offset = static_cast<int8_t>(cpu.fetch8()); 
                        regis.PC += offset;
                        break;
                    }
                }
            }
            break;
        
        case 0b11:
            switch (z) {
                case 0b101:
                    // Desvio de rotina que empilha o PC de retorno antes de voar para o destino.
                    if (y == 0b001) {
                        uint16_t addr = cpu.fetch16();
                        cpu.push(regis.PC);
                        regis.PC = addr;
                    } 
                    // Salva contextualmente os registradores pareados no topo da pilha.
                    else if ((y & 0b001) == 0) {
                        uint16_t val;

                        if (y == 0)      val = regis.BC();
                        else if (y == 2) val = regis.DE();
                        else if (y == 4) val = regis.HL();
                        else             val = regis.AF();

                        cpu.push(val);
                    }
                    break;
                    
                case 0b001:
                    // Retorno incondicional que desempilha o endereço chamador guardado pelo último CALL.
                    if (y == 0b001) {
                        regis.PC = cpu.pop();
                    }
                    // Recupera um par de registradores de 16 bits removendo da pilha (Stack Memory).
                    else if ((y & 0b001) == 0) {
                        uint16_t val = cpu.pop();

                        if (y == 0)      regis.setBC(val);
                        else if (y == 2) regis.setDE(val);
                        else if (y == 4) regis.setHL(val);
                        else {
                            regis.A = (val >> 8);
                            regis.F.fromByte(val & 0xFF);
                        }
                    }
                    break;
                
                case 0b011:
                    // Desvio direto do Program Counter (PC) baseado no endereço lido (Jump absoluto).
                    if (y == 0) {
                        regis.PC = cpu.fetch16();
                    }
                    break;
            }
            break;
    }
}
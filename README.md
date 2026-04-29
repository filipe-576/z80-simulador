# 💻 Simulador Z80 (`z80-simulador`)

Um emulador/simulador do clássico microprocessador Zilog Z80, desenvolvido do zero em C++ moderno. O objetivo deste projeto é fornecer uma simulação precisa da arquitetura do Z80, focando na precisão histórica da execução de instruções, gerenciamento de memória e manipulação de registradores
## 📌 Visão Geral do Projeto

Este projeto é desenvolvido colaborativamente e particionado em módulos rigorosos de engenharia de software para garantir escalabilidade e separação de responsabilidades. O emulador é capaz de gerenciar o barramento de memória, simular o ciclo de *fetch-decode-execute* do Z80 e interagir com uma interface gráfica.

### ✨ Principais Funcionalidades (Em Desenvolvimento)

* **Arquitetura 100% C++17:** Código moderno utilizando a Standard Template Library (STL) para segurança e performance.
* **Memória Isolada:** Módulo de barramento de 64KB (Endereçamento de 16-bits) com proteção contra overflow.
* **Precisão Little-Endian:** Simulação fiel da ordenação de bytes do hardware original.
* **Registradores Completos:** Implementação dos registradores de 8-bits (A, B, C, D, E, H, L) e 16-bits (PC, SP, IX, IY), juntamente com a Flag struct (Z, S, H, PV, N, C).
* **Controle de Pilha Lógico:** Simulação precisa das operações de `PUSH` e `POP` controladas dinamicamente pela CPU.
* **Interface Gráfica (GUI):** Módulo dedicado para visualização em tempo real do estado da máquina.

---

## 🏗️ Estrutura da Arquitetura

O projeto foi subdividido para facilitar o desenvolvimento paralelo pela equipe:

* `src/cpu/`: Contém o núcleo do processador (`cpu.cpp`) e o estado da máquina (`registers.cpp`).
* `src/memory/`: Gerencia o barramento de RAM/ROM de 64KB (`memory.cpp`).
* `src/instructions/`: Lida com a decodificação e execução do vasto Conjunto de Instruções (`instruction_set.cpp`).
* `src/gui/`: Lida com a renderização da interface e do Memory Viewer (`gui.cpp`).
* `tests/`: Ambiente de validação e testes unitários (`test.cpp`).

---

## 🛠️ Pré-requisitos

Para compilar e executar o simulador, você precisará das seguintes ferramentas instaladas no seu ambiente:

* **Compilador C++:** Compatível com o padrão **C++17** (ex: GCC/MinGW, Clang, MSVC).
* **CMake:** Versão 3.16 ou superior (Gerador de Build System).

---

## 🚀 Como Compilar (Build)

O projeto utiliza o **CMake** para o gerenciamento de compilação, o que permite que ele seja multiplataforma. 

### Via Terminal (Windows com MinGW / Linux / Mac)

1. Clone o repositório:
   ```bash
   git clone [https://github.com/SEU_USUARIO/z80-simulador.git](https://github.com/SEU_USUARIO/z80-simulador.git)
   cd z80-simulador 
   ```
# Simulador Z80

Conjunto de ferramentas para desenvolvimento e simulação de programas para o microprocessador **Zilog Z80**, desenvolvido em C++. O projeto cobre toda a cadeia de desenvolvimento: pré-processamento de macros, montagem, ligação e execução simulada.

---

## Visão Geral

O projeto é composto por quatro ferramentas independentes que formam uma pipeline completa:

```
arquivo.asm
    │
    ▼
[ macro_processor ]  ── expande macros ──►  arquivo_expandido.asm
    │
    ▼
[   assembler   ]    ── monta ──────────►  arquivo.o  (arquivo objeto)
    │
    ▼
[    linker     ]    ── liga/realoca ──►  arquivo.out (executável)
    │
    ▼
[ z80-simulador ]    ── executa ─────────► simulação com GUI
```

---

## Funcionalidades

### Processador de Macros (`macro_processor`)

- Processamento em **passagem única**
- Suporte a **macros com parâmetros**
- Expansão de **macros aninhadas
- Geração de **rótulos únicos** via diretiva `.SER`
- Saída: arquivo `.asm` pré-processado, pronto para o montador

### Montador (`assembler`)

- Montador de **duas passagens** para o conjunto de instruções Z80
- **Tabela de símbolos** com suporte a símbolos locais (`LOCAL`), globais (`INTDEF`) e externos (`INTUSE`)
- Geração de **mapa de realocação** para símbolos de endereço relativo
- Suporte às diretivas: `END`, `EQU`, `DS`, `DC`, `INTDEF`, `INTUSE`
- Instruções suportadas: `LD`, `ADD`, `SUB`, `INC`, `DEC`, `AND`, `OR`, `XOR`, `CP`, `JP`, `JR`, `CALL`, `RET`, `PUSH`, `POP`, `NOP`, `HALT`
- Saída: arquivo objeto `.o` em formato JSON contendo código de máquina, tabela de definições, tabela de uso e mapa de realocação

### Ligador (`linker`)

- Resolução de referências externas entre módulos objeto
- Suporte a **realocação de endereços** via flag `-r`
- Saída: arquivo executável `.out` em formato JSON

### Simulador Z80 (`z80-simulador`)

- Simula o ciclo **fetch-decode-execute** do Z80
- **Memória isolada** de 64 KB (endereçamento de 16 bits) com proteção contra overflow
- **Registradores completos**: A, B, C, D, E, H, L (8 bits) e PC, SP, IX, IY (16 bits)
- **Flags**: Z (zero), S (sinal), H (half-carry), PV (paridade/overflow), N, C (carry)
- **Precisão little-endian** fiel ao hardware original
- **Carregador absoluto**: usa o endereço base definido pelo ligador
- **Carregador relativo**: realoca endereços em tempo de carga usando o mapa de realocação
- **Interface gráfica (GUI)** para visualização em tempo real do estado da máquina (via ImGui + OpenGL)

---

## Estrutura do Projeto

```
z80-simulador/
├── src/
│   ├── macro/          # Processador de macros
│   ├── assembler/      # Montador
│   ├── linker/         # Ligador
│   ├── z80/
│   │   ├── cpu/        # Núcleo do processador (cpu.cpp, registers.cpp)
│   │   ├── memory/     # Barramento de memória de 64KB
│   │   ├── instructions/ # Decodificação e execução de instruções
│   │   └── gui/        # Interface gráfica (ImGui)
│   ├── utils.cpp / utils.h  # Utilitários compartilhados
│   └── vm.cpp          # Ponto de entrada do simulador
├── external/
│   └── imgui/          # Biblioteca Dear ImGui
├── tests/              # Testes unitários
└── CMakeLists.txt
```

---

## Pré-requisitos

- **Compilador C++** com suporte ao padrão **C++17** (GCC ≥ 7, Clang ≥ 5)
- **CMake** versão 3.16 ou superior
- **GLFW3** e **OpenGL** (para o simulador com GUI)

### Linux (Ubuntu/Debian)

```bash
sudo apt install cmake libglfw3-dev libgl-dev
```

### Windows

**MSYS2** com MinGW-w64:

1. Instale o [MSYS2](https://www.msys2.org/)
2. Abra o terminal **MSYS2 UCRT64** e instale as dependências:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc \
          mingw-w64-ucrt-x86_64-cmake \
          mingw-w64-ucrt-x86_64-ninja \
          mingw-w64-ucrt-x86_64-glfw
```

---

## Como Compilar

### Linux

```bash
# Clone o repositório
git clone <url-do-repositorio>
cd z80-simulador

# Crie o diretório de build e compile
cmake -B build
cmake --build build
```

Os executáveis serão gerados em `build/`:

- `build/macro_processor`
- `build/assembler`
- `build/linker`
- `build/z80-simulador`

### Windows (MSYS2 UCRT64)

No terminal **MSYS2 UCRT64**, dentro do diretório do projeto:

```bash
git clone <url-do-repositorio>
cd z80-simulador

cmake -B build -G Ninja
cmake --build build
```

Os executáveis serão gerados em `build/` com a extensão `.exe`:

- `build/macro_processor.exe`
- `build/assembler.exe`
- `build/linker.exe`
- `build/z80-simulador.exe`

---

## Como Executar

### 1. Processador de Macros

Expande macros do código assembly antes da montagem.

```bash
./macro_processor <arquivo.asm> [-o <saida.asm>]
```

| Argumento        | Descrição                                 |
| ---------------- | ----------------------------------------- |
| `<arquivo.asm>`  | Arquivo assembly de entrada (obrigatório) |
| `-o <saida.asm>` | Arquivo de saída (padrão: `a.o`)          |

**Exemplo:**

```bash
./macro_processor programa.asm -o programa_expandido.asm
```

---

### 2. Montador

Monta o código assembly (pré-processado ou não) e gera um arquivo objeto.

```bash
./assembler <arquivo.asm> [-o <saida.o>]
```

| Argumento       | Descrição                                 |
| --------------- | ----------------------------------------- |
| `<arquivo.asm>` | Arquivo assembly de entrada (obrigatório) |
| `-o <saida.o>`  | Arquivo objeto de saída (padrão: `a.o`)   |
> Saída deve utilizar extensão `.out` para executar diretamente ignorando o ligador.

**Exemplo:**

```bash
./assembler programa_expandido.asm -o programa.o
```

---

### 3. Ligador

Liga um ou mais módulos objeto e gera o arquivo executável. A realocação de endereços é opcional.

```bash
./linker <arquivo1.o> [<arquivo2.o>...] [-r <endbase>]
```

| Argumento      | Descrição                                        |
| -------------- | ------------------------------------------------ |
| `<arquivo.o>`  | Arquivo objeto de entrada (obrigatório)          |
| `-r <endbase>` | Endereço base decimal para realocação (opcional) |

**Exemplos:**

```bash
# Ligação absoluta (sem realocação)
./linker modulo1.o modulo2.o

# Ligação com realocação a partir do endereço 0x1000 (4096)
./linker programa.o -r 4096
```

---

### 4. Simulador Z80

Executa o arquivo `.out` gerado pelo ligador (ou do montador) com interface gráfica. A realocação pode ser feita diretamente no carregador, sem necessidade de passar pelo ligador.

```bash
./z80-simulador <arquivo.out> [-r <endbase>]
```

| Argumento       | Descrição                                                          |
| --------------- | ------------------------------------------------------------------ |
| `<arquivo.out>` | Arquivo executável de entrada (obrigatório)                        |
| `-r <endbase>`  | Endereço base decimal para realocação em tempo de carga (opcional) |

**Exemplos:**

```bash
# Execução com endereço base já definido pelo ligador
./z80-simulador programa.out

# Execução com realocação feita pelo próprio carregador
./z80-simulador programa.out -r 4096
```

> **Nota:** Quando `-r` é fornecido, o simulador atua como **carregador relativo** e aplica o mapa de realocação do arquivo para ajustar os endereços antes de iniciar a execução. Sem `-r`, o simulador atua como **carregador absoluto** e usa o endereço base definido pelo ligador.

---

## Fluxo Completo de Uso

```bash
# Passo 1: Expandir macros (opcional, se o programa usar macros)
./macro_processor programa.asm -o programa_exp.asm

# Passo 2: Montar
./assembler programa_exp.asm -o programa.o

# Passo 3: Ligar (gera o .out) (opcional, se o programa não usar diferentes módulos)
./linker programa.o

# Passo 4: Executar no simulador
./z80-simulador programa.out

# --- Alternativa com realocação no carregador ---
# Passo 3: Ligar sem definir endereço base
./linker programa.o

# Passo 4: Executar e realocar na hora da carga
./z80-simulador programa.out -r 4096
```

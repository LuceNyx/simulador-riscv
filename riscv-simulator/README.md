# RISC-V RV32I Simulator

Simulador educativo en C++17 para el subconjunto RV32I solicitado en HW#5 de
Arquitectura de Computadores. Carga un binario crudo (`.bin`) en la dirección
`0x00000000`, ejecuta instrucciones y permite inspeccionar PC, registros y memoria.

## Requisitos

- CMake 3.16 o superior.
- Compilador compatible con C++17 (GCC, Clang o MSVC).

## Compilar

### Linux / macOS / MinGW con Makefiles

```bash
cmake -S . -B build
cmake --build build
```

### Windows (Visual Studio)

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Ejecutable típico:
- Windows: `build/Release/riscv-simulator.exe` (Visual Studio) o `build/riscv-simulator.exe` (MinGW).
- Linux/macOS: `build/riscv-simulator`.

## Ejecutar

```bash
./build/riscv-simulator programa.bin
```

En Windows, ajusta la ruta al ejecutable y al binario.

## Comandos

| Comando | Acción |
|---|---|
| `step` | Ejecuta una instrucción |
| `run` | Ejecuta hasta terminar o alcanzar el límite |
| `run 10000` | Ejecuta como máximo 10 000 instrucciones |
| `pc` | Muestra el PC |
| `regs` | Muestra x0–x31 |
| `reg x5` | Muestra un registro |
| `mem 0x1000 0x101f` | Muestra un rango de memoria en bytes |
| `reset` | Reinicia PC y registros |
| `help` | Muestra ayuda |
| `exit` | Termina el simulador |

Las direcciones de `mem` aceptan hexadecimal (`0x...`) o decimal.

## Instrucciones implementadas

- Loads: `lb`, `lh`, `lw`, `lbu`, `lhu`
- Immediate: `addi`, `slli`, `slti`, `sltiu`, `xori`, `srli`, `srai`, `ori`, `andi`
- Upper immediate: `auipc`, `lui`
- Stores: `sb`, `sh`, `sw`
- Register-register: `add`, `sub`, `sll`, `slt`, `sltu`, `xor`, `srl`, `sra`, `or`, `and`
- Branches: `beq`, `bne`, `blt`, `bge`, `bltu`, `bgeu`
- Jumps: `jal`, `jalr`
- System: `ebreak` (`0x00100073`), which halts the simulator with the PC left on
  the breakpoint instruction.

## Decisiones y límites de esta versión

- La memoria simulada es plana, byte-addressable y little-endian, con 16 MiB
  asignados por defecto. El espacio arquitectural y las direcciones son de 32 bits,
  pero los accesos deben estar dentro de la memoria físicamente asignada.
- Las instrucciones RV32I base tienen 32 bits y el PC avanza 4 bytes en el flujo
  secuencial.
- `x0` siempre lee cero y descarta escrituras.
- El fin del archivo cargado se utiliza como condición de fin cuando el PC sale
  del rango del binario. Para programas con loops, `run` también tiene un límite
  de seguridad configurable.
- Se reportan instrucciones inválidas y accesos fuera de memoria.
- No se implementan ELF, GDB, `ecall`, otras instrucciones SYSTEM, extensiones M/C ni interrupciones; el
  enunciado los presenta como ideas adicionales o no los exige.

## Pruebas automatizadas

Las pruebas compilan un ejecutable C++ pequeño que genera los programas binarios
en el directorio temporal del sistema. Comprueban que EBREAK detenga el simulador,
que el PC quede en la instrucción y que ECALL siga rechazándose.

```bash
cmake -S . -B build-tests
cmake --build build-tests --config Release
ctest --test-dir build-tests --build-config Release --output-on-failure
```

En generadores de una sola configuración, `--config Release` y
`--build-config Release` se pueden omitir.

## Pruebas manuales sugeridas por el enunciado

También se puede probar con los binarios crudos de:
1. `riscvtest.s`
2. Quicksort
3. Árbol simétrico

En CPUlator: compilar y cargar, abrir Memory, elegir formato `Raw`, guardar
desde `0000` hasta una dirección que cubra el programa. Cargar ese `.bin` aquí.

## Estructura

```text
include/       Interfaces de memoria, decoder y simulador
src/           Implementación y CLI
tests/         Pruebas automatizadas, fuentes y binarios de prueba opcionales
```

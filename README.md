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
| `step [n]` | Ejecuta una instrucción o `n` instrucciones |
| `run` | Ejecuta hasta terminar o alcanzar el límite |
| `run 10000` | Ejecuta como máximo 10 000 instrucciones |
| `pc` | Muestra el PC |
| `regs` | Muestra x0–x31 |
| `regs a0 sp` o `reg x5` | Muestra registros por número o alias ABI |
| `mem 0x1000 0x101f` | Muestra un rango de memoria en bytes |
| `memw 0x64 [n]` | Muestra `n` palabras de 32 bits en hexadecimal y decimal |
| `break 0x20`, `delete 0x20`, `breaks` | Gestiona y lista breakpoints para `run` |
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

Las pruebas compilan un ejecutable C++ que genera programas pequeños en el
directorio temporal y ejecuta los binarios versionados de `programas/` y
`pruebas/`. Verifican EBREAK y su PC, instrucciones inválidas, los ejemplos y
`pruebas/full-test.bin`, que comprueba el conjunto RV32I y termina con `a0 = 1`.

```bash
cmake -S . -B build-tests
cmake --build build-tests --config Release
ctest --test-dir build-tests --build-config Release --output-on-failure
```

En generadores de una sola configuración, `--config Release` y
`--build-config Release` se pueden omitir.

## Comprobar los ejemplos desde la consola

Los casos de `programas/` incluyen fuentes `.s` y binarios crudos:

- `riscvtest`: verifica las instrucciones del ejemplo y deja 25 en `0x64`.
- `symmetric-tree`: evalúa el árbol del enunciado y deja `a0 = 1`.
- `quicksort`: ordena el arreglo del enunciado en `0x1000`.
- `pruebas/full-test.bin`: comprueba el conjunto RV32I; `a0 = 1` indica que pasó.

Ejecuta cada binario desde la raíz del proyecto:

```powershell
.\build-tests\Release\riscv-simulator.exe .\programas\riscvtest.bin
```

Dentro de la consola, comprueba `riscvtest` con `run` y `memw 100`; el resultado
debe ser `0x00000019 (25)`. Para el árbol, usa `run` y `reg a0`; debe mostrar
`0x00000001`. Para QuickSort, usa `run` y `memw 0x1000 7`; deben aparecer
`1, 2, 3, 4, 6, 8, 9`. La ejecución termina al encontrar EBREAK.

También puedes probar `step 3`, `break 0x20`, `breaks` y `delete 0x20`. `run`
se detiene al llegar a un breakpoint. Los binarios `.elf` y `ecall` no forman
parte del simulador de esta entrega.

## Estructura

```text
include/       Interfaces de memoria, decoder y simulador
src/           Implementación y CLI
programas/     Fuentes y binarios de los ejemplos del enunciado
pruebas/       Pruebas automatizadas y pasos de verificación
informe/       Informe y guion del video
```

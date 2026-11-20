# Pruebas del simulador

## Pruebas automatizadas

Desde la raíz del repositorio, compila y ejecuta CTest:

```bash
cmake -S . -B build-tests
cmake --build build-tests --config Release
ctest --test-dir build-tests --build-config Release --output-on-failure
```

En generadores de una sola configuración, se pueden omitir las opciones
`--config` y `--build-config`.

El caso `simulator-tests` genera programas pequeños en el directorio temporal y
ejecuta los ejemplos de `programas/` y la prueba `full-test.bin`. No requiere un
ensamblador RISC-V. Verifica:

- EBREAK (`0x00100073`) detiene la ejecución y deja el PC en su dirección.
- El PC avanza normalmente antes de EBREAK.
- ECALL y otras instrucciones SYSTEM no implementadas continúan generando error.
- La instrucción cero se rechaza sin avanzar el PC.
- `riscvtest` guarda 25 en `0x64`.
- `symmetric-tree` devuelve 1 en `x10` (`a0`).
- `quicksort` ordena `{6, 4, 3, 2, 1, 8, 9}` como `{1, 2, 3, 4, 6, 8, 9}`.
- `full-test.bin` comprueba el conjunto RV32I y devuelve `a0 = 1`.

## Pruebas manuales

Para ejecutar un binario, usa la carpeta `programas`:

```powershell
.\build-tests\Release\riscv-simulator.exe .\programas\programa.bin
```

En Linux o macOS:

```bash
./build-tests/riscv-simulator ./programas/programa.bin
```

Usa `step`, `pc`, `regs` y `mem INICIO FIN` para inspeccionar la ejecución.
Los binarios de ejemplo están permitidos en `programas/` por `.gitignore`.

## Casos del enunciado

Los binarios crudos incluidos se cargan desde 0x00000000:

| Archivo | Comprobación |
|---|---|
| programas/riscvtest.bin | Ejecuta add, sub, and, or, slt, addi, lw, sw, beq y jal; termina con 25 en 0x64. |
| programas/symmetric-tree.bin | Carga {6, 4, 4, 1, 2, 2, 1} en 0x1000; a0 (x10) termina en 1. |
| programas/quicksort.bin | Ordena {6, 4, 3, 2, 1, 8, 9} en 0x1000. |
| pruebas/full-test.bin | Comprueba el conjunto RV32I; `a0 = 1` indica que pasó. |

Las fuentes `.s` correspondientes están en `programas/`. Estos casos se
ejecutan automáticamente en CTest. Para repetirlos manualmente, abre cada
binario, escribe `run` y consulta `memw 100`, `reg a0` o `memw 0x1000 7`.

## Comandos adaptados del proyecto de referencia

El CLI permite `step [n]`, alias ABI como `a0` y `sp`, lectura de palabras con
`memw` y breakpoints (`break`, `delete`, `breaks`). No se incluyeron los casos
de ELF ni ECALL porque esas funciones no están implementadas en este proyecto.

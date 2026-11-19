# Pruebas del simulador

## Pruebas automatizadas

Desde la carpeta `riscv-simulator`, compila y ejecuta CTest:

```bash
cmake -S . -B build-tests
cmake --build build-tests --config Release
ctest --test-dir build-tests --build-config Release --output-on-failure
```

En generadores de una sola configuración, se pueden omitir las opciones
`--config` y `--build-config`.

El caso `simulator-tests` crea sus binarios de prueba en el directorio temporal
del sistema, así que no requiere un ensamblador RISC-V ni archivos `.bin`
precompilados. Verifica:

- EBREAK (`0x00100073`) detiene la ejecución y deja el PC en su dirección.
- El PC avanza normalmente antes de EBREAK.
- ECALL y otras instrucciones SYSTEM no implementadas continúan generando error.

## Pruebas manuales

Para ejecutar un binario propio, guárdalo aquí con extensión `.bin` y arranca el
simulador desde `riscv-simulator`:

```powershell
.\build-tests\Release\riscv-simulator.exe .\tests\programa.bin
```

En Linux o macOS:

```bash
./build-tests/riscv-simulator ./tests/programa.bin
```

Usa `step`, `pc`, `regs` y `mem INICIO FIN` para inspeccionar la ejecución.
Los binarios de prueba ubicados directamente en esta carpeta están permitidos
por `.gitignore`; los binarios de compilación fuera de `tests/` siguen ignorados.

El enunciado también sugiere probar `riscvtest.s`, Quicksort y árbol simétrico.
Para exportarlos desde CPUlator, selecciona formato `Raw` y dirección inicial
`0000`. Quicksort tiene instrucciones en `quicksort.s` y sus resultados esperados
se describen en `README.txt`.

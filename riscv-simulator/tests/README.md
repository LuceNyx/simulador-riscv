# Pruebas

Coloca aquí los binarios crudos exportados desde CPUlator (`Raw`, Start address
`0000`). El enunciado recomienda `riscvtest.s`, Quicksort y árbol simétrico.

Ejemplo de comprobación manual:
1. Ejecutar el simulador con el `.bin`.
2. Usar `step` para avanzar una instrucción.
3. Revisar `pc` y `regs`.
4. Revisar la memoria con `mem 0x1000 0x101f` o `mem 0x64 0x67`.

No se incluyen binarios compilados porque deben generarse/exportarse desde los
programas de prueba originales.

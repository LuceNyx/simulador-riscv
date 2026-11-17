PRUEBA QUICKSORT RECURSIVO - RV32I

El binario inicializa {6,4,3,2,1,8,9} en 0x1000 y ejecuta quicksort
recursivo (particion de Lomuto). El resultado esperado es {1,2,3,4,6,8,9}.

Copia quicksort.bin en la carpeta tests del simulador y ejecuta:

  .\build\riscv-simulator.exe .\tests\quicksort.bin

Dentro del simulador:

  run
  mem 0x1000 0x101B
  regs
  pc

Valores esperados, interpretando cada grupo de 4 bytes como little-endian:
  0x1000 = 1
  0x1004 = 2
  0x1008 = 3
  0x100C = 4
  0x1010 = 6
  0x1014 = 8
  0x1018 = 9

El programa termina en un bucle infinito intencional para mantener el PC
en una direccion valida después de completar el ordenamiento.

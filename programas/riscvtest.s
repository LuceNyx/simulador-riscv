# Prueba corta de instrucciones RV32I.
# Guarda el resultado 25 en la dirección 0x64.

    addi x1, x0, 20
    addi x2, x0, 5
    add  x3, x1, x2
    sub  x4, x1, x2
    and  x5, x1, x3
    or   x6, x1, x2
    slt  x7, x2, x1
    sw   x3, 100(x0)
    lw   x8, 100(x0)
    beq  x8, x3, success
    addi x9, x0, -1
success:
    jal  x0, done
    addi x9, x0, -2
done:
    ebreak

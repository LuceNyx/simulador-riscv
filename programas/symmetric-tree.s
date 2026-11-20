# Arbol completo aplanado: {6, 4, 4, 1, 2, 2, 1}.
# La funcion is_symmetric compara los nodos espejo y devuelve el resultado en a0.

    lui  x11, 1
    addi x2, x0, 6
    sw   x2, 0(x11)
    addi x2, x0, 4
    sw   x2, 4(x11)
    sw   x2, 8(x11)
    addi x2, x0, 1
    sw   x2, 12(x11)
    addi x2, x0, 2
    sw   x2, 16(x11)
    sw   x2, 20(x11)
    addi x2, x0, 1
    sw   x2, 24(x11)

    jal  x1, is_symmetric
    ebreak

is_symmetric:
    lw   x3, 4(x11)
    lw   x4, 8(x11)
    bne  x3, x4, fail
    lw   x3, 12(x11)
    lw   x4, 24(x11)
    bne  x3, x4, fail
    lw   x3, 16(x11)
    lw   x4, 20(x11)
    bne  x3, x4, fail
    addi x10, x0, 1
    jalr x0, 0(x1)
fail:
    addi x10, x0, 0
    jalr x0, 0(x1)

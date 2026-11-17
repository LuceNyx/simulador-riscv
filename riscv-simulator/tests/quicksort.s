# Quicksort recursivo (Lomuto) - RV32I
    # Array inicial: {6,4,3,2,1,8,9} en 0x1000
    # Resultado esperado: {1,2,3,4,6,8,9} en 0x1000
    # quicksort(a0=base, a1=low, a2=high)

    lui   t0, 0x1          # t0 = 0x1000
    addi  t1, zero, 6
    sw    t1, 0(t0)
    addi  t1, zero, 4
    sw    t1, 4(t0)
    addi  t1, zero, 3
    sw    t1, 8(t0)
    addi  t1, zero, 2
    sw    t1, 12(t0)
    addi  t1, zero, 1
    sw    t1, 16(t0)
    addi  t1, zero, 8
    sw    t1, 20(t0)
    addi  t1, zero, 9
    sw    t1, 24(t0)

    lui   a0, 0x1          # base = 0x1000
    addi  a1, zero, 0      # low = 0
    addi  a2, zero, 6      # high = 6
    lui   sp, 0x800          # sp = 0x00800000 (pila dentro de la memoria)
    jal   ra, quicksort

done:
    ebreak                 # detener la ejecucion del simulador

quicksort:
    # Prologo: guardar ra y s0-s3
    addi  sp, sp, -20
    sw    ra, 16(sp)
    sw    s0, 12(sp)
    sw    s1, 8(sp)
    sw    s2, 4(sp)
    sw    s3, 0(sp)

    addi  s0, a0, 0        # s0 = base
    addi  s1, a1, 0        # s1 = low
    addi  s2, a2, 0        # s2 = high

    bge   s1, s2, q_return # if low >= high, return

    # pivot = array[high]
    slli  t0, s2, 2
    add   t0, s0, t0
    lw    s3, 0(t0)

    # i = low - 1; j = low
    addi  t1, s1, -1       # i
    addi  t2, s1, 0        # j

partition_loop:
    bge   t2, s2, partition_done
    slli  t3, t2, 2
    add   t3, s0, t3
    lw    t4, 0(t3)        # arr[j]
    blt   s3, t4, skip_swap # if pivot < arr[j], skip
    addi  t1, t1, 1        # i++
    slli  t5, t1, 2
    add   t5, s0, t5       # &arr[i]
    lw    t6, 0(t5)        # temp = arr[i]
    sw    t4, 0(t5)        # arr[i] = arr[j]
    sw    t6, 0(t3)        # arr[j] = temp

skip_swap:
    addi  t2, t2, 1
    jal   zero, partition_loop

partition_done:
    # Colocar pivot en su posicion final: i++
    addi  t1, t1, 1
    slli  t5, t1, 2
    add   t5, s0, t5
    lw    t6, 0(t5)
    slli  t3, s2, 2
    add   t3, s0, t3
    lw    t4, 0(t3)
    sw    t4, 0(t5)
    sw    t6, 0(t3)
    addi  s3, t1, 0       # s3 = pivot index

    # quicksort(base, low, pivot_index - 1)
    addi  a0, s0, 0
    addi  a1, s1, 0
    addi  a2, s3, -1
    jal   ra, quicksort

    # quicksort(base, pivot_index + 1, high)
    addi  a0, s0, 0
    addi  a1, s3, 1
    addi  a2, s2, 0
    jal   ra, quicksort

q_return:
    lw    s3, 0(sp)
    lw    s2, 4(sp)
    lw    s1, 8(sp)
    lw    s0, 12(sp)
    lw    ra, 16(sp)
    addi  sp, sp, 20
    jalr  zero, 0(ra)

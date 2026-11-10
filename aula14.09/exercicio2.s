// (120 - 70 - 10) * (7 + 26)
.global _main

.text
.align 2

_main:

    mov x0, #120
    mov x1, #70
    mov x2, #10
    mov x3, #7
    mov x4, 26

    add x3, x3, x4
    sub x0, x0, x1
    sub x0, x0, x2

    mul x0, x0, x3
   

   bl imprime_num
   b sair

.include "runtime.s"

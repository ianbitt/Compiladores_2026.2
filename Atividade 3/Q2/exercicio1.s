// (21 * 13 - 11 * 7) + (117 - 33)
// Para rodar esse código, no diretório tem que ter runtime.s
//  e criar um executável com clang -o nomedoarquivo nomedoarquivo.s
//  em seguida, mesmo diretório digitar ./nomedoarquivo

.global _main

.text
.align 2

_main:

    mov x0, #21
    mov x1, #13
    mov x2, #11
    mov x3, #7
    mov x4, #117
    mov x5, #33

    mul x0,x0,x1
    mul x2,x2,x3
    sub x0,x0,x2

    sub x4,x4,x5
    add x0,x0,x4

   

   bl imprime_num
   b sair

.include "runtime.s"

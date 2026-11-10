// (9 * 8 * 7 * 6)/ (5 * 4 * 3 * 2)
// Para rodar esse código, no diretório tem que ter runtime.s
//  e criar um executável com clang -o nomedoarquivo nomedoarquivo.s
//  em seguida, mesmo diretório digitar ./nomedoarquivo

.global _main

.text
.align 2

_main:

    mov x0, #9
    mov x1, #8
    mov x2, #7
    mov x3, #6
    mov x4, #5
    mov x5, #4
    mov x6, #3
    mov x7, #2

    mul x0,x0,x1
    mul x0,x0,x2
    mul x0,x0,x3

    mul x4,x4,x5
    mul x4,x4,x6
    mul x4,x4,x7

    udiv x0,x0,x4

   bl imprime_num
   b sair

.include "runtime.s"

// (59 - 212) * 17 + (19 * 88)
// Para rodar esse código, no diretório tem que ter runtime.s
//  e criar um executável com clang -o nomedoarquivo nomedoarquivo.s
//  em seguida, mesmo diretório digitar ./nomedoarquivo

.global _main

.text
.align 2

_main:

    mov x0, #59
    mov x1, #17
    mov x2, #19
    mov x3, #88
    
    sub x0,x0,#212
    mul x0,x0,x1
    mul x2,x2,x3
    add x0,x0,x2

   bl imprime_num
   b sair

.include "runtime.s"

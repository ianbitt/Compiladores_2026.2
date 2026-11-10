// 112 * 7 - (423 / 3)
// Para rodar esse código, no diretório tem que ter runtime.s
//  e criar um executável com clang -o nomedoarquivo nomedoarquivo.s
//  em seguida, mesmo diretório digitar ./nomedoarquivo
// sdiv x3, x1, x2        ; x3 = quociente
// msub x4, x3, x2, x1    ; x4 = x1 - (x3 * x2)  -> resto

.global _main

.text
.align 2

_main:

    mov x0, #112
    mov x1, #7
    mov x2, #423
    mov x3, #3

    mul x0, x0, x1
    sdiv x2, x2, x3

    sub x0, x0, x2
   

   bl imprime_num
   b sair

.include "runtime.s"

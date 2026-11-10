// (7 + (12 + 15))
// Para rodar esse código, no diretório tem que ter runtime.s
//  e criar um executável com clang -o nomedoarquivo nomedoarquivo.s
//  em seguida, mesmo diretório digitar ./nomedoarquivo
// sdiv x3, x1, x2        ; x3 = quociente
// msub x4, x3, x2, x1    ; x4 = x1 - (x3 * x2)  -> resto

.global _main

.text
.align 2

_main:

    mov x0, #7
    str x0, [sp, #-16]!
    mov x0, #12
    str x0, [sp, #-16]!
    mov x0, #15
    ldr x1, [sp], #16
    add x0, x0, x1
    ldr x1, [sp], #16
    add x0, x0, x1
   

   bl imprime_num
   b sair

.include "runtime.s"

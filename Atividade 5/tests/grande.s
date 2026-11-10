//
// saida gerada pelo compilador EC1
// versao arm64 + macOS
//
.global _main

.text
.align 2

_main:

    ldr x0, =100000
    str x0, [sp, #-16]!
    mov x0, #1
    ldr x1, [sp], #16
    add x0, x0, x1

    bl imprime_num
    b sair

.include "runtime.s"

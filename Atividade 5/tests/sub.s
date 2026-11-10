//
// saida gerada pelo compilador EC1
// versao arm64 + macOS
//
.global _main

.text
.align 2

_main:

    mov x0, #11
    str x0, [sp, #-16]!
    mov x0, #7
    ldr x1, [sp], #16
    sub x0, x1, x0

    bl imprime_num
    b sair

.include "runtime.s"

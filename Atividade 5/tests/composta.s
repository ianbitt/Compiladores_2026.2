//
// saida gerada pelo compilador EC1
// versao arm64 + macOS
//
.global _main

.text
.align 2

_main:

    mov x0, #427
    str x0, [sp, #-16]!
    mov x0, #7
    ldr x1, [sp], #16
    sdiv x0, x1, x0
    str x0, [sp, #-16]!
    mov x0, #11
    str x0, [sp, #-16]!
    mov x0, #231
    str x0, [sp, #-16]!
    mov x0, #5
    ldr x1, [sp], #16
    add x0, x0, x1
    ldr x1, [sp], #16
    mul x0, x0, x1
    ldr x1, [sp], #16
    add x0, x0, x1

    bl imprime_num
    b sair

.include "runtime.s"

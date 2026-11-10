//
// saida gerada pelo compilador EC1
// versao arm64 + macOS
//
.global _main

.text
.align 2

_main:

    mov x0, #333

    bl imprime_num
    b sair

.include "runtime.s"

//
// saida gerada pelo compilador CI
// versao arm64 + macOS
//
.global _main
.text
.align 2
_main:
    mov x0, #45612
    bl imprime_num
    b sair
.include "runtime.s"

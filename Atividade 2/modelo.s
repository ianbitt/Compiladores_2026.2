//
// modelo de saida para o compilador
// versao arm64 + macOS
//

.global _main

.text
.align 2

_main:
   // saida do compilador deve ser inserida aqui

   bl imprime_num
   b sair

.include "runtime.s"

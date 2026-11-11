.section .bss
    .lcomm largura1, 8
    .lcomm altura2, 8

.section .text
.globl _start
_start:
    movq $10, %rax
    movq %rax, largura1(%rip)
    movq largura1(%rip), %rax
    pushq %rax
    movq $2, %rax
    popq %rbx
    imulq %rbx, %rax
    movq %rax, altura2(%rip)
    movq altura2(%rip), %rax
    call imprime_num
    call sair

.include "runtime.s"

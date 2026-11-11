.section .bss

.section .text
.globl _start
_start:
    movq $7, %rax
    pushq %rax
    movq $5, %rax
    popq %rbx
    addq %rbx, %rax
    pushq %rax
    movq $3, %rax
    popq %rbx
    imulq %rbx, %rax
    call imprime_num
    call sair

.include "runtime.s"

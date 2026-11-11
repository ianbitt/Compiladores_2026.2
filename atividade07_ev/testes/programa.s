.section .bss
    .lcomm x, 8
    .lcomm y, 8

.section .text
.globl _start
_start:
    movq $30, %rax
    movq %rax, x(%rip)
    movq $40, %rax
    movq %rax, y(%rip)
    movq x(%rip), %rax
    pushq %rax
    movq x(%rip), %rax
    popq %rbx
    addq %rbx, %rax
    pushq %rax
    movq y(%rip), %rax
    popq %rbx
    addq %rbx, %rax
    pushq %rax
    movq y(%rip), %rax
    popq %rbx
    addq %rbx, %rax
    call imprime_num
    call sair

.include "runtime.s"

//
// funcoes de apoio para o codigo compilado
//

//
// imprime na tela (stdout) o conteudo do registrador x0
// como um inteiro de 64 bits com sinal, em base decimal
//
imprime_num:
        // x1 aponta para o buffer
        // x2 conta o numero de caracteres
        // x3 armazena caracteres para copiar p/ o buffer
        // x10 guarda a base 10
        // x11 guarda o flag se o numero era negativo
        mov x10, #10
        mov x2, #0
        // inicializa x1 para o final do buffer
        adrp x1, buffer@PAGE
        add x1, x1, buffer@PAGEOFF
        add x1, x1, #25
        mov x3, #'\n'
        strb w3, [x1], -1  // coloca \n no final
        add x2, x2, #1
        cmp x0, #0
        beq valzero        // o numero eh 0, pula loop
        blt markneg
        mov x11, #0
        b loop

markneg:
        mov x11, #1
        neg x0, x0

loop:
        udiv x4, x0, x10
        msub x5, x4, x10, x0
        add x3, x5, #0x30
        strb w3, [x1], -1
        add x2, x2, #1
        mov x0, x4
        cmp x0, #0
        bne loop
        // numero convertido, verifica sinal
        cmp x11, #0
        beq print
        mov x3, #'-'
        strb w3, [x1], -1
        add x2, x2, #1
        b print

valzero:
        mov x3, #0x30  // '0'
        strb w3, [x1], -1
        add x2, x2, #1

print:
        mov x0, #1
        add x1, x1, #1
        mov x16, #4
        svc #0x80
        ret

sair:
        mov x0, #0
        mov x16, #1
        svc #0x80

.bss
        .lcomm buffer, 26
        

# Atividade 07 — Compilador EV

## Gramática

```text
<programa> ::= <decl>* '=' <exp>
<decl>     ::= <ident> '=' <exp> ';'
<exp>      ::= <exp_m> (('+' | '-') <exp_m>)*
<exp_m>    ::= <prim> (('*' | '/') <prim>)*
<prim>     ::= <num> | <ident> | '(' <exp> ')'
<ident>    ::= <letra><letra_digito>*
<num>      ::= <digito><digito>*
```

O compilador faz análise léxica, sintática, semântica e geração de assembly. A tabela de símbolos registra nomes declarados. Cada expressão de declaração é verificada antes de inserir o nome da variável, respeitando a ordem das declarações. Nomes duplicados também são rejeitados.

## Compilar e executar

```bash
make
./ev testes/programa.ev
```

O programa imprime tokens e AST e gera `testes/programa.s`.

## Testes

```bash
make testes
```

Inclui programas válidos, programa sem declarações, identificadores com dígitos e erros léxicos/semânticos.

## Alvo e runtime

O assembly gerado usa GNU assembler, sintaxe AT&T, x86-64/Linux, `_start`, `.section .bss`, `.lcomm`, e chamadas `imprime_num` e `sair`, conforme o modelo do enunciado. O arquivo `runtime.s` compatível deve estar no diretório de trabalho ao montar/ligar. O PDF fornecido não especifica aqui o comando exato de linkedição nem inclui o runtime.

## Grupo e uso de LLM

Grupo: 
- João Vittor de araújo Alves
- Ian Rocha Bittencourt

Uso de LLM: Utilizamos para nos ajudar a criar os testes, o makefile e em duvidas que tinhamos ao longo da atividade.
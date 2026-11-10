# Compilador EC1 — Atividade 05 (geração de código)

## Aluno
- Ian Rocha Bittencourt — 20230012850
- Joao Vittor de Araujo Alves - 20230145919

## Como compilar
Requer `clang` (Xcode Command Line Tools).

    make

Isso gera o executável `ec1`.

## Como executar
    ./ec1 <arquivo.ec1>

Gera `tests/arquivo.s`, um arquivo assembly ARM64 completo (segue o modelo
da Atividade 02) pronto para ser montado e linkado:

    clang -o tests/arquivo tests/arquivo.s
    ./tests/arquivo

Erros léxicos ou sintáticos são reportados na saída de erro e nenhum
arquivo `.s` é gerado.

## Como executar os testes
    make test

Roda `tests/run_tests.sh`, que para cada programa válido compila, monta,
linka e executa o binário, comparando o número impresso com o valor
esperado (inclui soma, subtração, expressão aninhada, resultado negativo e
constante maior que 16 bits). Também testa um programa com erro léxico e
outro com erro sintático, conferindo que o compilador rejeita ambos.

## Uso de LLMs
Claude usado como tutor e revisor: deu explicações sobre o esquema de
geração de código usando a pilha (empilhar/desempilhar com `str`/`ldr`
pré/pós-indexados) e revisão do código escrito. Além disso, ajudou a gerar os arquivos de teste solicitados e o makefile.

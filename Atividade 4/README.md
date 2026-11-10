# Compilador EC1 — Atividade 04

## Aluno
- Ian Rocha Bittencourt — 20230012850

## Como compilar
Requer `clang` (Xcode Command Line Tools).

    make

Isso gera o executável `ec1`.

## Como executar
    ./ec1 <arquivo.ec1>

A saída mostra, em sequência: a lista de tokens da análise léxica, a árvore
sintática (impressa no formato `(esq op dir)`) e o valor da expressão
avaliado pelo interpretador.

## Como executar os testes
    make test

Roda `tests/run_tests.sh`, que testa dois programas válidos
(`tests/valido1.ec1`, `tests/valido2.ec1`) e dois programas com erro
(`tests/erro_lexico.ec1`, com caractere inválido; `tests/erro_sintatico.ec1`,
com parêntese faltando), conferindo saída e código de retorno de cada um.

## Uso de LLMs
Claude usado como tutor e revisor. Pedi explicações sobre análise léxica e
sintática descendente recursiva em C e revisão do código escrito.

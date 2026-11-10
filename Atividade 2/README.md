# Compilador CI — Atividade 02

## Aluno
- Ian Rocha Bittencourt — 20230012850
(adicione os demais integrantes, se houver)

## Como compilar o compilador
Requer `clang` (Xcode Command Line Tools).

    make

Isso gera o executável `compci`.

## Como executar o compilador
    ./compci <arquivo.ci>

Exemplo:

    ./compci tests/valido.ci

Gera `tests/valido.s`. Para montar e linkar (requer `runtime.s` no mesmo diretório do `.s` gerado):

    clang -o tests/valido tests/valido.s
    ./tests/valido

## Como executar os testes
    make test

Executa o caso válido (`tests/valido.ci`) e o inválido (`tests/invalido.ci`), confirmando que o segundo falha com erro de sintaxe.

## Uso de LLMs
[Usei o Claude como tutor, pedindo explicação de cada etapa (leitura de arquivo, alocação dinâmica em C, geração do template assembly) por não ter tanto conhecimento da linguagem C na questão de manipulação de arquivos e revisão do código escrito por nós, sem que ele escrevesse a solução final sem explicação."]
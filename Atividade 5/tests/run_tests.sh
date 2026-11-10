#!/usr/bin/env bash
# Testes automaticos do compilador EC1 (Atividade 05).
# Para cada programa valido: compila para .s, monta+linka com clang,
# executa o binario e compara a saida com o valor esperado.
# Para os programas invalidos: confere que o compilador rejeita a entrada.

cd "$(dirname "$0")/.."
BIN=./ec1
pass=0
fail=0

check_valido() {
    local desc="$1" nome="$2" esperado="$3"

    if ! "$BIN" "tests/$nome.ec1" > /dev/null 2>&1; then
        echo "FAIL: $desc (compilador rejeitou um programa valido)"
        fail=$((fail + 1))
        return
    fi

    if ! clang -o "tests/out_$nome" "tests/$nome.s" 2> /dev/null; then
        echo "FAIL: $desc (falha ao montar/linkar tests/$nome.s)"
        fail=$((fail + 1))
        return
    fi

    saida=$("tests/out_$nome")
    if [[ "$saida" == "$esperado" ]]; then
        echo "PASS: $desc (resultado $saida)"
        pass=$((pass + 1))
    else
        echo "FAIL: $desc (esperado $esperado, obtido $saida)"
        fail=$((fail + 1))
    fi
}

check_invalido() {
    local desc="$1" nome="$2" esperado_substr="$3"

    out=$("$BIN" "tests/$nome.ec1" 2>&1)
    status=$?
    if [[ "$status" -ne 0 && "$out" == *"$esperado_substr"* ]]; then
        echo "PASS: $desc"
        pass=$((pass + 1))
    else
        echo "FAIL: $desc (status=$status, saida: $out)"
        fail=$((fail + 1))
    fi
}

check_valido  "constante simples (333)"                 simples   333
check_valido  "soma (7 + 11)"                            soma      18
check_valido  "soma aninhada (7 + (3 + 8))"              aninhado  18
check_valido  "subtracao (11 - 7)"                       sub       4
check_valido  "expressao composta do guia"               composta  2657
check_valido  "resultado negativo (5 - 20)"               negativo  -15
check_valido  "constante maior que 16 bits (100000 + 1)" grande    100001
check_invalido "erro lexico (caractere invalido)"        erro_lexico    "Erro lexico"
check_invalido "erro sintatico (parentese faltando)"     erro_sintatico "Erro sintatico"

echo
echo "Total: $((pass + fail)) | OK: $pass | Falhas: $fail"
[[ $fail -eq 0 ]]

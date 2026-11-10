#!/usr/bin/env bash
cd "$(dirname "$0")/.."
BIN=./ec1
pass=0
fail=0

check() {
    local desc="$1" file="$2" esperado_substr="$3" esperado_status="$4"
    out=$("$BIN" "$file" 2>&1)
    status=$?
    if [[ "$status" -eq "$esperado_status" && "$out" == *"$esperado_substr"* ]]; then
        echo "PASS: $desc"
        pass=$((pass + 1))
    else
        echo "FAIL: $desc (status=$status, esperado=$esperado_status)"
        echo "--- saida ---"
        echo "$out"
        echo "-------------"
        fail=$((fail + 1))
    fi
}

check "valido1 (constante simples)"       tests/valido1.ec1       "333"           0
check "valido2 (expressao composta)"      tests/valido2.ec1       "2657"          0
check "erro lexico (caractere invalido)"  tests/erro_lexico.ec1   "Erro lexico"   1
check "erro sintatico (parentese faltando)" tests/erro_sintatico.ec1 "Erro sintatico" 1

echo
echo "Total: $((pass + fail)) | OK: $pass | Falhas: $fail"
[[ $fail -eq 0 ]]

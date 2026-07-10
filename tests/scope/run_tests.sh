#!/bin/bash
# test_scopes.sh — Suite de testes automatizados para verificar a tabela de escopos e memória
#
# Executa o compilador em modo --symbols para cada caso de teste em tests/scope/
# e valida se as informações (Tipo, Categoria, Ponteiro, Array, Endereço, Tamanho, Escopo)
# são exatamente as esperadas.

COMPILADOR="./compilador"
TEMP_OUT="tests/scope/temp_symbols.txt"
PASSED=0
FAILED=0

# Cores para output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # Sem cor

assert_symbol() {
    local name="$1"
    local type="$2"
    local cat="$3"
    local ptr="$4"
    local arr="$5"
    local addr="$6"
    local size="$7"
    local scope="$8" # 'global' ou 'local' (valida qualquer #ID)
    
    # Procura a linha correspondente ao símbolo
    local line
    line=$(grep -E "^${name}[[:space:]]+" "$TEMP_OUT" | head -n 1)
    if [ -z "$line" ]; then
        echo -e "  [${RED}FAIL${NC}] Símbolo '${name}' não encontrado na saída."
        FAILED=$((FAILED + 1))
        return 1
    fi
    
    # Extrai os campos reais
    local actual_type=$(echo "$line" | awk '{print $2}')
    local actual_cat=$(echo "$line" | awk '{print $3}')
    local actual_ptr=$(echo "$line" | awk '{print $4}')
    local actual_arr=$(echo "$line" | awk '{print $5}')
    local actual_addr=$(echo "$line" | awk '{print $6}')
    local actual_size=$(echo "$line" | awk '{print $7}')
    local actual_scope=$(echo "$line" | awk '{print $8}')
    
    local match=1
    [ "$actual_type" = "$type" ] || match=0
    [ "$actual_cat" = "$cat" ] || match=0
    [ "$actual_ptr" = "$ptr" ] || match=0
    [ "$actual_arr" = "$arr" ] || match=0
    [ "$actual_addr" = "$addr" ] || match=0
    [ "$actual_size" = "$size" ] || match=0
    
    if [ "$scope" = "local" ]; then
        # Escopo local deve começar com '#' (ex: #1, #2)
        if [[ ! "$actual_scope" =~ ^#[0-9]+$ ]]; then
            match=0
        fi
    else
        [ "$actual_scope" = "$scope" ] || match=0
    fi
    
    if [ $match -eq 1 ]; then
        PASSED=$((PASSED + 1))
        return 0
    else
        echo -e "  [${RED}FAIL${NC}] Divergência no símbolo '${name}':"
        echo "    Esperado: Tipo=$type, Categoria=$cat, Ptr=$ptr, Array=$arr, Endereço=$addr, Tamanho=$size, Escopo=$scope"
        echo "    Obtido:   Tipo=$actual_type, Categoria=$actual_cat, Ptr=$actual_ptr, Array=$actual_arr, Endereço=$actual_addr, Tamanho=$actual_size, Escopo=$actual_scope"
        FAILED=$((FAILED + 1))
        return 1
    fi
}

run_test_file() {
    local f="$1"
    echo -e "\n=== Testando layout de escopos de: ${f} ==="
    $COMPILADOR --symbols "$f" > "$TEMP_OUT" 2>&1
    local ret=$?
    if [ $ret -ne 0 ]; then
        echo -e "  [${RED}FAIL${NC}] Compilador falhou ao processar ${f} (exit code: $ret)"
        FAILED=$((FAILED + 1))
        return 1
    fi
}

# ----------------------------------------------------
# 1. TESTE: sc01_basic_types.c
# ----------------------------------------------------
run_test_file "tests/scope/sc01_basic_types.c"
assert_symbol "g_int"    "int"    "variable" "-" "-" "0"  "4" "global"
assert_symbol "g_char"   "char"   "variable" "-" "-" "4"  "1" "global"
assert_symbol "g_float"  "float"  "variable" "-" "-" "5"  "4" "global"
assert_symbol "g_double" "double" "variable" "-" "-" "9"  "8" "global"
assert_symbol "l_int"    "int"    "variable" "-" "-" "0"  "4" "local"
assert_symbol "l_char"   "char"   "variable" "-" "-" "4"  "1" "local"
assert_symbol "l_float"  "float"  "variable" "-" "-" "5"  "4" "local"
assert_symbol "l_double" "double" "variable" "-" "-" "9"  "8" "local"

# ----------------------------------------------------
# 2. TESTE: sc02_pointer_sizes.c
# ----------------------------------------------------
run_test_file "tests/scope/sc02_pointer_sizes.c"
assert_symbol "p_int"    "int"   "variable" "1" "-" "0"  "8" "global"
assert_symbol "p_char"   "char"  "variable" "1" "-" "8"  "8" "global"
assert_symbol "p_float"  "float" "variable" "1" "-" "16" "8" "global"
assert_symbol "pp_int"   "int"   "variable" "2" "-" "24" "8" "global"
assert_symbol "ppp_int"  "int"   "variable" "3" "-" "32" "8" "global"
assert_symbol "lp"       "int"   "variable" "1" "-" "0"  "8" "local"
assert_symbol "lpp"      "int"   "variable" "2" "-" "8"  "8" "local"
assert_symbol "lpc"      "char"  "variable" "1" "-" "16" "8" "local"

# ----------------------------------------------------
# 3. TESTE: sc03_arrays.c
# ----------------------------------------------------
run_test_file "tests/scope/sc03_arrays.c"
assert_symbol "arr1" "int"    "variable" "-" "yes(1)" "0"   "40"   "global"
assert_symbol "arr2" "char"   "variable" "-" "yes(1)" "40"  "20"   "global"
assert_symbol "arr3" "float"  "variable" "-" "yes(1)" "60"  "20"   "global"
assert_symbol "arr4" "double" "variable" "-" "yes(1)" "80"  "32"   "global"
assert_symbol "mat2" "int"    "variable" "-" "yes(2)" "112" "48"   "global"
assert_symbol "mat3" "int"    "variable" "-" "yes(3)" "160" "96"   "global"
assert_symbol "la"   "int"    "variable" "-" "yes(1)" "0"   "32"   "local"
assert_symbol "lb"   "char"   "variable" "-" "yes(1)" "32"  "16"   "local"
assert_symbol "lmat" "int"    "variable" "-" "yes(2)" "48"  "40"   "local"

# ----------------------------------------------------
# 4. TESTE: sc04_struct_sizes.c
# ----------------------------------------------------
run_test_file "tests/scope/sc04_struct_sizes.c"
assert_symbol "struct:S1" "struct" "struct"   "-" "-" "0"  "4"  "global"
assert_symbol "struct:S2" "struct" "struct"   "-" "-" "0"  "8"  "global"
assert_symbol "struct:S3" "struct" "struct"   "-" "-" "0"  "9"  "global"
assert_symbol "struct:S4" "struct" "struct"   "-" "-" "0"  "13" "global"
assert_symbol "vs1"       "struct" "variable" "-" "-" "0"  "4"  "global"
assert_symbol "vs2"       "struct" "variable" "-" "-" "4"  "8"  "global"
assert_symbol "gap"       "int"    "variable" "-" "-" "12" "4"  "global"
assert_symbol "vs3"       "struct" "variable" "-" "-" "16" "9"  "global"
assert_symbol "sentinel"  "int"    "variable" "-" "-" "25" "4"  "global"

# ----------------------------------------------------
# 5. TESTE: sc05_typedef_namespace.c
# ----------------------------------------------------
run_test_file "tests/scope/sc05_typedef_namespace.c"
assert_symbol "Ponto"         "struct" "typedef"  "-" "-" "0" "0" "global"
assert_symbol "struct:Ponto"  "struct" "struct"   "-" "-" "0" "8" "global"
assert_symbol "p1"            "typedef" "variable" "-" "-" "0" "8" "global"
assert_symbol "p2"            "struct" "variable" "-" "-" "8" "8" "global"
assert_symbol "Node"          "struct" "typedef"  "-" "-" "0" "8" "global"
assert_symbol "struct:Node"   "struct" "struct"   "-" "-" "0" "8" "global"
assert_symbol "n1"            "typedef" "variable" "-" "-" "16" "8" "global"
assert_symbol "n2"            "struct" "variable" "-" "-" "24" "8" "global"

# ----------------------------------------------------
# 6. TESTE: sc06_function_params.c
# ----------------------------------------------------
run_test_file "tests/scope/sc06_function_params.c"
assert_symbol "a"    "int"    "parameter" "-" "-" "0" "4" "local"
assert_symbol "b"    "int"    "parameter" "-" "-" "4" "4" "local"
assert_symbol "x"    "float"  "parameter" "-" "-" "0" "4" "local"
assert_symbol "y"    "float"  "parameter" "-" "-" "4" "4" "local"
assert_symbol "z"    "float"  "parameter" "-" "-" "8" "4" "local"
assert_symbol "r"    "float"  "variable"  "-" "-" "0" "4" "local"
assert_symbol "src"  "char"   "parameter" "1" "-" "0" "8" "local"
assert_symbol "dst"  "char"   "parameter" "1" "-" "8" "8" "local"
assert_symbol "n"    "int"    "parameter" "-" "-" "16" "4" "local"
assert_symbol "base" "double" "parameter" "-" "-" "0" "8" "local"
assert_symbol "exp"  "int"    "parameter" "-" "-" "8" "4" "local"

# ----------------------------------------------------
# 7. TESTE: sc07_nested_scopes.c
# ----------------------------------------------------
run_test_file "tests/scope/sc07_nested_scopes.c"
assert_symbol "x"      "int" "variable" "-" "-" "0" "4" "local"
assert_symbol "y"      "int" "variable" "-" "-" "4" "4" "local"
assert_symbol "z"      "int" "variable" "-" "-" "4" "4" "local" # no escopo aninhado { int x; int z; }
assert_symbol "a"      "int" "variable" "-" "-" "0" "4" "local"
assert_symbol "b"      "int" "variable" "-" "-" "4" "4" "local"
assert_symbol "w"      "int" "variable" "-" "-" "0" "4" "local"
assert_symbol "inner1" "int" "variable" "-" "-" "0" "4" "local"
assert_symbol "inner2" "int" "variable" "-" "-" "0" "4" "local"

# ----------------------------------------------------
# 8. TESTE: sc08_global_addr_sequence.c
# ----------------------------------------------------
run_test_file "tests/scope/sc08_global_addr_sequence.c"
assert_symbol "v1" "int"    "variable" "-" "-" "0"  "4" "global"
assert_symbol "v2" "char"   "variable" "-" "-" "4"  "1" "global"
assert_symbol "v3" "float"  "variable" "-" "-" "5"  "4" "global"
assert_symbol "v4" "double" "variable" "-" "-" "9"  "8" "global"
assert_symbol "v5" "int"    "variable" "-" "-" "17" "4" "global"
assert_symbol "v6" "char"   "variable" "-" "-" "21" "1" "global"

# ----------------------------------------------------
# 9. TESTE: sc09_union_sizes.c
# ----------------------------------------------------
run_test_file "tests/scope/sc09_union_sizes.c"
assert_symbol "union:U1" "union" "union"    "-" "-" "0"  "4"  "global"
assert_symbol "union:U2" "union" "union"    "-" "-" "0"  "8"  "global"
assert_symbol "union:U3" "union" "union"    "-" "-" "0"  "4"  "global"
assert_symbol "union:U4" "union" "union"    "-" "-" "0"  "20" "global"
assert_symbol "u1"       "union" "variable" "-" "-" "0"  "4"  "global"
assert_symbol "u2"       "union" "variable" "-" "-" "4"  "8"  "global"
assert_symbol "gap"      "int"   "variable" "-" "-" "12" "4"  "global"
assert_symbol "u3"       "union" "variable" "-" "-" "16" "4"  "global"
assert_symbol "last"     "int"   "variable" "-" "-" "20" "4"  "global"

# Limpeza e Relatório
rm -f "$TEMP_OUT"

echo -e "\n========================================"
if [ $FAILED -eq 0 ]; then
    echo -e "  ${GREEN}SUCESSO:${NC} Todos os $PASSED testes de escopo passaram!"
    echo "========================================"
    exit 0
else
    echo -e "  ${RED}FALHA:${NC} $FAILED verificações falharam ($PASSED com sucesso)."
    echo "========================================"
    exit 1
fi

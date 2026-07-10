#!/bin/sh
# =============================================================================
# SCRIPT DE TESTES SEMÂNTICOS — mycompiler
#
# Executa os 7 casos de teste do analisador semântico e verifica se
# o resultado (erro/sucesso) corresponde ao esperado.
#
# Uso: ./run_tests.sh [caminho_para_compilador]
# Exemplo: ./run_tests.sh ../../compilador
#
# Saída:
#   [PASS] — teste passou (resultado esperado)
#   [FAIL] — teste falhou (resultado inesperado)
# =============================================================================

# Localizar o compilador
COMPILADOR="${1:-../../compilador}"

if [ ! -x "$COMPILADOR" ]; then
    echo "ERRO: compilador não encontrado ou não executável: $COMPILADOR"
    echo "Execute 'make' na raiz do projecto primeiro."
    exit 1
fi

# Diretório dos testes
TESTS_DIR="$(dirname "$0")"

# Contadores
total=0
passed=0
failed=0

# Função auxiliar para executar um teste
executar_teste() {
    ficheiro="$1"
    descricao="$2"
    erros_esperados="$3"   # "0" = sem erros, "1+" = com erros

    total=$((total + 1))

    # Executar o compilador e capturar saída de erro
    output=$("$COMPILADOR" "$ficheiro" 2>&1)
    exit_code=$?

    if [ "$erros_esperados" = "0" ]; then
        # Esperamos sucesso (sem erros)
        if [ $exit_code -eq 0 ]; then
            echo "[PASS] $descricao"
            passed=$((passed + 1))
        else
            echo "[FAIL] $descricao"
            echo "       Esperado: sem erros (exit 0)"
            echo "       Obtido:   exit $exit_code"
            echo "       Output:"
            echo "$output" | sed 's/^/         /'
            failed=$((failed + 1))
        fi
    else
        # Esperamos pelo menos um erro
        if [ $exit_code -ne 0 ]; then
            echo "[PASS] $descricao"
            passed=$((passed + 1))
        else
            echo "[FAIL] $descricao"
            echo "       Esperado: erro(s) semântico(s) (exit != 0)"
            echo "       Obtido:   exit 0 (sem erros reportados)"
            failed=$((failed + 1))
        fi
    fi
}

echo "============================================================"
echo " TESTES DO ANALISADOR SEMÂNTICO — mycompiler"
echo "============================================================"
echo ""

# -----------------------------------------------------------------------------
# Teste 01: Programa válido — zero erros esperados
executar_teste "$TESTS_DIR/sem01_valido.c" \
    "sem01 | Programa válido (zero erros)" \
    "0"

# -----------------------------------------------------------------------------
# Teste 02: Variável não declarada — alínea a)
executar_teste "$TESTS_DIR/sem02_var_nao_decl.c" \
    "sem02 | Variável não declarada (alínea a)" \
    "1+"

# -----------------------------------------------------------------------------
# Teste 03: Redeclaração no mesmo escopo — alínea b)
executar_teste "$TESTS_DIR/sem03_redeclaracao.c" \
    "sem03 | Redeclaração no mesmo escopo (alínea b)" \
    "1+"

# -----------------------------------------------------------------------------
# Teste 04: Incompatibilidade de tipos — alínea c)
executar_teste "$TESTS_DIR/sem04_tipos_incompat.c" \
    "sem04 | Incompatibilidade de tipos (alínea c)" \
    "1+"
# Nota: sem04 tem erros activos -> exit != 0

# -----------------------------------------------------------------------------
# Teste 05: Tipos nos argumentos de funções — alínea d)
executar_teste "$TESTS_DIR/sem05_args_funcao.c" \
    "sem05 | Argumentos de função (alínea d)" \
    "1+"

# -----------------------------------------------------------------------------
# Teste 06: Atribuição de tipo incompatível — alínea e)
executar_teste "$TESTS_DIR/sem06_atrib_incompat.c" \
    "sem06 | Atribuição incompatível (alínea e)" \
    "1+"

# -----------------------------------------------------------------------------
# Teste 07: Condições nas estruturas de controlo — alínea f)
executar_teste "$TESTS_DIR/sem07_controlo.c" \
    "sem07 | Condições de controlo (alínea f)" \
    "1+"

# -----------------------------------------------------------------------------
echo ""
echo "============================================================"
echo " Resultados: $passed/$total passaram, $failed falharam"
echo "============================================================"

[ $failed -eq 0 ]

#!/bin/bash
# ============================================================================
# COMPILAR E TESTAR ANALISADOR LÉXICO
# 
# Data: 14/04/2026
# Professor: André Filemon
# Disciplina: Compiladores - M3
# ============================================================================

echo "╔═══════════════════════════════════════════════════════════════════════════╗"
echo "║                                                                           ║"
echo "║                   COMPILADOR ANALISADOR LÉXICO                           ║"
echo "║              Instituto Superior Politécnico (ISPTEC)                      ║"
echo "║                    Disciplina: Compiladores - M3                          ║"
echo "║                                                                           ║"
echo "╚═══════════════════════════════════════════════════════════════════════════╝"
echo ""

# Detectar SO
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    CLEAR_CMD="clear || cls"
    EXE_EXT=".exe"
else
    CLEAR_CMD="clear"
    EXE_EXT=""
fi

# Cores de saída
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Função para exibir mensagens
info() {
    echo -e "${BLUE}ℹ️  $1${NC}"
}

success() {
    echo -e "${GREEN}✓ $1${NC}"
}

warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

error() {
    echo -e "${RED}❌ $1${NC}"
}

# ============================================================================
# ETAPA 1: VERIFICAR REQUISITOS
# ============================================================================

echo ""
info "ETAPA 1: Verificando requisitos do sistema..."
echo ""

# Verificar GCC
if ! command -v gcc &> /dev/null; then
    error "GCC não encontrado. Instale: sudo apt-get install build-essential"
    exit 1
fi
success "GCC encontrado: $(gcc --version | head -1)"

# ============================================================================
# ETAPA 2: COMPILAR
# ============================================================================

echo ""
info "ETAPA 2: Compilando projeto..."
echo ""

cd "$(dirname "$0")"

# Limpar arquivos antigos
rm -f analisador_lexico tabela_simbolos.txt

# Compilar
gcc \
    -Wall \
    -std=c99 \
    -I. \
    -O2 \
    main.c \
    src/lexer/lexer.c \
    src/lexer/tokens.c \
    src/lexer/keywords.c \
    -o analisador_lexico \
    2>&1

if [ $? -eq 0 ]; then
    success "Compilação concluída com sucesso!"
    ls -lh analisador_lexico
else
    error "Compilação falhou!"
    exit 2
fi

# ============================================================================
# ETAPA 3: EXECUTAR TESTES
# ============================================================================

echo ""
info "ETAPA 3: Executando análise léxica com arquivo de teste..."
echo ""

./analisador_lexico teste_entrada.c

# ============================================================================
# ETAPA 4: EXIBIR RESULTADOS
# ============================================================================

echo ""
info "ETAPA 4: Resumo do resultado..."
echo ""

if [ -f tabela_simbolos.txt ]; then
    success "Arquivo de saída criado: tabela_simbolos.txt"
    echo ""
    head -20 tabela_simbolos.txt
    echo ""
    echo "... (veja arquivo completo em tabela_simbolos.txt)"
else
    error "Arquivo de saída não foi criado!"
    exit 3
fi

# ============================================================================
# RESUMO FINAL
# ============================================================================

echo ""
echo "╔═══════════════════════════════════════════════════════════════════════════╗"
echo "║                                                                           ║"
echo "║                      ✓ COMPILAÇÃO E TESTE BEM-SUCEDIDOS!                 ║"
echo "║                                                                           ║"
echo "║  Executável criado:        ./analisador_lexico                            ║"
echo "║  Tabela de símbolos:       tabela_simbolos.txt                            ║"
echo "║                                                                           ║"
echo "║  PRÓXIMOS PASSOS:                                                         ║"
echo "║  1. Testar com seus próprios arquivos .c                                  ║"
echo "║  2. Consultar manuais: mu.md (Usuário) e mp.md (Programador)             ║"
echo "║  3. Enviar para: compiladores2023@gmail.com                               ║"
echo "║                                                                           ║"
echo "║  Data Limite: 14/04/2026 às 23:50                                         ║"
echo "║  Defesa: 16/04/2026                                                       ║"
echo "║                                                                           ║"
echo "╚═══════════════════════════════════════════════════════════════════════════╝"
echo ""

success "Programa finalizado!"

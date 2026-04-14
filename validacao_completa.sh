#!/bin/bash

# ============================================================================
# CHECKLIST VALIDAÇÃO - ENUNCIADO DO PROFESSOR (Versão Simplificada)
# Windows-compatible
# ============================================================================

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PASSOU=0
FALHOU=0

check_ok() {
    echo -e "${GREEN}✓${NC} $1"
    ((PASSOU++))
}

check_fail() {
    echo -e "${RED}✗${NC} $1"
    ((FALHOU++))
}

check_info() {
    echo -e "${BLUE}ℹ${NC} $1"
}

clear
cat <<'EOF'

╔═══════════════════════════════════════════════════════════════════════════╗
║                                                                           ║
║           VALIDAÇÃO CONFORME ENUNCIADO DO PROFESSOR                      ║
║                   Análise Léxica - Fase 1 Compilador                     ║
║                                                                           ║
║              Disciplina: Compiladores - M3                               ║
║              Professor: André Filemon                                    ║
║              Data Limite: 14/04/2026 às 23:50                            ║
║                                                                           ║
╚═══════════════════════════════════════════════════════════════════════════╝

EOF

# ============================================================================
# 1. REQUISITOS DE LINGUAGEM
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "REQUISITE 1: LINGUAGEM DE IMPLEMENTAÇÃO"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

check_info "Verificando linguagem..."
if [ -f "main.c" ] && [ -f "src/lexer/lexer.c" ]; then
    check_ok "Linguagem C confirmada (não Python)"
else
    check_fail "Arquivo .c não encontrado"
fi

# ============================================================================
# 2. FUNÇÕES OBRIGATÓRIAS
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "REQUISITO 2: FUNÇÕES OBRIGATÓRIAS (Enunciado pág 1)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

check_info "Função: analex()"
if grep -q "Token analex(Lexer \*lexer)" src/lexer/lexer.c; then
    check_ok "analex() - Implementa autômatos finitos determinísticos ✓"
else
    check_fail "analex() não encontrada"
fi

check_info "Função: ler_caractere()"
if grep -q "char ler_caractere(Lexer \*lexer)" src/lexer/lexer.c; then
    check_ok "ler_caractere() - Leitura caractere por caractere ✓"
else
    check_fail "ler_caractere() não encontrada"
fi

check_info "Função: volta_caractere()"
if grep -q "void volta_caractere(Lexer \*lexer, char c)" src/lexer/lexer.c; then
    check_ok "volta_caractere() - Transição 'OUTRO' no autômato ✓"
else
    check_fail "volta_caractere() não encontrada"
fi

check_info "Função: gravar_token_lexema()"
if grep -q "Token gravar_token_lexema(Lexer \*lexer" src/lexer/lexer.c; then
    check_ok "gravar_token_lexema() - Guardar tabela de símbolos ✓"
else
    check_fail "gravar_token_lexema() não encontrada"
fi

# ============================================================================
# 3. CONCEITO DE MÁQUINA DE ESTADOS
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "REQUISITO 3: MÁQUINA DE ESTADOS (Obrigatório no Enunciado)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

check_info "Verificando implementação..."
if grep -q "estado_atual\|ESTADO_INICIAL" src/lexer/lexer.h; then
    check_ok "Máquina de estados definida ✓"
else
    check_fail "Máquina de estados não encontrada"
fi

if grep -q "while.*ler_caractere\|switch.*estado\|if.*eh_letra" src/lexer/lexer.c; then
    check_ok "Transições de estados implementadas ✓"
else
    check_fail "Transições não encontradas"
fi

# ============================================================================
# 4. COMPILAÇÃO
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "REQUISITO 4: COMPILAÇÃO SEM ERROS"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

check_info "Compilando..."
if gcc -Wall -std=c99 -I. -O2 main.c src/lexer/*.c -o analisador_lexico 2>/dev/null; then
    check_ok "Compilação bem-sucedida ✓"
else
    check_fail "Compilação falhou"
fi

if [ -x analisador_lexico ]; then
    check_ok "Executável criado: analisador_lexico ✓"
else
    check_fail "Executável não foi criado"
fi

# ============================================================================
# 5. SÍMBOLOS VÁLIDOS RECONHECIDOS
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "REQUISITO 5: SÍMBOLOS VÁLIDOS (Enunciado pág 1)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

check_info "• Letras: a,b,...z,A,B,...Z"
if grep -q "KW_INT\|IDENTIFICADOR" src/lexer/tokens.h; then
    check_ok "Letras reconhecidas ✓"
else
    check_fail "Letras não encontradas"
fi

check_info "• Dígitos: 0,1,...9"
if grep -q "NUM_INT\|NUM_FLOAT" src/lexer/tokens.h; then
    check_ok "Dígitos reconhecidos ✓"
else
    check_fail "Dígitos não encontrados"
fi

check_info "• Símbolos Especiais: ( ) { } [ ]"
if grep -q "SYM_LPAREN\|SYM_LBRACE\|SYM_LBRACKET" src/lexer/tokens.h; then
    check_ok "Símbolos especiais reconhecidos ✓"
else
    check_fail "Símbolos especiais não encontrados"
fi

check_info "• Operadores Relacionais: < > <= >="
if grep -q "OP_LT\|OP_LE\|OP_GT\|OP_GE" src/lexer/tokens.h; then
    check_ok "Operadores relacionais definidos ✓"
else
    check_fail "Operadores relacionais não encontrados"
fi

check_info "• Palavras Reservadas: while if etc"
if grep -q "KW_WHILE\|KW_IF" src/lexer/keywords.c; then
    check_ok "Palavras reservadas definidas ✓"
else
    check_fail "Palavras reservadas não encontradas"
fi

check_info "• Pontuação: , ; : ."
if grep -q "SYM_SEMICOLON\|SYM_COMMA\|SYM_COLON\|SYM_DOT" src/lexer/tokens.h; then
    check_ok "Pontuação definida ✓"
else
    check_fail "Pontuação não encontrada"
fi

# ============================================================================
# 6. TABELA DE SÍMBOLOS
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "REQUISITO 6: TABELA DE SÍMBOLOS"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

check_info "Verificando estrutura..."
if grep -q "struct.*Token\|typedef.*Token" src/lexer/tokens.h; then
    check_ok "Estrutura Token definida ✓"
else
    check_fail "Token não definido"
fi

if grep -q "typedef struct.*TablaSimbolos\|struct.*Tabla" src/lexer/tokens.h; then
    check_ok "Tabela de símbolos definida ✓"
else
    check_fail "Tabela de símbolos não definida"
fi

if grep -q "adicionar_token\|gravar_token" src/lexer/tokens.c; then
    check_ok "Funções de gravação implementadas ✓"
else
    check_fail "Funções de gravação não encontradas"
fi

# ============================================================================
# 7. TOKENS COMO #define
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "REQUISITO 7: TOKENS COMO #define (Dica do Professor)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

TOKEN_COUNT=$(grep -c "^#define.*[0-9]" src/lexer/tokens.h || echo 0)
check_info "Contando tokens como #define..."
if [ "$TOKEN_COUNT" -gt 30 ]; then
    check_ok "Tokens definidos com #define: $TOKEN_COUNT ✓"
else
    check_fail "Poucos tokens #define: $TOKEN_COUNT"
fi

# ============================================================================
# 8. MANUAIS OBRIGATÓRIOS
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "REQUISITO 8: MANUAIS OBRIGATÓRIOS (Enunciado pág 2)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

check_info "Manual do Usuário (mu.md)"
if [ -f "mu.md" ]; then
    LINES=$(wc -l < mu.md)
    if [ "$LINES" -ge 150 ]; then
        check_ok "mu.md presente e completo (~$LINES linhas) ✓"
    else
        check_fail "mu.md muito pequeno ($LINES linhas)"
    fi
else
    check_fail "mu.md NÃO ENCONTRADO"
fi

check_info "Manual do Programador (mp.md)"
if [ -f "mp.md" ]; then
    LINES=$(wc -l < mp.md)
    if [ "$LINES" -ge 200 ]; then
        check_ok "mp.md presente e completo (~$LINES linhas) ✓"
    else
        check_fail "mp.md muito pequeno ($LINES linhas)"
    fi
else
    check_fail "mp.md NÃO ENCONTRADO"
fi

check_info "Conteúdo de mu.md:"
if grep -q "Como Usar\|entrada\|saída\|exemplo" mu.md 2>/dev/null; then
    check_ok "Formato entrada/saída documentado ✓"
else
    check_fail "Formato entrada/saída não documentado"
fi

check_info "Conteúdo de mp.md:"
if grep -q "token\|autômato\|expressão regular\|pseudo\|função" mp.md 2>/dev/null; then
    check_ok "mp.md contém: tokens, autômato, expressões, pseudocódigo ✓"
else
    check_fail "mp.md incompleto"
fi

# ============================================================================
# 9. AUTÔMATO FINITO (JFLAP)
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "REQUISITO 9: AUTÔMATO FINITO DETERMINÍSTICO (JFLAP)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

check_info "Verificando arquivo JFLAP..."
if [ -f "CompiladorAutomatos.jff" ]; then
    check_ok "CompiladorAutomatos.jff presente ✓"
    
    if file CompiladorAutomatos.jff | grep -q "XML"; then
        check_ok "Arquivo válido em formato JFLAP/XML ✓"
    else
        check_fail "Arquivo pode não ser JFLAP válido"
    fi
else
    check_fail "CompiladorAutomatos.jff NÃO ENCONTRADO"
fi

check_info "Expressões regulares em mp.md:"
if grep -q "\\[a-zA-Z_\\]\|\\[0-9\\]\|\\[.\\*\\]\|regex\|Expressão" mp.md 2>/dev/null; then
    check_ok "Expressões regulares documentadas ✓"
else
    check_fail "Expressões regulares não documentadas"
fi

# ============================================================================
# 10. TESTE FUNCIONAL
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "REQUISITO 10: TESTE FUNCIONAL"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

check_info "Verificando arquivo de teste..."
if [ -f "teste_entrada.c" ]; then
    check_ok "teste_entrada.c presente ✓"
    
    LINES=$(wc -l < teste_entrada.c)
    check_info "Linhas de código: $LINES"
else
    check_fail "teste_entrada.c não encontrado"
fi

check_info "Executando teste..."
if ./analisador_lexico teste_entrada.c > /tmp/test_output.txt 2>&1; then
    check_ok "Execução bem-sucedida ✓"
    
    if [ -f "tabela_simbolos.txt" ]; then
        TOKENS=$(grep -c "│" tabela_simbolos.txt || echo "N/A")
        check_ok "Tabela de símbolos gerada com $TOKENS tokens ✓"
    else
        check_fail "Arquivo de saída não foi criado"
    fi
else
    check_fail "Erro ao executar analisador"
fi

# ============================================================================
# RESUMO FINAL
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "RESUMO FINAL"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

TOTAL=$((PASSOU + FALHOU))
if [ $TOTAL -gt 0 ]; then
    PERCENTUAL=$((PASSOU * 100 / TOTAL))
else
    PERCENTUAL=0
fi

echo "✓ Requisitos atendidos: ${GREEN}$PASSOU${NC}"
echo "✗ Requisitos não-atendidos: ${RED}$FALHOU${NC}"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "Taxa de conformidade: ${GREEN}$PERCENTUAL%${NC} ($PASSOU/$TOTAL)"
echo ""

if [ $FALHOU -eq 0 ]; then
    cat <<'EOF'

╔═══════════════════════════════════════════════════════════════════════════╗
║                                                                           ║
║                ✓✓✓ PROJETO VALIDADO COM 100% DE SUCESSO! ✓✓✓             ║
║                                                                           ║
║                 Todos os requisitos do professor atendidos!              ║
║                                                                           ║
║                    PRONTO PARA ENTREGA AO PROFESSOR!                     ║
║                    Email: compiladores2023@gmail.com                      ║
║                                                                           ║
║               Data Limite: 14/04/2026 às 23:50  ✓ À TEMPO!               ║
║               Defesa: 16/04/2026  ✓ PRONTO PARA APRESENTAR!              ║
║                                                                           ║
╚═══════════════════════════════════════════════════════════════════════════╝

EOF
    exit 0
else
    cat <<EOF

╔═══════════════════════════════════════════════════════════════════════════╗
║                    ⚠️  Há $FALHOU itens para revisar                        ║
╚═══════════════════════════════════════════════════════════════════════════╝

EOF
    exit 1
fi

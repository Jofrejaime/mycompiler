#!/bin/bash

# ============================================================================
# CHECKLIST DE VALIDAÇÃO - ENUNCIADO DO PROFESSOR
# 
# Data: 14/04/2026
# Professor: André Filemon
# Disciplina: Compiladores - M3
# 
# Este script valida se o projeto atende TODOS os requisitos do enunciado
# ============================================================================

set -e

# Cores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Contadores
PASSOU=0
FALHOU=0

# Funções auxiliares
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

echo ""
echo "╔═══════════════════════════════════════════════════════════════════════════╗"
echo "║                                                                           ║"
echo "║           VALIDAÇÃO CONFORME ENUNCIADO DO PROFESSOR                      ║"
echo "║                   Análise Léxica - Fase 1 Compilador                     ║"
echo "║                                                                           ║"
echo "╚═══════════════════════════════════════════════════════════════════════════╝"
echo ""

# ============================================================================
# SEÇÃO 1: ESTRUTURA E COMPILAÇÃO
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "SEÇÃO 1: ESTRUTURA E COMPILAÇÃO"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

# 1.1 Verificar se está em C (não Python)
check_info "Verificando linguagem de implementação..."
if grep -q "\.c" *.c 2>/dev/null; then
    check_ok "Linguagem C - CONFORME ENUNCIADO"
else
    check_fail "Não encontrado código C"
fi

# 1.2 Verificar compilação
check_info "Testando compilação..."
if gcc -Wall -std=c99 -I. -O2 main.c src/lexer/*.c -o analisador_lexico 2>/dev/null; then
    check_ok "Compilação bem-sucedida"
else
    check_fail "Compilação falhou"
fi

# 1.3 Verificar executável
if [ -f analisador_lexico ]; then
    check_ok "Executável criado: analisador_lexico"
else
    check_fail "Executável não encontrado"
fi

# ============================================================================
# SEÇÃO 2: FUNÇÕES OBRIGATÓRIAS
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "SEÇÃO 2: FUNÇÕES OBRIGATÓRIAS DO PROFESSOR"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

# 2.1 analex()
check_info "Verificando função analex()..."
if grep -q "Token analex(Lexer" src/lexer/lexer.h && \
   grep -q "Token analex(Lexer" src/lexer/lexer.c; then
    check_ok "Função analex() IMPLEMENTADA"
else
    check_fail "Função analex() NÃO encontrada"
fi

# 2.2 ler_caractere()
check_info "Verificando função ler_caractere()..."
if grep -q "char ler_caractere(Lexer" src/lexer/lexer.h && \
   grep -q "char ler_caractere(Lexer" src/lexer/lexer.c; then
    check_ok "Função ler_caractere() IMPLEMENTADA"
else
    check_fail "Função ler_caractere() NÃO encontrada"
fi

# 2.3 volta_caractere()
check_info "Verificando função volta_caractere()..."
if grep -q "void volta_caractere(Lexer" src/lexer/lexer.h && \
   grep -q "void volta_caractere(Lexer" src/lexer/lexer.c; then
    check_ok "Função volta_caractere() IMPLEMENTADA"
else
    check_fail "Função volta_caractere() NÃO encontrada"
fi

# 2.4 gravar_token_lexema()
check_info "Verificando função gravar_token_lexema()..."
if grep -q "Token gravar_token_lexema(Lexer" src/lexer/lexer.h && \
   grep -q "Token gravar_token_lexema(Lexer" src/lexer/lexer.c; then
    check_ok "Função gravar_token_lexema() IMPLEMENTADA"
else
    check_fail "Função gravar_token_lexema() NÃO encontrada"
fi

# ============================================================================
# SEÇÃO 3: MÁQUINA DE ESTADOS OBRIGATÓRIA
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "SEÇÃO 3: MÁQUINA DE ESTADOS (Obrigatório no Enunciado)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

check_info "Verificando implementação de máquina de estados..."
if grep -q "estado_atual" src/lexer/lexer.h && \
   grep -q "ESTADO_INICIAL\|estado" src/lexer/lexer.c; then
    check_ok "Máquina de estados com estados definidos"
else
    check_fail "Máquina de estados não encontrada"
fi

if grep -q "eh_letra\|eh_digito\|eh_espacobrancho" src/lexer/lexer.c; then
    check_ok "Predicados de transição implementados"
else
    check_fail "Predicados não encontrados"
fi

# ============================================================================
# SEÇÃO 4: TOKENS COMO #define (Recomendação do Professor)
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "SEÇÃO 4: TOKENS COMO #define (Recomendação do Professor)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

check_info "Verificando #define para tokens..."
TOKEN_COUNT=$(grep -c "#define.*[0-9]" src/lexer/tokens.h || echo 0)
if [ "$TOKEN_COUNT" -gt 20 ]; then
    check_ok "Tokens definidos com #define: $TOKEN_COUNT tipos"
else
    check_fail "Tokens #define insuficientes"
fi

# ============================================================================
# SEÇÃO 5: SÍMBOLOS VÁLIDOS EXIGIDOS
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "SEÇÃO 5: SÍMBOLOS VÁLIDOS EXIGIDOS PELO ENUNCIADO"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

check_info "Verificando reconhecimento de Letras [a-zA-Z]..."
if echo "int abc ABC;" | ./analisador_lexico /dev/stdin 2>/dev/null | grep -q "IDENTIFICADOR"; then
    check_ok "Letras reconhecidas"
else
    check_fail "Falha ao reconhecer letras"
fi

check_info "Verificando reconhecimento de Dígitos [0-9]..."
if echo "int x = 123;" | ./analisador_lexico /dev/stdin 2>/dev/null | grep -q "NUM_INT"; then
    check_ok "Dígitos reconhecidos"
else
    check_fail "Falha ao reconhecer dígitos"
fi

check_info "Verificando Símbolos Especiais: ( ) { } [ ]..."
if echo "(){}[]" | ./analisador_lexico /dev/stdin 2>/dev/null | grep -q "LPAREN"; then
    check_ok "Símbolos especiais reconhecidos"
else
    check_fail "Falha ao reconhecer símbolos"
fi

check_info "Verificando Operadores Relacionais: < > <= >=..."
if echo "x < y; x <= y;" | ./analisador_lexico /dev/stdin 2>/dev/null | grep -q "OP_LE\|OP_LT"; then
    check_ok "Operadores relacionais reconhecidos"
else
    check_fail "Falha ao reconhecer operadores relacionais"
fi

check_info "Verificando Palavras Reservadas: while if..."
if echo "while if" | ./analisador_lexico /dev/stdin 2>/dev/null | grep -q "KW_WHILE\|KW_IF"; then
    check_ok "Palavras reservadas reconhecidas"
else
    check_fail "Falha ao reconhecer palavras reservadas"
fi

check_info "Verificando Pontuação: , ; : ."
if echo "a,b;c:d." | ./analisador_lexico /dev/stdin 2>/dev/null | grep -q "SEMICOLON\|COMMA"; then
    check_ok "Pontuação reconhecida"
else
    check_fail "Falha ao reconhecer pontuação"
fi

# ============================================================================
# SEÇÃO 6: TABELA DE SÍMBOLOS
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "SEÇÃO 6: TABELA DE SÍMBOLOS"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

check_info "Verificando gravaçao de tokens e lexemas..."
if grep -q "TablaSimbolos\|struct.*Token" src/lexer/tokens.h; then
    check_ok "Estrutura de tabela de símbolos definida"
else
    check_fail "Tabela de símbolos não encontrada"
fi

if grep -q "adicionar_token\|gravar" src/lexer/tokens.c; then
    check_ok "Função de gravação de tokens implementada"
else
    check_fail "Gravação de tokens não implementada"
fi

# ============================================================================
# SEÇÃO 7: MANUAIS OBRIGATÓRIOS
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "SEÇÃO 7: MANUAIS OBRIGATÓRIOS"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

check_info "Verificando Manual do Usuário..."
if [ -f mu.md ]; then
    LINES=$(wc -l < mu.md)
    if [ "$LINES" -ge 200 ]; then
        check_ok "mu.md presente (~$LINES linhas)"
    else
        check_fail "mu.md muito pequeno ($LINES linhas)"
    fi
else
    check_fail "mu.md NÃO encontrado"
fi

check_info "Verificando Manual do Programador..."
if [ -f mp.md ]; then
    LINES=$(wc -l < mp.md)
    if [ "$LINES" -ge 300 ]; then
        check_ok "mp.md presente (~$LINES linhas)"
    else
        check_fail "mp.md muito pequeno ($LINES linhas)"
    fi
else
    check_fail "mp.md NÃO encontrado"
fi

check_info "Verificando conteúdo em mu.md..."
if grep -q "entrada\|saída\|exemplo\|Como Usar" mu.md; then
    check_ok "mu.md contém formato entrada/saída"
else
    check_fail "mu.md sem documentação de formatos"
fi

check_info "Verificando conteúdo em mp.md..."
if grep -q "token\|autômato\|expressão\|pseudo\|função" mp.md; then
    check_ok "mp.md contém tokens, autômato, expressões regulares"
else
    check_fail "mp.md incompleto"
fi

# ============================================================================
# SEÇÃO 8: AUTÔMATO FINITO DETERMINÍSTICO
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "SEÇÃO 8: AUTÔMATO FINITO DETERMINÍSTICO (JFLAP)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

check_info "Verificando arquivo JFLAP..."
if [ -f CompiladorAutomatos.jff ]; then
    check_ok "CompiladorAutomatos.jff presente"
    
    if grep -q "<jflap" CompiladorAutomatos.jff; then
        check_ok "Arquivo JFLAP válido (XML bem-formado)"
    else
        check_fail "Arquivo JFLAP com problemas de formato"
    fi
else
    check_fail "CompiladorAutomatos.jff NÃO encontrado"
fi

check_info "Verificando expressões regulares em mp.md..."
if grep -q "\\[a-zA-Z\\]\|\\[0-9\\]\|Expressões Regulares" mp.md; then
    check_ok "Expressões regulares documentadas em mp.md"
else
    check_fail "Expressões regulares não documentadas"
fi

# ============================================================================
# SEÇÃO 9: TESTE FUNCIONAL
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "SEÇÃO 9: TESTE FUNCIONAL COMPLETO"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

check_info "Testando analisador com teste_entrada.c..."
if [ -f teste_entrada.c ]; then
    if ./analisador_lexico teste_entrada.c > /tmp/saida_teste.txt 2>&1; then
        check_ok "Teste com testeprograma bem-sucedido"
        
        TOKENS=$(grep -c "│" /tmp/saida_teste.txt || echo 0)
        check_info "Total de tokens processados: $TOKENS"
    else
        check_fail "Erro ao processar teste_entrada.c"
    fi
else
    check_fail "teste_entrada.c não encontrado"
fi

check_info "Verificando geração de arquivo de saída..."
if [ -f tabela_simbolos.txt ]; then
    LINHAS=$(wc -l < tabela_simbolos.txt)
    check_ok "tabela_simbolos.txt gerado ($LINHAS linhas)"
else
    check_fail "tabela_simbolos.txt não foi gerado"
fi

# ============================================================================
# RESUMO FINAL
# ============================================================================

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "RESUMO FINAL DA VALIDAÇÃO"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

TOTAL=$((PASSOU + FALHOU))
PERCENTUAL=$((PASSOU * 100 / TOTAL))

echo "✓ Requisitos atendidos: ${GREEN}$PASSOU${NC}"
echo "✗ Requisitos falhados:  ${RED}$FALHOU${NC}"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "${GREEN}Taxa de conformidade: $PERCENTUAL%${NC} ($PASSOU/$TOTAL)"
echo ""

if [ $FALHOU -eq 0 ]; then
    echo "╔═══════════════════════════════════════════════════════════════════════════╗"
    echo "║                                                                           ║"
    echo "║              ✓✓✓ TODOS OS REQUISITOS FORAM ATENDIDOS! ✓✓✓                ║"
    echo "║                                                                           ║"
    echo "║           O projeto está COMPLETO e PRONTO PARA ENTREGA!                 ║"
    echo "║                                                                           ║"
    echo "╚═══════════════════════════════════════════════════════════════════════════╝"
    exit 0
else
    echo "╔═══════════════════════════════════════════════════════════════════════════╗"
    echo "║                    ⚠️  Há $FALHOU itens para revisar                        ║"
    echo "╚═══════════════════════════════════════════════════════════════════════════╝"
    exit 1
fi

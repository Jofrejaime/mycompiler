#include "tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
   IMPLEMENTAÇÃO: TABELA DE SÍMBOLOS E FUNÇÕES DE TOKEN
   ============================================================================ */

/* ============================================================================
   CRIAR TABELA DE SÍMBOLOS
   
   Inicializa uma nova tabela com capacidade inicial
   Aloca espaço para array de tokens
   ============================================================================ */
tabla_simbolos_t* criar_tabla_simbolos(int capacidade_inicial) {
    if (capacidade_inicial <= 0) {
        capacidade_inicial = 100;
    }
    
    tabla_simbolos_t *ts = (tabla_simbolos_t *)malloc(sizeof(tabla_simbolos_t));
    if (ts == NULL) {
        return NULL;
    }
    
    ts->tokens = (token_t *)malloc(sizeof(token_t) * capacidade_inicial);
    if (ts->tokens == NULL) {
        free(ts);
        return NULL;
    }
    
    ts->capacidade = capacidade_inicial;
    ts->quantidade = 0;
    ts->posicao_leitura = 0;
    
    return ts;
}

/* ============================================================================
   ADICIONAR TOKEN À TABELA
   
   Se tabela está cheia, realoca com capacidade dobrada
   Copia token para array
   ============================================================================ */
void adicionar_token(tabla_simbolos_t *ts, token_t t) {
    if (ts == NULL) {
        return;
    }
    
    /* Verificar se precisa expandir */
    if (ts->quantidade >= ts->capacidade) {
        /* Dobrar capacidade */
        int nova_capacidade = ts->capacidade * 2;
        token_t *novo_array = (token_t *)realloc(ts->tokens, sizeof(token_t) * nova_capacidade);
        
        if (novo_array == NULL) {
            fprintf(stderr, "Erro: Não foi possível expandir tabela de símbolos\n");
            return;
        }
        
        ts->tokens = novo_array;
        ts->capacidade = nova_capacidade;
    }
    
    /* Copiar token */
    ts->tokens[ts->quantidade] = t;
    ts->quantidade++;
}

/* ============================================================================
   LIBERAR TABELA DE SÍMBOLOS
   
   Libera memória alocada para array de tokens e estrutura
   ============================================================================ */
void liberar_tabla_simbolos(tabla_simbolos_t *ts) {
    if (ts == NULL) {
        return;
    }
    
    if (ts->tokens != NULL) {
        free(ts->tokens);
    }
    
    free(ts);
}

/* ============================================================================
   CONVERTER TIPO DE TOKEN PARA STRING
   
   Útil para debugging e exibição
   Retorna string descritiva do tipo de token
   ============================================================================ */
const char* tipo_token_para_string(int tipo) {
    switch (tipo) {
        /* Palavras-chave */
        case KW_INT:        return "KW_INT";
        case KW_CHAR:       return "KW_CHAR";
        case KW_VOID:       return "KW_VOID";
        case KW_FLOAT:      return "KW_FLOAT";
        case KW_DOUBLE:     return "KW_DOUBLE";
        case KW_IF:         return "KW_IF";
        case KW_ELSE:       return "KW_ELSE";
        case KW_WHILE:      return "KW_WHILE";
        case KW_FOR:        return "KW_FOR";
        case KW_DO:         return "KW_DO";
        case KW_BREAK:      return "KW_BREAK";
        case KW_CONTINUE:   return "KW_CONTINUE";
        case KW_RETURN:     return "KW_RETURN";
        case KW_STRUCT:     return "KW_STRUCT";
        case KW_UNION:      return "KW_UNION";
        case KW_TYPEDEF:    return "KW_TYPEDEF";
        case KW_SIZEOF:     return "KW_SIZEOF";
        case KW_STATIC:     return "KW_STATIC";
        case KW_EXTERN:     return "KW_EXTERN";
        case KW_CONST:      return "KW_CONST";
        case KW_VOLATILE:   return "KW_VOLATILE";
        case KW_SWITCH:     return "KW_SWITCH";
        case KW_CASE:       return "KW_CASE";
        case KW_DEFAULT:    return "KW_DEFAULT";
        case KW_GOTO:       return "KW_GOTO";
        
        /* Identificadores e constantes */
        case TK_ID:         return "IDENTIFICADOR";
        case TK_NUM_INT:    return "NUM_INT";
        case TK_NUM_FLOAT:  return "NUM_FLOAT";
        case TK_NUM_EXP:    return "NUM_EXP";
        case TK_STRING:     return "STRING";
        case TK_CHAR:       return "CHAR";
        
        /* Operadores aritméticos */
        case OP_PLUS:       return "OP_PLUS (+)";
        case OP_MINUS:      return "OP_MINUS (-)";
        case OP_MULT:       return "OP_MULT (*)";
        case OP_DIV:        return "OP_DIV (/)";
        case OP_MOD:        return "OP_MOD (%)";
        
        /* Operadores de atribuição */
        case OP_ASSIGN:     return "OP_ASSIGN (=)";
        case OP_PLUS_ASSIGN: return "OP_PLUS_ASSIGN (+=)";
        case OP_MINUS_ASSIGN: return "OP_MINUS_ASSIGN (-=)";
        case OP_MULT_ASSIGN: return "OP_MULT_ASSIGN (*=)";
        case OP_DIV_ASSIGN: return "OP_DIV_ASSIGN (/=)";
        case OP_AND_ASSIGN: return "OP_AND_ASSIGN (&=)";
        case OP_OR_ASSIGN:  return "OP_OR_ASSIGN (|=)";
        case OP_XOR_ASSIGN: return "OP_XOR_ASSIGN (^=)";
        
        /* Operadores relacionais */
        case OP_EQ:         return "OP_EQ (==)";
        case OP_NE:         return "OP_NE (!=)";
        case OP_LT:         return "OP_LT (<)";
        case OP_LE:         return "OP_LE (<=)";
        case OP_GT:         return "OP_GT (>)";
        case OP_GE:         return "OP_GE (>=)";
        
        /* Operadores lógicos */
        case OP_AND:        return "OP_AND (&&)";
        case OP_OR:         return "OP_OR (||)";
        case OP_NOT:        return "OP_NOT (!)";
        
        /* Operadores bitwise */
        case OP_BITAND:     return "OP_BITAND (&)";
        case OP_BITOR:      return "OP_BITOR (|)";
        case OP_BITXOR:     return "OP_BITXOR (^)";
        case OP_BITNOT:     return "OP_BITNOT (~)";
        case OP_LSHIFT:     return "OP_LSHIFT (<<)";
        case OP_RSHIFT:     return "OP_RSHIFT (>>)";
        
        /* Operadores unários */
        case OP_INC:        return "OP_INC (++)";
        case OP_DEC:        return "OP_DEC (--)";
        case OP_ARROW:      return "OP_ARROW (->)";
        
        /* Símbolos especiais */
        case SYM_LPAREN:    return "SYM_LPAREN ())";
        case SYM_RPAREN:    return "SYM_RPAREN ())";
        case SYM_LBRACE:    return "SYM_LBRACE ({)";
        case SYM_RBRACE:    return "SYM_RBRACE (})";
        case SYM_LBRACKET:  return "SYM_LBRACKET ([)";
        case SYM_RBRACKET:  return "SYM_RBRACKET (])";
        
        /* Pontuação */
        case SYM_SEMICOLON: return "SYM_SEMICOLON (;)";
        case SYM_COMMA:     return "SYM_COMMA (,)";
        case SYM_DOT:       return "SYM_DOT (.)";
        case SYM_COLON:     return "SYM_COLON (:)";
        case SYM_TERNARY:   return "SYM_TERNARY (?)";
        
        /* Especiais */
        case TK_EOF:        return "EOF";
        case TK_ERROR:      return "ERRO";
        
        default:            return "DESCONHECIDO";
    }
}

/* ============================================================================
   IMPRIMIR TABELA DE SÍMBOLOS
   
   Função de debug: exibe todos os tokens em formato tabular
   Mostra: tipo, lexema, linha, coluna
   ============================================================================ */
void imprimir_tabla_simbolos(tabla_simbolos_t *ts) {
    if (ts == NULL || ts->quantidade == 0) {
        printf("Tabela de símbolos vazia\n");
        return;
    }
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                    TABELA DE SÍMBOLOS (TOKENS)                        ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ Nº  │ Tipo                    │ Lexema         │ Linha │ Coluna │ Valor│\n");
    printf("╟─────┼─────────────────────────┼────────────────┼───────┼────────┼───────╢\n");
    
    for (int i = 0; i < ts->quantidade; i++) {
        token_t *t = &ts->tokens[i];
        const char *tipo_str = tipo_token_para_string(t->tipo);
        
        printf("│ %3d │ %-23s │ %-14s │ %5d │ %6d │",
               i + 1, tipo_str, t->lexeme, t->linha, t->coluna);
        
        /* Exibir valor para números */
        if (t->tipo == TK_NUM_INT) {
            printf(" %5d│\n", t->valor.valor_int);
        } else if (t->tipo == TK_NUM_FLOAT || t->tipo == TK_NUM_EXP) {
            printf(" %.2f│\n", t->valor.valor_float);
        } else {
            printf("      │\n");
        }
    }
    
    printf("╠════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ TOTAL DE TOKENS: %-55d │\n", ts->quantidade);
    printf("╚════════════════════════════════════════════════════════════════════════╝\n");
}

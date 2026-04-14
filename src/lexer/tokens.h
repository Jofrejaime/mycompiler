#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
   ENUNCIADO DE COMPILADORES - PROF. ANDRÉ FILEMON
   ANALISADOR LÉXICO - DEFINIÇÃO DE TOKENS
   
   Baseado em: Compiladores - Princípios, Técnicas e Ferramentas
   ============================================================================ */

/* ============================================================================
   TOKEN TYPES - CONSTANTES COM #define
   ============================================================================ */

/* Palavras-chave da linguagem C (Subconjunto) */
#define KW_INT          1
#define KW_CHAR         2
#define KW_VOID         3
#define KW_FLOAT        4
#define KW_DOUBLE       5
#define KW_IF           6
#define KW_ELSE         7
#define KW_WHILE        8
#define KW_FOR          9
#define KW_DO           10
#define KW_BREAK        11
#define KW_CONTINUE     12
#define KW_RETURN       13
#define KW_STRUCT       14
#define KW_UNION        15
#define KW_TYPEDEF      16
#define KW_SIZEOF       17
#define KW_STATIC       18
#define KW_EXTERN       19
#define KW_CONST        20
#define KW_VOLATILE     21
#define KW_SWITCH       22
#define KW_CASE         23
#define KW_DEFAULT      24
#define KW_GOTO         25

/* Identificadores e Constantes */
#define TK_ID           50      /* Identificador: [a-zA-Z_][a-zA-Z0-9_]* */
#define TK_NUM_INT      51      /* Número inteiro: [0-9]+ */
#define TK_NUM_FLOAT    52      /* Número float: [0-9]+\.[0-9]+ */
#define TK_NUM_EXP      53      /* Notação científica: [0-9]+ e/E [+-]? [0-9]+ */
#define TK_STRING       54      /* String: "..." */
#define TK_CHAR         55      /* Char: '.' */

/* Operadores Aritméticos */
#define OP_PLUS         100     /* + */
#define OP_MINUS        101     /* - */
#define OP_MULT         102     /* * */
#define OP_DIV          103     /* / */
#define OP_MOD          104     /* % */

/* Operadores de Atribuição */
#define OP_ASSIGN       110     /* = */
#define OP_PLUS_ASSIGN  111     /* += */
#define OP_MINUS_ASSIGN 112     /* -= */
#define OP_MULT_ASSIGN  113     /* *= */
#define OP_DIV_ASSIGN   114     /* /= */

/* Operadores Relacionais */
#define OP_EQ           120     /* == */
#define OP_NE           121     /* != */
#define OP_LT           122     /* < */
#define OP_LE           123     /* <= */
#define OP_GT           124     /* > */
#define OP_GE           125     /* >= */

/* Operadores Lógicos */
#define OP_AND          130     /* && */
#define OP_OR           131     /* || */
#define OP_NOT          132     /* ! */

/* Operadores Bitwise */
#define OP_BITAND       140     /* & */
#define OP_BITOR        141     /* | */
#define OP_BITXOR       142     /* ^ */
#define OP_BITNOT       143     /* ~ */
#define OP_LSHIFT       144     /* << */
#define OP_RSHIFT       145     /* >> */

/* Operadores Unários */
#define OP_INC          150     /* ++ */
#define OP_DEC          151     /* -- */
#define OP_ARROW        152     /* -> */

/* Símbolos Especiais */
#define SYM_LPAREN      200     /* ( */
#define SYM_RPAREN      201     /* ) */
#define SYM_LBRACE      202     /* { */
#define SYM_RBRACE      203     /* } */
#define SYM_LBRACKET    204     /* [ */
#define SYM_RBRACKET    205     /* ] */

/* Pontuação */
#define SYM_SEMICOLON   210     /* ; */
#define SYM_COMMA       211     /* , */
#define SYM_DOT         212     /* . */
#define SYM_COLON       213     /* : */
#define SYM_TERNARY     214     /* ? */

/* Fim de Arquivo e Erro */
#define TK_EOF          300
#define TK_ERROR        301

/* ============================================================================
   ESTRUTURA DE DADOS: TOKEN
   ============================================================================ */

typedef struct {
    int tipo;                   /* Tipo de token (constante #define) */
    char lexeme[256];           /* String do token: "int", "x", "10", etc. */
    int linha;                  /* Linha no arquivo-fonte */
    int coluna;                 /* Coluna no arquivo-fonte */
    union {
        int valor_int;          /* Para TK_NUM_INT */
        double valor_float;     /* Para TK_NUM_FLOAT e TK_NUM_EXP */
    } valor;
} Token;

/* ============================================================================
   ESTRUTURA: SYMBOL TABLE (Tabela de Símbolos)
   ============================================================================ */

typedef struct {
    Token *tokens;              /* Array de tokens */
    int quantidade;             /* Número de tokens adicionados */
    int capacidade;             /* Capacidade máxima do array */
    int posicao_leitura;        /* Posição atual para leitura (para parser) */
} TablaSimbolos;

/* ============================================================================
   FUNÇÕES DE UTILIDADE PARA TOKENS
   ============================================================================ */

/* Criar e inicializar tabela de símbolos */
TablaSimbolos* criar_tabla_simbolos(int capacidade_inicial);

/* Adicionar token à tabela */
void adicionar_token(TablaSimbolos *ts, Token t);

/* Liberar memória da tabela */
void liberar_tabla_simbolos(TablaSimbolos *ts);

/* Imprimir tabela de símbolos (debug) */
void imprimir_tabla_simbolos(TablaSimbolos *ts);

/* Converter tipo de token para string (para debug) */
const char* tipo_token_para_string(int tipo);

#endif /* TOKENS_H */

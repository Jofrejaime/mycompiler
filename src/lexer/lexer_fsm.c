#include "lexer.h"
#include "keywords.h"
#include <string.h>
#include <ctype.h>

/* ============================================================================
   MACROS AUXILIARES
   ============================================================================ */

/* Macro para simplificar retorno de tokens com posição */
#define RETURN_TOKEN(lexer, tipo, lexema) \
    gravar_token_lexema(lexer, tipo, lexema, \
                       lexer->linha_inicio_lexema, \
                       lexer->coluna_inicio_lexema)

/* Macro para retroceder caractere e retornar token */
#define BACKTRACK_RETURN(lexer, c, tipo, lexema) \
    do { \
        volta_caractere(lexer, c); \
        return RETURN_TOKEN(lexer, tipo, lexema); \
    } while(0)

/* ============================================================================
   AUTÔMATO FINITO DETERMINÍSTICO - MÁQUINA DE ESTADOS
   
   Implementação completa da máquina de estados conforme especificação
   do autômato. Estados organizados por grupos léxicos:
   
   - G0/q0: Estado inicial
   - G8/q1: Whitespace
   - G2/q2-q4: Identificadores e palavras-chave
   - G3/q3-q5: Números inteiros e floats
   - G7/q6-q12: Comentários (linhas e blocos)
   - G5/q13-q22: Operadores e símbolos
   - G4/q23-q32: Strings e char literals
   
   ============================================================================ */

/* ============================================================================
   ESTADOS DO AUTÔMATO (Enumeração)
   ============================================================================ */

typedef enum {
    Q0_INICIAL = 0,
    
    /* G8 - Whitespace */
    Q1_WHITESPACE,
    
    /* G2 - Identificadores */
    Q2_IDENT,
    
    /* G3 - Números */
    Q3_INT,
    Q5_FLOAT,
    
    /* G7 - Comentários */
    Q6_SLASH,           /* Após primeiro forward slash */
    Q7_COMMENT_LINE,    /* Dentro de linha comentada */
    Q8_RESERVED,        /* Reservado */
    Q9_COMMENT_BLOCK,   /* Dentro de bloco comentado */
    Q10_COMMENT_STAR,   /* Após asterisco em bloco comentado */
    Q11_RESERVED,       /* Reservado */
    Q12_RESERVED,       /* Reservado */
    
    /* G5 - Operadores */
    Q13_OP_EQ,          /* Operador = */
    Q14_OP_NOT,         /* Operador ! (NOT lógico) */
    Q15_OP_PLUS,        /* Operador + */
    Q16_OP_BITOR,       /* Operador | (OR bit-wise) */
    Q17_OP_MINUS,       /* Operador - */
    Q18_OP_BITAND,      /* Operador & (AND bit-wise) */
    Q19_OP_BITXOR,      /* Operador ^ (XOR bit-wise) */
    Q20_OP_LT,          /* Operador < */
    Q21_OP_GT,          /* Operador > */
    Q22_OP_BITNOT,      /* Operador ~ (NOT bit-wise) */
    Q_OP_MULT,          /* Operador * e *= */
    Q25_OP_AND_ASSIGN,  /* Operador &= */
    Q26_OP_OR_ASSIGN,   /* Operador |= */
    Q27_OP_XOR_ASSIGN,  /* Operador ^= */
    
    /* G4 - Strings e Char */
    Q28_STRING_OPEN,    /* Dentro de " */
    Q29_STRING_ESCAPE,  /* Após \ em string */
    Q30_CHAR_OPEN,      /* Após ' */
    Q31_CHAR_ESCAPE,    /* Após \ em char */
    Q32_CHAR_AWAIT,     /* Aguardando ' */
    Q33_RESERVED,       /* Reservado */
    Q34_RESERVED,       /* Reservado */
    
    /* G9 - Pré-processador */
    Q35_PP_DIRECTIVE,   /* Dentro de diretiva #... */

    /* G3 - Expoente (notação científica: 1.5e3, 2E-4) */
    Q_EXP_E,            /* Após 'e' ou 'E' em número */
    Q_EXP_SIGN,         /* Após sinal +/- do expoente */
    Q_EXP_DIGITS        /* Dígitos do expoente */
} Estado_FSM;

/* ============================================================================
   MÁQUINA DE ESTADOS PRINCIPAL - FSM (Finite State Machine)
   
   Lê caractere por caractere e transiciona entre estados
   Quando atinge um estado final, retorna o token correspondente
   ============================================================================ */

token_t fsm_next_token(lexer_t *lexer) {
    if (lexer == NULL) {
        return (token_t){TK_ERROR, "", 0, 0, {0}};
    }
    
    Estado_FSM estado = Q0_INICIAL;
    int buffer_pos = 0;
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));

    char c;
    while (1) {
        /* Captura a posição de início do lexema no momento em que começamos a
           reconhecê-lo. Em Q0 o buffer está sempre vazio; whitespace/comentários
           fazem volta_caractere e regressam a Q0, pelo que esta re-captura fixa a
           posição do PRIMEIRO caractere real do lexema (e não a do token anterior). */
        if (estado == Q0_INICIAL) {
            lexer->linha_inicio_lexema = lexer->linha;
            lexer->coluna_inicio_lexema = lexer->coluna;
        }
        c = ler_caractere(lexer);
        
        switch (estado) {
        
        /* ═════════════════════════════════════════════════════════════════ */
        /* Q0: ESTADO INICIAL - Determina categoria léxica                  */
        /* ═════════════════════════════════════════════════════════════════ */
        case Q0_INICIAL:
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                estado = Q1_WHITESPACE;
            }
            else if (isalpha(c) || c == '_') {
                buffer[buffer_pos++] = c;
                estado = Q2_IDENT;
            }
            else if (isdigit(c)) {
                buffer[buffer_pos++] = c;
                estado = Q3_INT;
            }
            else if (c == '/') {
                estado = Q6_SLASH;
            }
            else if (c == '"') {
                estado = Q28_STRING_OPEN;
            }
            else if (c == '\'') {
                estado = Q30_CHAR_OPEN;  /* Atualizado de Q28 */
            }
            else if (c == '=') {
                estado = Q13_OP_EQ;
            }
            else if (c == '+') {
                estado = Q15_OP_PLUS;
            }
            else if (c == '-') {
                estado = Q17_OP_MINUS;
            }
            else if (c == '<') {
                estado = Q20_OP_LT;
            }
            else if (c == '>') {
                estado = Q21_OP_GT;
            }
            else if (c == '!') {
                estado = Q14_OP_NOT;
            }
            else if (c == '&') {
                estado = Q18_OP_BITAND;
            }
            else if (c == '|') {
                estado = Q16_OP_BITOR;
            }
            else if (c == '^') {
                estado = Q19_OP_BITXOR;
            }
            else if (c == '~') {
                buffer[0] = c;
                return RETURN_TOKEN(lexer, OP_BITNOT, "~");
            }
            else if (c == '*') {
                estado = Q_OP_MULT;
            }
            else if (c == '%') {
                buffer[0] = c;
                return RETURN_TOKEN(lexer, OP_MOD, "%");
            }
            else if (c == '#') {
                buffer[buffer_pos++] = c;
                estado = Q35_PP_DIRECTIVE;
            }
            else if (c == '\0') {
                return (token_t){TK_EOF, "", lexer->linha, lexer->coluna, {0}};
            }
            else {
                /* Símbolos simples */
                buffer[0] = c;
                int tipo = TK_ERROR;
                if (c == '(') tipo = SYM_LPAREN;
                else if (c == ')') tipo = SYM_RPAREN;
                else if (c == '{') tipo = SYM_LBRACE;
                else if (c == '}') tipo = SYM_RBRACE;
                else if (c == '[') tipo = SYM_LBRACKET;
                else if (c == ']') tipo = SYM_RBRACKET;
                else if (c == ';') tipo = SYM_SEMICOLON;
                else if (c == ',') tipo = SYM_COMMA;
                else if (c == '.') tipo = SYM_DOT;
                else if (c == ':') tipo = SYM_COLON;
                else if (c == '?') tipo = SYM_TERNARY;
                
                return RETURN_TOKEN(lexer, tipo, buffer);
            }
            break;
        
        /* ═════════════════════════════════════════════════════════════════ */
        /* G8/Q1: WHITESPACE - Pular espaços em branco                     */
        /* ═════════════════════════════════════════════════════════════════ */
        case Q1_WHITESPACE:
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                /* Self-loop: continua consumindo whitespace */
            } else {
                volta_caractere(lexer, c);
                buffer_pos = 0;
                memset(buffer, 0, sizeof(buffer));
                estado = Q0_INICIAL;
                continue;
            }
            break;
        
        /* ═════════════════════════════════════════════════════════════════ */
        /* G2/Q2: IDENTIFICADORES E PALAVRAS-CHAVE                         */
        /* ═════════════════════════════════════════════════════════════════ */
        case Q2_IDENT:
            if (isalnum(c) || c == '_') {
                if (buffer_pos < 255) buffer[buffer_pos++] = c;
            } else {
                volta_caractere(lexer, c);
                buffer[buffer_pos] = '\0';
                
                /* Procurar se é palavra-chave */
                int tipo_kw = procurar_palavra_chave(buffer);
                if (tipo_kw == TK_ID) tipo_kw = TK_ID;
                
                return RETURN_TOKEN(lexer, tipo_kw, buffer);
            }
            break;
        
        /* ═════════════════════════════════════════════════════════════════ */
        /* G3/Q3: NÚMEROS INTEIROS                                         */
        /* ═════════════════════════════════════════════════════════════════ */
        case Q3_INT:
            if (isdigit(c)) {
                if (buffer_pos < 255) buffer[buffer_pos++] = c;
            } else if (c == '.') {
                if (buffer_pos < 255) buffer[buffer_pos++] = c;
                estado = Q5_FLOAT;
            } else if (c == 'e' || c == 'E') {
                /* Notação científica: 2e3, 1E10 */
                if (buffer_pos < 255) buffer[buffer_pos++] = c;
                estado = Q_EXP_E;
            } else {
                volta_caractere(lexer, c);
                buffer[buffer_pos] = '\0';
                return RETURN_TOKEN(lexer, TK_NUM_INT, buffer);
            }
            break;
        
        /* ═════════════════════════════════════════════════════════════════ */
        /* G3/Q5: NÚMEROS FLOAT (após ponto)                               */
        /* ═════════════════════════════════════════════════════════════════ */
        case Q5_FLOAT:
            if (isdigit(c)) {
                if (buffer_pos < 255) buffer[buffer_pos++] = c;
            } else if (c == 'e' || c == 'E') {
                /* Notação científica: 1.5e3, 3.14E-2 */
                if (buffer_pos < 255) buffer[buffer_pos++] = c;
                estado = Q_EXP_E;
            } else {
                volta_caractere(lexer, c);
                buffer[buffer_pos] = '\0';
                return RETURN_TOKEN(lexer, TK_NUM_FLOAT, buffer);
            }
            break;
        
        /* ═════════════════════════════════════════════════════════════════ */
        /* G3: NOTAÇÃO CIENTÍFICA — estados de expoente                    */
        /* ═════════════════════════════════════════════════════════════════ */
        case Q_EXP_E:
            /* Após 'e'/'E': aceita sinal opcional ou dígito imediato */
            if (c == '+' || c == '-') {
                if (buffer_pos < 255) buffer[buffer_pos++] = c;
                estado = Q_EXP_SIGN;
            } else if (isdigit(c)) {
                if (buffer_pos < 255) buffer[buffer_pos++] = c;
                estado = Q_EXP_DIGITS;
            } else {
                /* 'e'/'E' sem dígitos — não é expoente válido.
                   Emite o número sem o 'e' e devolve 'e' e o char actual ao stream. */
                buffer_pos--;              /* apaga o 'e'/'E' do buffer */
                buffer[buffer_pos] = '\0';
                volta_caractere(lexer, c); /* devolve o char actual */
                /* Nota: o 'e'/'E' será relido como início de identificador */
                int has_dot = 0;
                for (int i = 0; i < buffer_pos; i++) {
                    if (buffer[i] == '.') { has_dot = 1; break; }
                }
                return RETURN_TOKEN(lexer, has_dot ? TK_NUM_FLOAT : TK_NUM_INT, buffer);
            }
            break;

        case Q_EXP_SIGN:
            /* Após sinal do expoente: obrigatório pelo menos um dígito */
            if (isdigit(c)) {
                if (buffer_pos < 255) buffer[buffer_pos++] = c;
                estado = Q_EXP_DIGITS;
            } else {
                /* Sinal sem dígitos — emite o acumulado e retrocede */
                volta_caractere(lexer, c);
                buffer[buffer_pos] = '\0';
                return RETURN_TOKEN(lexer, TK_NUM_EXP, buffer);
            }
            break;

        case Q_EXP_DIGITS:
            /* Dígitos do expoente: continua até não-dígito */
            if (isdigit(c)) {
                if (buffer_pos < 255) buffer[buffer_pos++] = c;
            } else {
                volta_caractere(lexer, c);
                buffer[buffer_pos] = '\0';
                return RETURN_TOKEN(lexer, TK_NUM_EXP, buffer);
            }
            break;
        
        /* ═════════════════════════════════════════════════════════════════ */
        /* G7/Q6-Q12: COMENTÁRIOS                                          */
        /* ═════════════════════════════════════════════════════════════════ */
        case Q6_SLASH:
            if (c == '/') {
                estado = Q7_COMMENT_LINE;
            } else if (c == '*') {
                estado = Q9_COMMENT_BLOCK;
            } else if (c == '=') {
                return RETURN_TOKEN(lexer, OP_DIV_ASSIGN, "/=");
            } else {
                BACKTRACK_RETURN(lexer, c, OP_DIV, "/");
            }
            break;


        /* ═════════════════════════════════════════════════════════════════ */
        /* Q_OP_MULT: Operador * e *=                                         */
        /* ═════════════════════════════════════════════════════════════════ */
        case Q_OP_MULT:
            if (c == '=') {
                return RETURN_TOKEN(lexer, OP_MULT_ASSIGN, "*=");
            } else {
                BACKTRACK_RETURN(lexer, c, OP_MULT, "*");
            }
            break;
        
        case Q7_COMMENT_LINE:
            if (c == '\n' || c == '\0') {
                volta_caractere(lexer, c);
                /* Continuar ao Q0 para próximo token */
                estado = Q0_INICIAL;
                buffer_pos = 0;
                memset(buffer, 0, sizeof(buffer));
            }
            break;
        
        case Q9_COMMENT_BLOCK:
            if (c == '*') {
                estado = Q10_COMMENT_STAR;
            } else if (c == '\0') {
                /* EOF dentro de comentário de bloco — erro, não loop infinito */
                return (token_t){TK_ERROR, "Comentario de bloco nao fechado",
                                lexer->linha_inicio_lexema,
                                lexer->coluna_inicio_lexema, {0}};
            }
            break;
        
        case Q10_COMMENT_STAR:
            if (c == '/') {
                /* Fim do comentário de bloco */
                estado = Q0_INICIAL;
                buffer_pos = 0;
                memset(buffer, 0, sizeof(buffer));
            } else if (c == '\0') {
                /* EOF logo após '*' — comentário não fechado */
                return (token_t){TK_ERROR, "Comentario de bloco nao fechado",
                                lexer->linha_inicio_lexema,
                                lexer->coluna_inicio_lexema, {0}};
            } else if (c != '*') {
                estado = Q9_COMMENT_BLOCK;
            }
            break;
        
        /* ═════════════════════════════════════════════════════════════════ */
        /* G5/Q13-Q22: OPERADORES                                          */
        /* ═════════════════════════════════════════════════════════════════ */
        /* ═════════════════════════════════════════════════════════════════ */
        /* Operador NOT (!) e !=                                           */
        /* ═════════════════════════════════════════════════════════════════ */
        /* Operador = e == */
        case Q13_OP_EQ:
            if (c == '=') {
                return RETURN_TOKEN(lexer, OP_EQ, "==");
            } else {
                volta_caractere(lexer, c);
                return gravar_token_lexema(lexer, OP_ASSIGN, "=",
                                          lexer->linha_inicio_lexema,
                                          lexer->coluna_inicio_lexema);
            }
            break;
        
        case Q14_OP_NOT:
            if (c == '=') {
                return gravar_token_lexema(lexer, OP_NE, "!=",
                                          lexer->linha_inicio_lexema,
                                          lexer->coluna_inicio_lexema);
            } else {
                volta_caractere(lexer, c);
                return gravar_token_lexema(lexer, OP_NOT, "!",
                                          lexer->linha_inicio_lexema,
                                          lexer->coluna_inicio_lexema);
            }
            break;
        
        case Q15_OP_PLUS:
            if (c == '+') {
                return RETURN_TOKEN(lexer, OP_INC, "++");
            } else if (c == '=') {
                return RETURN_TOKEN(lexer, OP_PLUS_ASSIGN, "+=");
            } else {
                BACKTRACK_RETURN(lexer, c, OP_PLUS, "+");
            }
            break;
        
        case Q16_OP_BITOR:
            if (c == '|') {
                return RETURN_TOKEN(lexer, OP_OR, "||");
            } else if (c == '=') {
                return RETURN_TOKEN(lexer, OP_OR_ASSIGN, "|=");
            } else {
                BACKTRACK_RETURN(lexer, c, OP_BITOR, "|");
            }
            break;
        
        case Q18_OP_BITAND:
            if (c == '&') {
                return gravar_token_lexema(lexer, OP_AND, "&&",
                                          lexer->linha_inicio_lexema,
                                          lexer->coluna_inicio_lexema);
            } else if (c == '=') {
                return gravar_token_lexema(lexer, OP_AND_ASSIGN, "&=",
                                          lexer->linha_inicio_lexema,
                                          lexer->coluna_inicio_lexema);
            } else {
                volta_caractere(lexer, c);
                return gravar_token_lexema(lexer, OP_BITAND, "&",
                                          lexer->linha_inicio_lexema,
                                          lexer->coluna_inicio_lexema);
            }
            break;
        
        case Q19_OP_BITXOR:
            if (c == '=') {
                return gravar_token_lexema(lexer, OP_XOR_ASSIGN, "^=",
                                          lexer->linha_inicio_lexema,
                                          lexer->coluna_inicio_lexema);
            } else {
                volta_caractere(lexer, c);
                return gravar_token_lexema(lexer, OP_BITXOR, "^",
                                          lexer->linha_inicio_lexema,
                                          lexer->coluna_inicio_lexema);
            }
            break;
        
        case Q17_OP_MINUS:
            if (c == '-') {
                return gravar_token_lexema(lexer, OP_DEC, "--",
                                          lexer->linha_inicio_lexema,
                                          lexer->coluna_inicio_lexema);
            } else if (c == '=') {
                return gravar_token_lexema(lexer, OP_MINUS_ASSIGN, "-=",
                                          lexer->linha_inicio_lexema,
                                          lexer->coluna_inicio_lexema);
            } else if (c == '>') {
                return RETURN_TOKEN(lexer, OP_ARROW, "->");
            } else {
                volta_caractere(lexer, c);
                return gravar_token_lexema(lexer, OP_MINUS, "-",
                                          lexer->linha_inicio_lexema,
                                          lexer->coluna_inicio_lexema);
            }
            break;
        
        case Q20_OP_LT:
            if (c == '=') {
                return RETURN_TOKEN(lexer, OP_LE, "<=");
            } else if (c == '<') {
                return RETURN_TOKEN(lexer, OP_LSHIFT, "<<");
            } else {
                BACKTRACK_RETURN(lexer, c, OP_LT, "<");
            }
            break;
        
        case Q21_OP_GT:
            if (c == '=') {
                return RETURN_TOKEN(lexer, OP_GE, ">=");
            } else if (c == '>') {
                return RETURN_TOKEN(lexer, OP_RSHIFT, ">>");
            } else {
                BACKTRACK_RETURN(lexer, c, OP_GT, ">");
            }
            break;
        
        /* ═════════════════════════════════════════════════════════════════ */
        /* G4/Q23-Q32: STRINGS                                             */
        /* ═════════════════════════════════════════════════════════════════ */
        case Q28_STRING_OPEN:
            if (c == '"') {
                buffer[buffer_pos] = '\0';
                return RETURN_TOKEN(lexer, TK_STRING, buffer);
            } else if (c == '\\') {
                estado = Q29_STRING_ESCAPE;
            } else if (c == '\0') {
                return (token_t){TK_ERROR, "Unterminated string", 
                                lexer->linha_inicio_lexema,
                                lexer->coluna_inicio_lexema, {0}};
            } else {
                if (buffer_pos < 255) buffer[buffer_pos++] = c;
            }
            break;
        
        case Q29_STRING_ESCAPE:
            /* Traduzir escape sequence para o valor real */
            switch (c) {
                case 'n':  buffer[buffer_pos++] = '\n'; break;
                case 't':  buffer[buffer_pos++] = '\t'; break;
                case 'r':  buffer[buffer_pos++] = '\r'; break;
                case '\\': buffer[buffer_pos++] = '\\'; break;
                case '"':  buffer[buffer_pos++] = '"';  break;
                case '0':  buffer[buffer_pos++] = '\0'; break;
                case 'a':  buffer[buffer_pos++] = '\a'; break;
                case 'b':  buffer[buffer_pos++] = '\b'; break;
                case 'f':  buffer[buffer_pos++] = '\f'; break;
                case 'v':  buffer[buffer_pos++] = '\v'; break;
                default:   buffer[buffer_pos++] = c;    break;
            }
            estado = Q28_STRING_OPEN;
            break;
        
        /* ═════════════════════════════════════════════════════════════════ */
        /* G4/Q28-Q32: CHAR LITERALS                                       */
        /* ═════════════════════════════════════════════════════════════════ */
        case Q30_CHAR_OPEN:
            if (c == '\\') {
                estado = Q31_CHAR_ESCAPE;
            } else {
                buffer[buffer_pos++] = c;
                estado = Q32_CHAR_AWAIT;
            }
            break;
        
        case Q31_CHAR_ESCAPE:
            /* Traduzir escape sequence para o valor real */
            switch (c) {
                case 'n':  buffer[buffer_pos++] = '\n'; break;
                case 't':  buffer[buffer_pos++] = '\t'; break;
                case 'r':  buffer[buffer_pos++] = '\r'; break;
                case '\\': buffer[buffer_pos++] = '\\'; break;
                case '\'': buffer[buffer_pos++] = '\''; break;
                case '0':  buffer[buffer_pos++] = '\0'; break;
                case 'a':  buffer[buffer_pos++] = '\a'; break;
                case 'b':  buffer[buffer_pos++] = '\b'; break;
                case 'f':  buffer[buffer_pos++] = '\f'; break;
                case 'v':  buffer[buffer_pos++] = '\v'; break;
                default:   buffer[buffer_pos++] = c;    break;
            }
            estado = Q32_CHAR_AWAIT;
            break;
        
        case Q32_CHAR_AWAIT:
            if (c == '\'') {
                buffer[buffer_pos] = '\0';
                return RETURN_TOKEN(lexer, TK_CHAR, buffer);
            } else {
                return (token_t){TK_ERROR, "Invalid char literal",
                                lexer->linha_inicio_lexema,
                                lexer->coluna_inicio_lexema, {0}};
            }
            break;
        
        /* ═════════════════════════════════════════════════════════════════ */
        /* G9/Q35: DIRETIVAS DE PRE-PROCESSADOR (#include, #define, etc)   */
        /* ═════════════════════════════════════════════════════════════════ */
        case Q35_PP_DIRECTIVE:
            if (c == '\n' || c == '\0') {
                /* Fim da diretiva */
                buffer[buffer_pos] = '\0';
                
                /* Identificar tipo de preprocessor */
                int tipo_pp = identificar_preprocessor(buffer);
                
                /* Retracar o \n ou \0 para proximo token */
                volta_caractere(lexer, c);
                
                return RETURN_TOKEN(lexer, tipo_pp, buffer);
            } else {
                /* Continua acumulando caracteres da diretiva */
                if (buffer_pos < 255) {
                    buffer[buffer_pos++] = c;
                }
            }
            break;
        
        default:
            return (token_t){TK_ERROR, "Unknown state", 0, 0, {0}};
        
        } /* fim switch */
    } /* fim while */
    
    return (token_t){TK_EOF, "", lexer->linha, lexer->coluna, {0}};
}

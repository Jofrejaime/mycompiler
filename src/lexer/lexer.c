#include "lexer.h"
#include <string.h>

extern token_t process_identifier(lexer_t *lexer, char first_char);
extern token_t process_number(lexer_t *lexer, char first_char);
extern token_t process_string(lexer_t *lexer);
extern token_t process_char_literal(lexer_t *lexer);
extern token_t process_operator(lexer_t *lexer, char first_char);
extern token_t process_symbol(lexer_t *lexer, char c);
extern void skip_whitespace_and_comments(lexer_t *lexer, char *c);

/* ============================================================================
   ANALISADOR LÉXICO PRINCIPAL - MÁQUINA DE ESTADOS COM SWITCH
   
   Entrada: lexer inicializado
   Saída: próximo token encontrado
   
   q0 (ESTADO_INICIAL):
   - Letra/_  → process_identifier()
   - Dígito   → process_number()
   - "        → process_string()
   - '        → process_char_literal()
   - +,-,*,/,% → process_operator()
   - =,<,>    → process_operator()
   - !,&,|,^,~ → process_operator()
   - (,),[,]  → process_symbol()
   - {,}      → process_symbol()
   - ;,,,.:,? → process_symbol()
   - Outro    → TK_ERROR
   ============================================================================ */

token_t analex(lexer_t *lexer) {
    if (lexer == NULL) return (token_t){TK_ERROR, "", 0, 0, {0}};
    
    char c;
    skip_whitespace_and_comments(lexer, &c);
    
    if (c == '\0') {
        return (token_t){TK_EOF, "", lexer->linha, lexer->coluna, {0}};
    }
    
    lexer->linha_inicio_lexema = lexer->linha;
    lexer->coluna_inicio_lexema = lexer->coluna - 1;
    memset(lexer->lexema, 0, sizeof(lexer->lexema));
    
    /* MÁQUINA DE ESTADOS - SWITCH AUTOMATO */
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
        return process_identifier(lexer, c);
    }
    
    switch (c) {
        /* NÚMEROS */
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return process_number(lexer, c);
        
        /* STRINGS E CHARS */
        case '"':  return process_string(lexer);
        case '\'': return process_char_literal(lexer);
        
        /* OPERADORES ARITMÉTICOS E LÓGICOS */
        case '+': case '-': case '*': case '/': case '%':
        case '=': case '<': case '>': case '!':
        case '&': case '|': case '^': case '~':
            return process_operator(lexer, c);
        
        /* SÍMBOLOS ESPECIAIS */
        case '(': case ')': case '{': case '}':
        case '[': case ']': case ';': case ',':
        case '.': case ':': case '?':
            return process_symbol(lexer, c);
        
        /* ERRO LÉXICO */
        default: {
            token_t error;
            error.tipo = TK_ERROR;
            error.lexeme[0] = c;
            error.lexeme[1] = '\0';
            error.linha = lexer->linha_inicio_lexema;
            error.coluna = lexer->coluna_inicio_lexema;
            return error;
        }
    }
}

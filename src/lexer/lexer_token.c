#include "lexer.h"
#include <string.h>
#include <stdlib.h>

token_t gravar_token_lexema(lexer_t *lexer, int tipo, const char *lexema, 
                          int linha, int coluna) {
    token_t t;
    t.tipo = tipo;
    t.linha = linha;
    t.coluna = coluna;
    
    strncpy(t.lexeme, lexema, sizeof(t.lexeme) - 1);
    t.lexeme[sizeof(t.lexeme) - 1] = '\0';
    
    t.valor.valor_int = 0;
    t.valor.valor_float = 0.0;
    
    if (tipo == TK_NUM_INT) {
        t.valor.valor_int = atoi(lexema);
    } else if (tipo == TK_NUM_FLOAT || tipo == TK_NUM_EXP) {
        t.valor.valor_float = atof(lexema);
    }
    
    if (lexer->tabla_simbolos != NULL) {
        adicionar_token(lexer->tabla_simbolos, t);
    }
    
    return t;
}

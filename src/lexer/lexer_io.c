#include "lexer.h"
#include <string.h>

static void refill_buffer(lexer_t *lexer) {
    if (lexer->arquivo_entrada == NULL) return;
    
    lexer->tamanho_buffer = fread(lexer->buffer, 1, sizeof(lexer->buffer), 
                                   lexer->arquivo_entrada);
    lexer->pos_buffer = 0;
}

static void update_position(lexer_t *lexer, char c) {
    if (c == '\n') {
        lexer->linha++;
        lexer->coluna = 1;
    } else if (c != '\r') {
        lexer->coluna++;
    }
}

static void revert_position(lexer_t *lexer, char c) {
    if (c == '\n') {
        lexer->linha--;
    } else if (c != '\r') {
        lexer->coluna--;
    }
}

char ler_caractere(lexer_t *lexer) {
    if (lexer == NULL) return '\0';
    
    if (lexer->teve_retracao) {
        lexer->teve_retracao = 0;
        return lexer->caractere_retratado;
    }
    
    if (lexer->pos_buffer >= lexer->tamanho_buffer) {
        refill_buffer(lexer);
        if (lexer->tamanho_buffer == 0) {
            return '\0';
        }
    }
    
    char c = lexer->buffer[lexer->pos_buffer++];
    update_position(lexer, c);
    
    return c;
}

void volta_caractere(lexer_t *lexer, char c) {
    if (lexer == NULL) return;
    
    lexer->caractere_retratado = c;
    lexer->teve_retracao = 1;
    revert_position(lexer, c);
}

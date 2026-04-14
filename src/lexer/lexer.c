#include "lexer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern token_t process_identifier(lexer_t *lexer, char first_char);
extern token_t process_number(lexer_t *lexer, char first_char);
extern token_t process_string(lexer_t *lexer);
extern token_t process_char_literal(lexer_t *lexer);
extern token_t process_operator(lexer_t *lexer, char first_char);
extern token_t process_symbol(lexer_t *lexer, char c);
extern void skip_whitespace_and_comments(lexer_t *lexer, char *c);

token_t analex(lexer_t *lexer) {
    if (lexer == NULL) {
        token_t error;
        error.tipo = TK_ERROR;
        strcpy(error.lexeme, "");
        return error;
    }
    
    char c;
    skip_whitespace_and_comments(lexer, &c);
    
    if (c == '\0') {
        token_t eof;
        eof.tipo = TK_EOF;
        eof.lexeme[0] = '\0';
        eof.linha = lexer->linha;
        eof.coluna = lexer->coluna;
        return eof;
    }
    
    lexer->linha_inicio_lexema = lexer->linha;
    lexer->coluna_inicio_lexema = lexer->coluna - 1;
    memset(lexer->lexema, 0, sizeof(lexer->lexema));
    
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
        return process_identifier(lexer, c);
    
    if (c >= '0' && c <= '9')
        return process_number(lexer, c);
    
    if (c == '"') return process_string(lexer);
    if (c == '\'') return process_char_literal(lexer);
    
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
        c == '=' || c == '<' || c == '>' || c == '!' || c == '&' ||
        c == '|' || c == '^' || c == '~')
        return process_operator(lexer, c);
    
    if (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' ||
        c == ';' || c == ',' || c == '.' || c == ':' || c == '?')
        return process_symbol(lexer, c);
    
    token_t error;
    error.tipo = TK_ERROR;
    error.lexeme[0] = c;
    error.lexeme[1] = '\0';
    error.linha = lexer->linha_inicio_lexema;
    error.coluna = lexer->coluna_inicio_lexema;
    return error;
}

lexer_t* criar_lexer(const char *nome_arquivo) {
    if (nome_arquivo == NULL) return NULL;
    
    lexer_t *lexer = (lexer_t *)malloc(sizeof(lexer_t));
    if (lexer == NULL) return NULL;
    
    lexer->arquivo_entrada = fopen(nome_arquivo, "r");
    if (lexer->arquivo_entrada == NULL) {
        free(lexer);
        return NULL;
    }
    
    lexer->pos_buffer = 0;
    lexer->tamanho_buffer = 0;
    lexer->linha = 1;
    lexer->coluna = 1;
    lexer->linha_inicio_lexema = 1;
    lexer->coluna_inicio_lexema = 1;
    lexer->pos_lexema = 0;
    lexer->estado_atual = ESTADO_INICIAL;
    lexer->teve_retracao = 0;
    lexer->caractere_retratado = '\0';
    
    memset(lexer->lexema, 0, sizeof(lexer->lexema));
    memset(lexer->buffer, 0, sizeof(lexer->buffer));
    
    lexer->tabla_simbolos = criar_tabla_simbolos(100);
    
    return lexer;
}

void liberar_lexer(lexer_t *lexer) {
    if (lexer == NULL) return;
    
    if (lexer->arquivo_entrada != NULL) {
        fclose(lexer->arquivo_entrada);
    }
    
    if (lexer->tabla_simbolos != NULL) {
        liberar_tabla_simbolos(lexer->tabla_simbolos);
    }
    
    free(lexer);
}

tabla_simbolos_t* processar_arquivo_completo(const char *nome_arquivo) {
    lexer_t *lexer = criar_lexer(nome_arquivo);
    if (lexer == NULL) return NULL;
    
    token_t token;
    do {
        token = analex(lexer);
    } while (token.tipo != TK_EOF);
    
    tabla_simbolos_t *resultado = lexer->tabla_simbolos;
    
    if (lexer->arquivo_entrada != NULL) {
        fclose(lexer->arquivo_entrada);
        lexer->arquivo_entrada = NULL;
    }
    lexer->tabla_simbolos = NULL;
    
    liberar_lexer(lexer);
    return resultado;
}

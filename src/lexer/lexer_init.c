#include "lexer.h"
#include <stdlib.h>
#include <stdio.h>

/* ============================================================================
   INICIALIZAÇÃO E LIMPEZA DO LEXER
   ============================================================================ */

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

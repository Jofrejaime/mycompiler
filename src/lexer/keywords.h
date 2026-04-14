#ifndef KEYWORDS_H
#define KEYWORDS_H

#include "tokens.h"

/* ============================================================================
   TABELA DE PALAVRAS-CHAVE (Keywords Lookup Table)
   ============================================================================ */

typedef struct {
    char *palavra;      /* Palavra-chave: "int", "while", etc. */
    int token_tipo;     /* Tipo de token correspondente */
} Palavra_Chave;

/* Função para procurar palavra-chave na tabela */
int procurar_palavra_chave(const char *palavra);

/* Função para obter todas as palavras-chave (para debug) */
Palavra_Chave* obter_tabela_palavras_chave(int *quantidade);

#endif /* KEYWORDS_H */

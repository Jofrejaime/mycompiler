#include "keywords.h"
#include <string.h>

/* ============================================================================
   TABELA DE PALAVRAS-CHAVE - IMPLEMENTAÇÃO
   
   Tabela de lookup para keywords da linguagem C (Subconjunto)
   O lexer consulta esta tabela quando encontra um identificador
   ============================================================================ */

static Palavra_Chave tabela_palavras_chave[] = {
    /* Tipos de dados */
    {"int",         KW_INT},
    {"char",        KW_CHAR},
    {"void",        KW_VOID},
    {"float",       KW_FLOAT},
    {"double",      KW_DOUBLE},
    
    /* Controle de fluxo */
    {"if",          KW_IF},
    {"else",        KW_ELSE},
    {"while",       KW_WHILE},
    {"for",         KW_FOR},
    {"do",          KW_DO},
    {"break",       KW_BREAK},
    {"continue",    KW_CONTINUE},
    {"return",      KW_RETURN},
    {"switch",      KW_SWITCH},
    {"case",        KW_CASE},
    {"default",     KW_DEFAULT},
    {"goto",        KW_GOTO},
    
    /* Tipos compostos */
    {"struct",      KW_STRUCT},
    {"union",       KW_UNION},
    {"typedef",     KW_TYPEDEF},
    
    /* Qualificadores e modificadores */
    {"static",      KW_STATIC},
    {"extern",      KW_EXTERN},
    {"const",       KW_CONST},
    {"volatile",    KW_VOLATILE},
    
    /* Operadores especiais */
    {"sizeof",      KW_SIZEOF},
    
    /* Sentinela: marca o fim da tabela */
    {NULL,          0}
};

/* ============================================================================
   PROCURAR PALAVRA-CHAVE NA TABELA
   
   Entrada: palavra - string a procurar (ex: "int", "while")
   Saída: tipo de token se encontrado, TK_ID se não for palavra-chave
   
   Usa busca linear (poderia ser otimizado com hash table)
   ============================================================================ */
int procurar_palavra_chave(const char *palavra) {
    if (palavra == NULL || strlen(palavra) == 0) {
        return TK_ID;
    }
    
    /* Percorrer tabela até encontrar sentinela ou correspondência exata */
    for (int i = 0; tabela_palavras_chave[i].palavra != NULL; i++) {
        if (strcmp(tabela_palavras_chave[i].palavra, palavra) == 0) {
            /* Encontrou! Retornar tipo de token */
            return tabela_palavras_chave[i].token_tipo;
        }
    }
    
    /* Não encontrou na tabela: é um identificador comum */
    return TK_ID;
}

/* ============================================================================
   OBTER TABELA COMPLETA DE PALAVRAS-CHAVE (para debug/documentação)
   
   Entrada: ponteiro para variável que ficará com a quantidade
   Saída: ponteiro para array de Palavra_Chave
   ============================================================================ */
Palavra_Chave* obter_tabela_palavras_chave(int *quantidade) {
    if (quantidade == NULL) {
        return NULL;
    }
    
    /* Contar palavras-chave (até sentinela) */
    *quantidade = 0;
    for (int i = 0; tabela_palavras_chave[i].palavra != NULL; i++) {
        (*quantidade)++;
    }
    
    return (Palavra_Chave *)tabela_palavras_chave;
}

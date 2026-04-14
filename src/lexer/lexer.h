#ifndef LEXER_H
#define LEXER_H

#include "tokens.h"
#include <stdio.h>

/* ============================================================================
   ANALISADOR LÉXICO (LEXER) - HEADER
   
   Implementa a PRIMEIRA FASE do compilador conforme enunciado de Compiladores.
   
   Funções Obrigatórias (conforme professor):
   - analex()                 Analisar e criar tokens
   - ler_caractere()         Leitura caractere por caractere
   - volta_caractere()       Retroceder um caractere (OUTRO no autômato)
   - gravar_token_lexema()   Guardar token na tabela de símbolos
   
   ============================================================================ */

/* ============================================================================
   ESTRUTURA DO LEXER (MÁQUINA DE ESTADOS)
   ============================================================================ */

typedef struct lexer_s {
    FILE *arquivo_entrada;      /* Arquivo-fonte sendo analisado */
    char buffer[1024];          /* Buffer de entrada (para leitura eficiente) */
    int pos_buffer;             /* Posição atual no buffer */
    int tamanho_buffer;         /* Tamanho efetivo dos dados no buffer */
    
    int linha;                  /* Linha atual no arquivo */
    int coluna;                 /* Coluna atual no arquivo */
    int linha_inicio_lexema;    /* Linha onde começou o lexema atual */
    int coluna_inicio_lexema;   /* Coluna onde começou o lexema atual */
    
    char lexema[256];           /* Acumulador de caracteres do lexema */
    int pos_lexema;             /* Posição no lexema */
    
    tabla_simbolos_t *tabla_simbolos;  /* Tabela de símbolos */
    
    /* Controle de retraçamento */
    char caractere_retratado;   /* Caractere lido demais (volta_caractere) */
    int teve_retracao;          /* Flag se houve retraçamento */
    
    /* Estado atual da máquina de estados */
    int estado_atual;           /* Estado do autômato finito determinístico */
} lexer_t;

/* ============================================================================
   ESTADOS DO AUTÔMATO FINITO DETERMINÍSTICO (Máquina de Estados)
   
   Baseado no arquivo CompiladorAutomatos.jff
   Estados q0-q79 conforme seu autômato
   ============================================================================ */

#define ESTADO_INICIAL      0   /* q0 */
#define ESTADO_FINAL        -1  /* Estado sink (não existe) */

/* ============================================================================
   FUNÇÕES OBRIGATÓRIAS (CONFORME ENUNCIADO DO PROFESSOR)
   ============================================================================ */

/*
   1. ANALEX() - Analisador Léxico Principal
   
   Descrição: Implementa os autômatos finitos determinísticos.
   Faz a validação do lexema e token, chamando gravar_token_lexema().
   
   Entrada: lexer - estrutura do lexer já inicializada
   Saída: token_t - o próximo token encontrado no arquivo
   
   Algoritmo:
   - Pula espaços em branco
   - Pula comentários
   - Inicia máquina de estados no estado q0
   - Segue transições conforme caracteres lidos
   - Quando atinge estado final, cria token
   - Chama volta_caractere() quando necessário (transição "outro")
   - Retorna token criado
*/
token_t analex(lexer_t *lexer);

/*
   2. LER_CARACTERE() - Leitura Caractere por Caractere
   
   Descrição: Lê o próximo caractere do arquivo-fonte.
   Mantém controle de linha e coluna.
   Usa buffer para eficiência.
   
   Entrada: lexer - estrutura do lexer
   Saída: char - o próximo caractere, ou '\0' se EOF
   
   Nota: Função interna, chamada por analex()
*/
char ler_caractere(lexer_t *lexer);

/*
   3. VOLTA_CARACTERE() - Retraçamento (Backtracking)
   
   Descrição: Retrocede um caractere quando a máquina de estados
   lê "outro" (caractere que não pertence ao padrão atual).
   
   Usado quando o lexer lê um caractere demais.
   Exemplo: "10abc" - após reconhecer "10", o 'a' é retraçado.
   
   Entrada: lexer - estrutura do lexer, c - caractere a retroceder
   Saída: void
   
   Nota: Armazena em lexer->caractere_retratado para próxima leitura
*/
void volta_caractere(lexer_t *lexer, char c);

/*
   4. GRAVAR_TOKEN_LEXEMA() - Guardar na Tabela de Símbolos
   
   Descrição: Recebe tipo e lexema, cria token_t e armazena
   na tabela de símbolos (tabla_simbolos_t).
   
   Entrada: lexer - estrutura do lexer
            tipo - tipo de token (KW_INT, TK_ID, NUM, ...)
            lexema - string do token ("int", "x", "123", etc.)
            linha - linha no arquivo
            coluna - coluna no arquivo
   Saída: token_t - o token criado e guardado
   
   Nota: Função complementar a analex()
*/
token_t gravar_token_lexema(lexer_t *lexer, int tipo, const char *lexema, 
                          int linha, int coluna);

/* ============================================================================
   FUNÇÕES DE INICIALIZAÇÃO E LIMPEZA
   ============================================================================ */

/*
   Criar e inicializar o lexer
   
   Entrada: nome_arquivo - caminho do arquivo-fonte a analisar
   Saída: ponteiro para Lexer inicializado, ou NULL se erro
*/
lexer_t* criar_lexer(const char *nome_arquivo);

/*
   Liberar recursos do lexer
   
   Entrada: lexer - estrutura a liberar
   Saída: void
*/
void liberar_lexer(lexer_t *lexer);

/* ============================================================================
   FUNÇÕES DE UTILIDADE
   ============================================================================ */

/* Processar arquivo inteiro: lê todos os tokens até EOF */
tabla_simbolos_t* processar_arquivo_completo(const char *nome_arquivo);

/* Imprimir estado debug do lexer */
void debug_lexer(lexer_t *lexer);

#endif /* LEXER_H */

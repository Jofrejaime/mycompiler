#include <stdio.h>
#include "printer.h"
#include "../src/lexer/tokens.h"

/* ============================================================================
   FUNCOES UTILITARIAS - IMPRESSAO E FORMATACAO
   ============================================================================ */

void print_header(void) {
    printf("\n");
    printf("===============================================================\n");
    printf("\n");
    printf("    ANALISADOR LEXICO - FASE 1 DO COMPILADOR C->C\n");
    printf("\n");
    printf("  Instituto Superior Politecnico de Tecnologias e Ciencias\n");
    printf("  Disciplina: Compiladores - M3\n");
    printf("  Professor: Andre Filemon\n");
    printf("\n");
    printf("===============================================================\n");
    printf("\n");
}

void print_usage(const char *programa) {
    printf("USO: %s <arquivo_entrada> [arquivo_saida]\n\n", programa);
    printf("Argumentos:\n");
    printf("  arquivo_entrada  Caminho do arquivo-fonte em C a analisar\n");
    printf("  arquivo_saida    (Opcional) Caminho para salvar tabela de simbolos\n");
    printf("                   Padrao: 'tabela_simbolos.txt'\n\n");
    printf("Exemplo:\n");
    printf("  %s programa.c\n", programa);
    printf("  %s programa.c resultado.txt\n", programa);
}

void print_summary(lexer_t *lexer, int error_count) {
    printf("\n");
    printf("===============================================================\n");
    printf("                         RESUMO DA ANALISE\n");
    printf("===============================================================\n");
    printf(" Total de tokens analisados: %d\n", lexer->tabla_simbolos->quantidade - 1);
    printf(" Erros lexicos encontrados:  %d\n", error_count);
    printf(" Linhas do arquivo:          %d\n", lexer->linha);
    printf("===============================================================\n");
    
    if (error_count == 0) {
        printf(" OK: Analise concluida com SUCESSO!\n");
    } else {
        printf(" AVISO: Analise concluida com ERROS (verifique acima)\n");
    }
    
    printf("===============================================================\n");
    printf("\n");
}

void save_symbol_table(const char *output_file, lexer_t *lexer, int error_count) {
    FILE *fp = fopen(output_file, "w");
    
    if (fp == NULL) {
        fprintf(stderr, "ERRO: Nao foi possivel criar arquivo '%s'\n", output_file);
        return;
    }
    
    fprintf(fp, "===============================================================\n");
    fprintf(fp, "    TABELA DE SIMBOLOS - ANALISE LEXICA\n");
    fprintf(fp, "===============================================================\n\n");
    fprintf(fp, "Total de tokens: %d\n", lexer->tabla_simbolos->quantidade - error_count);
    fprintf(fp, "Erros lexicos: %d\n\n", error_count);
    
    fprintf(fp, "TOKENS ENCONTRADOS:\n");
    fprintf(fp, "-----------------------------------------------------------\n");
    fprintf(fp, "No.  | Tipo                 | Lexema         | Linha | Coluna\n");
    fprintf(fp, "-----+----------------------+----------------+-------+-------\n");
    
    int num = 0;
    for (int i = 0; i < lexer->tabla_simbolos->quantidade; i++) {
        token_t *t = &lexer->tabla_simbolos->tokens[i];
        
        if (t->tipo == TK_EOF) continue;
        
        if (t->tipo == TK_ERROR) {
            fprintf(fp, "%3d  | [ERRO]               | %-14s | %5d | %5d\n",
                    ++num, t->lexeme, t->linha, t->coluna);
        } else {
            fprintf(fp, "%3d  | %-20s | %-14s | %5d | %5d\n",
                    ++num, tipo_token_para_string(t->tipo), t->lexeme,
                    t->linha, t->coluna);
        }
    }
    
    fprintf(fp, "\n===============================================================\n");
    fprintf(fp, "FIM DO RELATORIO\n");
    fprintf(fp, "===============================================================\n");
    
    fclose(fp);
}

/* ============================================================================
   FUNCOES DE LOGICA PRINCIPAL
   ============================================================================ */

int validate_args(int argc, char *argv[], const char **input, const char **output) {
    if (argc < 2) {
        fprintf(stderr, "ERRO: Numero de argumentos invalido\n\n");
        print_usage(argv[0]);
        return 1;
    }
    
    *input = argv[1];
    *output = (argc >= 3) ? argv[2] : "tabela_simbolos.txt";
    
    printf("Arquivo de entrada: %s\n", *input);
    printf("Arquivo de saida:   %s\n\n", *output);
    
    return 0;
}

lexer_t* run_lexical_analysis(const char *input, const char *output, int *error_count) {
    printf("Criando lexer...\n");
    lexer_t *lexer = criar_lexer(input);
    
    if (lexer == NULL) {
        fprintf(stderr, "ERRO: Nao foi possivel abrir arquivo '%s'\n", input);
        return NULL;
    }
    
    printf("OK: lexer criado\n\n");
    printf("Executando analise lexica...\n");
    printf("-----------------------------------\n\n");
    
    token_t token;
    *error_count = 0;
    
    do {
        token = analex(lexer);
        
        if (token.tipo == TK_ERROR) {
            printf("ERRO na linha %d, coluna %d: '%s'\n", 
                   token.linha, token.coluna, token.lexeme);
            (*error_count)++;
        }
    } while (token.tipo != TK_EOF);
    
    printf("\n-----------------------------------\n");
    printf("OK: Analise lexica concluida\n\n");
    printf("Tabela de simbolos:\n\n");
    imprimir_tabla_simbolos(lexer->tabla_simbolos);
    
    printf("\nSalvando em '%s'...\n", output);
    save_symbol_table(output, lexer, *error_count);
    printf("OK: Resultado salvo\n");
    
    return lexer;
}

void cleanup(lexer_t *lexer) {
    printf("Liberando recursos...\n");
    liberar_lexer(lexer);
    printf("OK: Programa finalizado com sucesso!\n\n");
}

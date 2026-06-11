#include <stdio.h>
#include <stdlib.h>
#include "src/lexer/lexer.h"
#include "src/parser/parser.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_entrada.c>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];

    /* Fase 1: Análise Léxica */
    fprintf(stderr, "\n================ ANÁLISE LÉXICA ================\n");
    tabla_simbolos_t *symbol_table = processar_arquivo_completo(input_file);
    if (!symbol_table) {
        fprintf(stderr, "Erro: Não foi possível processar arquivo '%s'\n", input_file);
        return 1;
    }

    fprintf(stderr, "Tokens processados: %d\n\n", symbol_table->quantidade);

    /* Fase 2: Análise Sintática */
    fprintf(stderr, "================ ANÁLISE SINTÁTICA ================\n");
    parser_t *parser = parser_init(symbol_table);
    if (!parser) {
        fprintf(stderr, "Erro: Falha ao inicializar parser\n");
        return 1;
    }

    parser->source_path = input_file;
    parser_run(parser);

    /* Imprime a AST */
    fprintf(stderr, "\n================ ÁRVORE SINTÁTICA ================\n");
    if (parser->ast_root) {
        print_ast(parser->ast_root, 0);
    } else {
        fprintf(stderr, "AST vazia!\n");
    }

    /* Imprime tabelas de escopo */
    fprintf(stderr, "\n================ TABELAS DE ESCOPO ================\n");
    print_all_scopes(parser);

    /* Resumo */
    fprintf(stderr, "\n================ RESUMO ================\n");
    fprintf(stderr, "Tokens processados: %d\n", symbol_table->quantidade);
    fprintf(stderr, "Erros sintáticos: %d\n", parser->error_count);

    if (parser->error_count == 0) {
        fprintf(stderr, "Status: SUCESSO\n");
    } else {
        fprintf(stderr, "Status: FALHA\n");
    }

    /* Limpeza */
    parser_free(parser);

    return (parser->error_count > 0) ? 1 : 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/lexer/lexer.h"
#include "src/parser/parser.h"
#include "src/semantic/semantic.h"

/* ============================================================================
   OPÇÕES DE LINHA DE COMANDO
   ============================================================================ */

typedef struct {
    const char *input_file;
    int show_ast;       /* --ast      */
    int show_symbols;   /* --symbols  */
    int show_semantic;  /* --semantic */
    int debug;          /* --debug    */
} cli_options_t;

static void print_usage(const char *prog) {
    fprintf(stderr, "Uso: %s [opções] <arquivo.c>\n", prog);
    fprintf(stderr, "Opções:\n");
    fprintf(stderr, "  --ast       Imprimir a árvore sintática\n");
    fprintf(stderr, "  --symbols   Imprimir tabela de símbolos\n");
    fprintf(stderr, "  --semantic  Activar modo verboso do análise semântica\n");
    fprintf(stderr, "  --debug     Activar todos os logs de depuração\n");
}

static int parse_cli(int argc, char *argv[], cli_options_t *opts) {
    opts->input_file  = NULL;
    opts->show_ast    = 0;
    opts->show_symbols = 0;
    opts->show_semantic = 0;
    opts->debug       = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--ast") == 0) {
            opts->show_ast = 1;
        } else if (strcmp(argv[i], "--symbols") == 0) {
            opts->show_symbols = 1;
        } else if (strcmp(argv[i], "--semantic") == 0) {
            opts->show_semantic = 1;
        } else if (strcmp(argv[i], "--debug") == 0) {
            opts->debug = 1;
            opts->show_ast = 1;
            opts->show_symbols = 1;
            opts->show_semantic = 1;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "%s: opção desconhecida '%s'\n", argv[0], argv[i]);
            print_usage(argv[0]);
            return -1;
        } else {
            if (opts->input_file) {
                fprintf(stderr, "%s: apenas um ficheiro de entrada é suportado\n", argv[0]);
                return -1;
            }
            opts->input_file = argv[i];
        }
    }

    if (!opts->input_file) {
        print_usage(argv[0]);
        return -1;
    }
    return 0;
}

/* ============================================================================
   MAIN
   ============================================================================ */

int main(int argc, char *argv[]) {
    cli_options_t opts;
    if (parse_cli(argc, argv, &opts) != 0) {
        return 1;
    }

    /* --- Fase 1: Análise Léxica --- */
    tabla_simbolos_t *symbol_table = processar_arquivo_completo(opts.input_file);
    if (!symbol_table) {
        fprintf(stderr, "%s: erro: não foi possível processar '%s'\n",
                argv[0], opts.input_file);
        return 1;
    }

    /* --- Fase 2: Análise Sintática --- */
    parser_t *parser = parser_init(symbol_table);
    if (!parser) {
        fprintf(stderr, "%s: erro interno: falha ao inicializar parser\n", argv[0]);
        liberar_tabla_simbolos(symbol_table);
        return 1;
    }

    parser->source_path = opts.input_file;
    parser_run(parser);

    /* --- Saída opcional: AST --- */
    if (opts.show_ast && parser->ast_root) {
        fprintf(stderr, "\n=== AST ===\n");
        print_ast(parser->ast_root, 0);
    }

    /* --- Saída opcional: tabela de símbolos --- */
    if (opts.show_symbols) {
        fprintf(stderr, "\n=== TABELA DE SIMBOLOS ===\n");
        print_all_scopes(parser);
    }

    int errors = parser->error_count;

    /* --- Fase 3: Análise Semântica --- */
    if (errors == 0) {
        /* Só executar a análise semântica se o parser não encontrou erros.
           Erros sintácticos podem gerar uma AST incompleta que confundiria
           o analisador semântico e produziria erros espúreos. */
        semantic_t *sem = semantic_init(parser);
        if (!sem) {
            fprintf(stderr, "%s: erro interno: falha ao inicializar analisador semântico\n",
                    argv[0]);
        } else {
            int sem_errors = semantic_run(sem);

            if (opts.show_semantic && sem_errors == 0) {
                fprintf(stderr, "\n=== ANÁLISE SEMÂNTICA: sem erros ===\n");
            }

            errors += sem_errors;
            semantic_free(sem);
        }
    }

    /* Limpeza */
    parser_free(parser);
    /* O parser NÃO é dono do token stream (apenas o referencia). A posse fica em
       main, por isso libertamo-lo aqui, depois de o parser deixar de o usar. */
    liberar_tabla_simbolos(symbol_table);

    /* Saída de erro final — apenas se houver erros (silencioso em sucesso) */
    if (errors > 0) {
        fprintf(stderr, "%s: %d erro(s) encontrado(s)\n", opts.input_file, errors);
    }

    return (errors > 0) ? 1 : 0;
}

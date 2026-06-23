#include "parser.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================================
   ANALISADOR SINTÁTICO (PARSER) - IMPLEMENTAÇÃO
   ============================================================================ */

/* ============================================================================
   FUNÇÕES DE INICIALIZAÇÃO E LIMPEZA
   ============================================================================ */

static int register_scope(parser_t *parser, scope_t *scope) {
    if (!parser || !scope) {
        return 0;
    }

    if (parser->total_scopes >= 1024) {
#ifdef DEBUG_PARSER
        fprintf(stderr, "Erro: limite de escopos atingido\n");
#endif
        return 0;
    }

    parser->all_scopes[parser->total_scopes++] = scope;
    return 1;
}

parser_t* parser_init(tabla_simbolos_t *symbol_table) {
    if (!symbol_table) {
        return NULL;
    }
    
    parser_t *parser = (parser_t*)malloc(sizeof(parser_t));
    if (!parser) {
        return NULL;
    }
    
    parser->token_stream = symbol_table;
    parser->current_position = 0;
    
    parser->total_scopes = 0;

    /* Initialize GLOBAL scope (scope_id = 0) */
    parser->global_scope = scope_create(0, -1, 0);
    if (!parser->global_scope) {
        free(parser);
        return NULL;
    }

    parser->global_symbol_table = parser->global_scope;
    parser->current_scope = parser->global_scope;
    register_scope(parser, parser->global_scope);
    
    /* Initialize LOCAL symbol table (initially NULL) */
    parser->current_local_table = NULL;
    
    /* Initialize scope stack */
    parser->scope_stack_capacity = 16;
    parser->scope_stack = (scope_t**)malloc(sizeof(scope_t*) * parser->scope_stack_capacity);
    if (!parser->scope_stack) {
        scope_free(parser->global_scope);
        free(parser);
        return NULL;
    }
    parser->scope_stack_size = 0;
    parser->next_scope_id = 1;  /* 0 is reserved for global */
    
    /* Initialize memory allocation */
    parser->next_global_address = 0;
    parser->next_local_offset = 0;
    
    /* Initialize error handling */
    parser->error_count = 0;
    parser->panic_mode = 0;
    
    /* Initialize AST */
    parser->ast_root = NULL;
    parser->source_path = NULL;
    
    return parser;
}

void parser_free(parser_t *parser) {
    if (!parser) {
        return;
    }
    
    /* Free all registered scopes exactly once */
    for (int i = 0; i < parser->total_scopes; i++) {
        scope_free(parser->all_scopes[i]);
    }
    free(parser->scope_stack);
    
    /* Free AST */
    if (parser->ast_root) {
        free_ast(parser->ast_root);
    }
    
    free(parser);
}

int parser_run(parser_t *parser) {
    if (!parser) {
        return -1;
    }
    
    /* Parse program — no debug output by default */
    parser->ast_root = parse_programa(parser);
    
    return parser->error_count;
}

/* ============================================================================
   FUNÇÕES DE CONSUMO DE TOKENS
   ============================================================================ */

token_t peek_token(parser_t *parser) {
    if (parser->current_position >= parser->token_stream->quantidade) {
        token_t eof_token;
        eof_token.tipo = TK_EOF;
        strcpy(eof_token.lexeme, "EOF");
        eof_token.linha = 0;
        eof_token.coluna = 0;
        return eof_token;
    }
    
    return parser->token_stream->tokens[parser->current_position];
}

token_t consume_token(parser_t *parser) {
    token_t token = peek_token(parser);
    if (parser->current_position < parser->token_stream->quantidade) {
        parser->current_position++;
    }
    return token;
}

int match(parser_t *parser, int token_type) {
    return peek_token(parser).tipo == token_type;
}

void expect(parser_t *parser, int token_type) {
    if (match(parser, token_type)) {
        consume_token(parser);
    } else {
        token_t found = peek_token(parser);
        syntax_error(parser, "token inesperado", token_type, found);
    }
}

/* ============================================================================
   FUNÇÕES DE GERENCIAMENTO DE TABELAS DE SÍMBOLOS
   ============================================================================ */

symbol_info_t* lookup_symbol(parser_t *parser, const char *name) {
    for (int i = parser->scope_stack_size - 1; i >= 0; i--) {
        symbol_info_t *info = (symbol_info_t*)scope_lookup_symbol(parser->scope_stack[i], name);
        if (info) {
            return info;
        }
    }
    return (symbol_info_t*)scope_lookup_symbol(parser->global_scope, name);
}

symbol_info_t* lookup_global_symbol(parser_t *parser, const char *name) {
    return (symbol_info_t*)scope_lookup_symbol(parser->global_scope, name);
}

symbol_info_t* lookup_local_symbol(parser_t *parser, const char *name) {
    if (!parser->current_local_table) {
        return NULL;
    }
    return (symbol_info_t*)scope_lookup_symbol(parser->current_local_table, name);
}

int add_global_symbol(parser_t *parser, const char *name, symbol_info_t *info) {
    return scope_add_symbol(parser->global_scope, name, info);
}

int add_local_symbol(parser_t *parser, const char *name, symbol_info_t *info) {
    if (!parser->current_local_table) {
#ifdef DEBUG_PARSER
        fprintf(stderr, "Erro: tentativa de adicionar simbolo local sem escopo ativo\n");
#endif
        return 0;
    }
    return scope_add_symbol(parser->current_local_table, name, info);
}

void enter_local_scope(parser_t *parser) {
    int scope_id = parser->next_scope_id++;
    int parent_id = parser->current_scope ? parser->current_scope->scope_id : 0;
    
    scope_t *new_scope = scope_create(scope_id, parent_id, parser->next_local_offset);
    if (!new_scope) {
#ifdef DEBUG_PARSER
        fprintf(stderr, "Erro: falha ao criar novo escopo local\n");
#endif
        return;
    }
    
    if (parser->scope_stack_size >= parser->scope_stack_capacity) {
        parser->scope_stack_capacity *= 2;
        scope_t **new_stack = (scope_t**)realloc(parser->scope_stack,
                                                  sizeof(scope_t*) * parser->scope_stack_capacity);
        if (!new_stack) {
            scope_free(new_scope);
            return;
        }
        parser->scope_stack = new_stack;
    }
    
    parser->scope_stack[parser->scope_stack_size++] = new_scope;
    if (!register_scope(parser, new_scope)) {
        parser->scope_stack_size--;
        scope_free(new_scope);
        return;
    }
    parser->current_local_table = new_scope;
    parser->current_scope = new_scope;
}

void exit_local_scope(parser_t *parser) {
    if (parser->scope_stack_size == 0) {
#ifdef DEBUG_PARSER
        fprintf(stderr, "Erro: tentativa de sair de escopo sem escopo ativo\n");
#endif
        return;
    }
    
    parser->scope_stack_size--;
    
    if (parser->scope_stack_size > 0) {
        parser->current_local_table = parser->scope_stack[parser->scope_stack_size - 1];
        parser->current_scope = parser->current_local_table;
    } else {
        parser->current_local_table = NULL;
        parser->current_scope = parser->global_scope;
    }
}

/* ============================================================================
   FUNÇÕES DE ENRIQUECIMENTO DA TABELA DE SÍMBOLOS
   ============================================================================ */

void enrich_symbol_type(parser_t *parser, const char *name, int data_type, int is_pointer) {
    symbol_info_t *info = lookup_symbol(parser, name);
    if (!info) return;
    info->data_type = data_type;
    info->is_pointer = is_pointer;
}

void enrich_symbol_scope(parser_t *parser, const char *name, int scope_id, variable_type_t var_type) {
    symbol_info_t *info = lookup_symbol(parser, name);
    if (!info) return;
    info->scope_id = scope_id;
    info->variable_type = var_type;
}

void enrich_symbol_memory(parser_t *parser, const char *name, int address, int size) {
    symbol_info_t *info = lookup_symbol(parser, name);
    if (!info) return;
    info->memory_address = address;
    info->size_bytes = size;
}

void enrich_symbol_array(parser_t *parser, const char *name, int *dimensions, int dim_count) {
    symbol_info_t *info = lookup_symbol(parser, name);
    if (!info) return;
    info->is_array = (dim_count > 0);
    info->array_dim_count = dim_count;
    for (int i = 0; i < dim_count && i < 8; i++) {
        info->array_dimensions[i] = dimensions[i];
    }
}

void enrich_symbol_function(parser_t *parser, const char *name, int return_type,
                            int *param_types, int param_count) {
    symbol_info_t *info = lookup_symbol(parser, name);
    if (!info) return;
    info->is_function = 1;
    info->function_return_type = return_type;
    info->function_param_count = param_count;
    for (int i = 0; i < param_count && i < 32; i++) {
        info->function_param_types[i] = param_types[i];
    }
}

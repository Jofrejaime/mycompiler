#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Forward declarations */
void parse_campos_struct_union(parser_t *parser, ast_node_t *parent, int *total_size);
ast_node_t* parse_declaracao_typedef(parser_t *parser);
ast_node_t* parse_declaracao_union(parser_t *parser);
static void attach_struct_fields(symbol_info_t *info, ast_node_t *struct_node);

/*
   Parse pointer declarators: * or ** or ***
   Returns number of pointer levels (0 if no pointers)
*/
int parse_asteriscos(parser_t *parser) {
    int pointer_count = 0;
    while (match(parser, OP_MULT)) {
        consume_token(parser);
        pointer_count++;
    }
    return pointer_count;
}

/*
   Parse array dimensions: [expr] or [expr][expr]...
   Stores the integer constant value when the size is a literal;
   stores 0 for empty brackets or non-constant expressions.
   Returns number of dimensions.
*/
int parse_sufixo_array(parser_t *parser, int *dimensions) {
    int dim_count = 0;

    while (match(parser, SYM_LBRACKET)) {
        consume_token(parser);  /* Consume '[' */

        if (match(parser, SYM_RBRACKET)) {
            /* Empty brackets [] — unknown/flexible size */
            dimensions[dim_count++] = 0;
        } else if (match(parser, TK_NUM_INT)) {
            /* Constant integer dimension — capture the value */
            token_t size_tok = peek_token(parser);
            dimensions[dim_count++] = size_tok.valor.valor_int
                                      ? size_tok.valor.valor_int
                                      : (int)strtol(size_tok.lexeme, NULL, 10);
            consume_token(parser);
            /* There may still be more tokens before ']' (e.g. constant expr).
               Consume them so the expect below finds ']'. */
            while (!match(parser, SYM_RBRACKET) && !match(parser, TK_EOF)) {
                consume_token(parser);
            }
        } else {
            /* Non-constant expression — parse it but record size as 0 */
            parse_expressao(parser);
            dimensions[dim_count++] = 0;
        }

        expect(parser, SYM_RBRACKET);  /* Consume ']' */
    }
    return dim_count;
}

/*
   Parse initialization: either expression or initializer list
*/
ast_node_t* parse_inicializacao(parser_t *parser) {
    /* Recursive initializer:
       initializer := '{' (initializer (',' initializer)* ','?)? '}' | expressao

       This supports nested braces for multidimensional arrays.
    */
    if (match(parser, SYM_LBRACE)) {
        token_t token = peek_token(parser);
        consume_token(parser);  /* Consume '{' */

        ast_node_t *init_list = create_ast_node(AST_INIT_LIST, NULL, token.linha, token.coluna);

        while (!match(parser, SYM_RBRACE) && !match(parser, TK_EOF)) {
            ast_node_t *item = parse_inicializacao(parser);
            if (item) {
                add_ast_child(init_list, item);
            }

            if (match(parser, SYM_COMMA)) {
                consume_token(parser);
                /* Allow trailing comma */
                if (match(parser, SYM_RBRACE)) {
                    break;
                }
                continue;
            }
            break;
        }

        expect(parser, SYM_RBRACE);
        return init_list;
    }

    return parse_expressao(parser);
}

/* ============================================================================
   ESPECIFICADOR DE TIPO
   ============================================================================ */

/* Forward helper: typedef name check (implemented below) */
static int is_typedef_name(parser_t *parser, const char *name);

/*
   EspecificadorTipo → KW_INT | KW_CHAR | KW_FLOAT | KW_VOID | KW_DOUBLE
                     | KW_STRUCT [TK_ID] [corpo_campos]
                     | KW_UNION [TK_ID] [corpo_campos]
                     | TK_ID (typedef'd type)

   Extended variant: if tag_name_out != NULL and out_len > 0, the struct/union
   tag name is written there (e.g. "Ponto" for "struct Ponto").
*/
int parse_especificador_tipo_ex(parser_t *parser, char *tag_name_out, size_t out_len) {
    token_t token = peek_token(parser);
    
    switch (token.tipo) {
        case KW_INT:
        case KW_CHAR:
        case KW_FLOAT:
        case KW_VOID:
        case KW_DOUBLE:
            consume_token(parser);
            return token.tipo;
        
        case KW_STRUCT: {
            consume_token(parser);  // Consume 'struct'
            if (match(parser, TK_ID)) {
                token_t tag_tok = peek_token(parser);
                if (tag_name_out && out_len > 0) {
                    strncpy(tag_name_out, tag_tok.lexeme, out_len - 1);
                    tag_name_out[out_len - 1] = '\0';
                }
                expect(parser, TK_ID);  // Consume struct tag name
            }
            /* Note: Could be struct {...} but that's handled elsewhere */
            return KW_STRUCT;
        }
        
        case KW_UNION: {
            consume_token(parser);  // Consume 'union'
            if (match(parser, TK_ID)) {
                token_t tag_tok = peek_token(parser);
                if (tag_name_out && out_len > 0) {
                    strncpy(tag_name_out, tag_tok.lexeme, out_len - 1);
                    tag_name_out[out_len - 1] = '\0';
                }
                expect(parser, TK_ID);  // Consume union tag name
            }
            return KW_UNION;
        }
        
        case TK_ID:
            /* Only accept TK_ID here when it's a known typedef name. */
            if (!is_typedef_name(parser, token.lexeme)) {
                syntax_error(parser, "especificador de tipo esperado (typedef desconhecido)", -1, token);
                return KW_INT;  /* Default fallback */
            }
            if (tag_name_out && out_len > 0) {
                strncpy(tag_name_out, token.lexeme, out_len - 1);
                tag_name_out[out_len - 1] = '\0';
            }
            consume_token(parser);
            return TK_ID;
        
        default:
            syntax_error(parser, "especificador de tipo esperado", -1, token);
            return KW_INT;  /* Default fallback */
    }
}

/* Backward-compatible wrapper: no tag capture */
int parse_especificador_tipo(parser_t *parser) {
    return parse_especificador_tipo_ex(parser, NULL, 0);
}

/*
   Verificar se token é especificador de tipo
*/
static int is_typedef_name(parser_t *parser, const char *name) {
    if (!parser || !name) {
        return 0;
    }

    symbol_info_t *info = lookup_global_symbol(parser, name);
    return (info && info->is_typedef);
}

int is_type_specifier(parser_t *parser, token_t token) {
    if (token.tipo == KW_INT ||
        token.tipo == KW_CHAR ||
        token.tipo == KW_FLOAT ||
        token.tipo == KW_VOID ||
        token.tipo == KW_DOUBLE ||
        token.tipo == KW_STRUCT ||
        token.tipo == KW_UNION ||
        token.tipo == KW_STATIC) {
        return 1;
    }

    /* TK_ID sÃ³ Ã© tipo se for um nome de typedef conhecido. */
    if (token.tipo == TK_ID) {
        return is_typedef_name(parser, token.lexeme);
    }

    return 0;
}

/* ============================================================================
   DECLARAÇÃO DE VARIÁVEL GLOBAL
   ============================================================================ */

/*
   DeclaracaoVariavelGlobal → EspecificadorTipo Declarator (SYM_COMMA Declarator)* SYM_SEMICOLON
   Declarator → [Asteriscos] TK_ID (SYM_LBRACKET Expressao? SYM_RBRACKET)* (OP_ASSIGN Expressao)?
*/
ast_node_t* parse_declaracao_variavel_global(parser_t *parser) {
    char struct_tag_buf[64] = "";
    int data_type = parse_especificador_tipo_ex(parser, struct_tag_buf, sizeof(struct_tag_buf));

    ast_node_t *first_decl = NULL;  /* First VAR_DECL (returned directly when only one) */
    ast_node_t *wrapper    = NULL;  /* AST_BLOCK wrapper, created on second declarator */
    int decl_count = 0;

    do {
        /* Each declarator may have its own pointer stars: int *x, y; */
        int pointer_level = parse_asteriscos(parser);

        token_t id_token = peek_token(parser);
        expect(parser, TK_ID);  // Consume identifier

        ast_node_t *var_decl = create_ast_node(AST_VAR_DECL, id_token.lexeme,
                                               id_token.linha, id_token.coluna);
        var_decl->data_type    = data_type;
        var_decl->operator_type = pointer_level;

        /* Array dimensions per declarator */
        int dimensions[8] = {0};
        int dim_count = parse_sufixo_array(parser, dimensions);
        int is_array  = (dim_count > 0);

        var_decl->data.decl.array_dim_count = dim_count;
        for (int i = 0; i < dim_count; i++)
            var_decl->data.decl.array_dimensions[i] = dimensions[i];

        /* Optional initializer — supports both scalar and brace-enclosed lists */
        if (match(parser, OP_ASSIGN)) {
            consume_token(parser);  // Consume '='
            ast_node_t *init_expr = parse_inicializacao(parser);
            var_decl->data.decl.initializer = init_expr;
        }

        /* Symbol table registration */
        symbol_info_t *info = (symbol_info_t*)calloc(1, sizeof(symbol_info_t));
        info->data_type     = data_type;
        info->is_pointer    = pointer_level;
        info->kind          = SYMBOL_VARIABLE;
        info->variable_type = VAR_GLOBAL;
        info->scope_id      = 0;
        info->is_array      = is_array;
        info->array_dim_count = dim_count;
        for (int i = 0; i < dim_count; i++)
            info->array_dimensions[i] = dimensions[i];

        /* Store struct/union tag name or resolve typedef struct/union tag name */
        if ((data_type == KW_STRUCT || data_type == KW_UNION) && struct_tag_buf[0] != '\0') {
            snprintf(info->struct_tag_name, sizeof(info->struct_tag_name), "%s", struct_tag_buf);
        } else if (data_type == TK_ID && struct_tag_buf[0] != '\0') {
            symbol_info_t *tinfo = lookup_global_symbol(parser, struct_tag_buf);
            if (tinfo && tinfo->kind == SYMBOL_TYPEDEF &&
                (tinfo->data_type == KW_STRUCT || tinfo->data_type == KW_UNION)) {
                snprintf(info->struct_tag_name, sizeof(info->struct_tag_name), "%s", tinfo->typedef_base_name);
            }
        }

        /* Calculate exact base type size (resolving structs and typedefs) */
        int base_size = get_base_type_size(data_type, pointer_level);
        if (pointer_level == 0) {
            if ((data_type == KW_STRUCT || data_type == KW_UNION) && struct_tag_buf[0] != '\0') {
                char skey[264];
                snprintf(skey, sizeof(skey), "%s:%s",
                         (data_type == KW_STRUCT) ? "struct" : "union",
                         struct_tag_buf);
                symbol_info_t *sinfo = lookup_global_symbol(parser, skey);
                if (sinfo) base_size = sinfo->size_bytes;
            } else if (data_type == TK_ID && struct_tag_buf[0] != '\0') {
                symbol_info_t *tinfo = lookup_global_symbol(parser, struct_tag_buf);
                if (tinfo && tinfo->kind == SYMBOL_TYPEDEF) {
                    if (tinfo->is_pointer > 0) {
                        base_size = 8;
                    } else if (tinfo->data_type == KW_STRUCT || tinfo->data_type == KW_UNION) {
                        char skey[264];
                        snprintf(skey, sizeof(skey), "%s:%s",
                                 (tinfo->data_type == KW_STRUCT) ? "struct" : "union",
                                 tinfo->typedef_base_name);
                        symbol_info_t *sinfo = lookup_global_symbol(parser, skey);
                        if (sinfo) base_size = sinfo->size_bytes;
                    } else {
                        base_size = get_base_type_size(tinfo->data_type, 0);
                    }
                }
            }
        }

        int total_size = base_size;
        for (int i = 0; i < dim_count; i++) {
            if (dimensions[i] > 0) total_size *= dimensions[i];
        }

        info->size_bytes = total_size;

        info->memory_address = parser->next_global_address;
        parser->next_global_address += total_size;

        if (!add_global_symbol(parser, id_token.lexeme, info)) {
            report_redeclaration(parser, id_token.lexeme, id_token);
        }
        enrich_symbol_type(parser, id_token.lexeme, data_type, pointer_level);
        enrich_symbol_scope(parser, id_token.lexeme, 0, VAR_GLOBAL);
        enrich_symbol_memory(parser, id_token.lexeme, info->memory_address, total_size);
        if (is_array)
            enrich_symbol_array(parser, id_token.lexeme, dimensions, dim_count);
        /* Store literal constant value if initializer is a simple literal */
        if (var_decl->data.decl.initializer)
            try_set_const_init(info, var_decl->data.decl.initializer);


        /* Accumulate nodes */
        if (decl_count == 0) {
            first_decl = var_decl;
        } else {
            if (decl_count == 1) {
                wrapper = create_ast_node(AST_BLOCK, NULL,
                                          first_decl->line, first_decl->column);
                add_ast_child(wrapper, first_decl);
            }
            add_ast_child(wrapper, var_decl);
        }
        decl_count++;

        /* Continue if comma separates declarators */
        if (match(parser, SYM_COMMA)) {
            consume_token(parser);  // Consume ','
        } else {
            break;
        }
    } while (1);

    expect(parser, SYM_SEMICOLON);  // Consume ';'
    return (wrapper != NULL) ? wrapper : first_decl;
}

    /* ============================================================================
       LISTA DE PARÂMETROS DE FUNÇÃO
       ============================================================================ */


/*
   ListaParametros → Parametro (SYM_COMMA Parametro)*
                   | KW_VOID
                   | ε
   Parametro → EspecificadorTipo [Asteriscos] [TK_ID [SufixoArray]]
*/
void parse_lista_parametros(parser_t *parser, ast_node_t *func_node, 
                            int *param_types, int *param_count) {
    *param_count = 0;
    
    /* Empty parameter list */
    if (match(parser, SYM_RPAREN)) {
        return;
    }
    
    /* void parameter list: void alone means no parameters */
    if (match(parser, KW_VOID)) {
        /* Peek ahead: if next is ')' it's "(void)" meaning no params */
        int save = parser->current_position;
        consume_token(parser);
        if (match(parser, SYM_RPAREN)) {
            return;  /* void — no parameters */
        }
        /* Otherwise treat 'void' as a parameter type — restore */
        parser->current_position = save;
    }
    
    /* Parse parameters */
    do {
        /* Variadic '...' — stop parameter parsing */
        if (match(parser, SYM_DOT)) {
            /* Consume '...' (three dots arrive as separate SYM_DOT tokens
               from the lexer; consume all three) */
            consume_token(parser);
            if (match(parser, SYM_DOT)) consume_token(parser);
            if (match(parser, SYM_DOT)) consume_token(parser);
            break;
        }

        char struct_tag_buf[64] = "";
        int param_type = parse_especificador_tipo_ex(parser, struct_tag_buf, sizeof(struct_tag_buf));
        int pointer_level = parse_asteriscos(parser);

        /* Name is optional (prototype without parameter names) */
        char param_name_buf[256] = "";
        int  has_name = 0;
        token_t param_name_tok = peek_token(parser);

        if (match(parser, TK_ID)) {
            strncpy(param_name_buf, param_name_tok.lexeme,
                    sizeof(param_name_buf) - 1);
            param_name_buf[sizeof(param_name_buf) - 1] = '\0';
            has_name = 1;
            consume_token(parser);
        }

        /* Check for array parameter */
        int dimensions[8] = {0};
        int dim_count = parse_sufixo_array(parser, dimensions);
        int is_array = (dim_count > 0);
        
        /* Create parameter node (name may be NULL for anonymous params) */
        ast_node_t *param = create_ast_node(
            AST_PARAM_DECL,
            has_name ? param_name_buf : NULL,
            param_name_tok.linha, param_name_tok.coluna);
        param->data_type    = param_type;
        param->operator_type = pointer_level;
        add_ast_child(func_node, param);
        
        /* Add to parameter types array */
        if (*param_count < 32) {
            param_types[*param_count] = param_type;
            (*param_count)++;
        }
        
        /* Only register named parameters in the local symbol table */
        if (has_name && parser->current_local_table) {
            symbol_info_t *param_info = (symbol_info_t*)calloc(1, sizeof(symbol_info_t));
            param_info->data_type     = param_type;
            param_info->is_pointer    = pointer_level;
            param_info->kind          = SYMBOL_PARAMETER;
            param_info->variable_type = VAR_PARAMETER;
            param_info->scope_id      = parser->current_local_table->scope_id;
            param_info->is_array      = is_array;

            /* Store struct/union tag name or resolve typedef struct/union tag name */
            if ((param_type == KW_STRUCT || param_type == KW_UNION) && struct_tag_buf[0] != '\0') {
                snprintf(param_info->struct_tag_name, sizeof(param_info->struct_tag_name), "%s", struct_tag_buf);
            } else if (param_type == TK_ID && struct_tag_buf[0] != '\0') {
                symbol_info_t *tinfo = lookup_global_symbol(parser, struct_tag_buf);
                if (tinfo && tinfo->kind == SYMBOL_TYPEDEF &&
                    (tinfo->data_type == KW_STRUCT || tinfo->data_type == KW_UNION)) {
                    snprintf(param_info->struct_tag_name, sizeof(param_info->struct_tag_name), "%s", tinfo->typedef_base_name);
                }
            }

            /* Calculate exact base type size (resolving structs and typedefs) */
            int base_size = get_base_type_size(param_type, pointer_level);
            if (pointer_level == 0) {
                if ((param_type == KW_STRUCT || param_type == KW_UNION) && struct_tag_buf[0] != '\0') {
                    char skey[264];
                    snprintf(skey, sizeof(skey), "%s:%s",
                             (param_type == KW_STRUCT) ? "struct" : "union",
                             struct_tag_buf);
                    symbol_info_t *sinfo = lookup_global_symbol(parser, skey);
                    if (sinfo) base_size = sinfo->size_bytes;
                } else if (param_type == TK_ID && struct_tag_buf[0] != '\0') {
                    symbol_info_t *tinfo = lookup_global_symbol(parser, struct_tag_buf);
                    if (tinfo && tinfo->kind == SYMBOL_TYPEDEF) {
                        if (tinfo->is_pointer > 0) {
                            base_size = 8;
                        } else if (tinfo->data_type == KW_STRUCT || tinfo->data_type == KW_UNION) {
                            char skey[264];
                            snprintf(skey, sizeof(skey), "%s:%s",
                                     (tinfo->data_type == KW_STRUCT) ? "struct" : "union",
                                     tinfo->typedef_base_name);
                            symbol_info_t *sinfo = lookup_global_symbol(parser, skey);
                            if (sinfo) base_size = sinfo->size_bytes;
                        } else {
                            base_size = get_base_type_size(tinfo->data_type, 0);
                        }
                    }
                }
            }

            int param_size = base_size;
            for (int i = 0; i < dim_count; i++) {
                if (dimensions[i] > 0) param_size *= dimensions[i];
            }
            param_info->size_bytes = param_size;

            param_info->memory_address =
                scope_allocate_memory(parser->current_local_table, param_info->size_bytes);
            add_local_symbol(parser, param_name_buf, param_info);
            enrich_symbol_type(parser, param_name_buf, param_type, pointer_level);
            enrich_symbol_scope(parser, param_name_buf, param_info->scope_id, VAR_PARAMETER);
            enrich_symbol_memory(parser, param_name_buf,
                                  param_info->memory_address, param_info->size_bytes);
            if (is_array)
                enrich_symbol_array(parser, param_name_buf, dimensions, dim_count);
        }
        
        /* Check for more parameters */
        if (match(parser, SYM_COMMA)) {
            consume_token(parser);
        } else {
            break;
        }
    } while (1);
}

/* ============================================================================
   DECLARAÇÃO DE FUNÇÃO
   ============================================================================ */

/*
   DeclaracaoFuncao → EspecificadorTipo TK_ID SYM_LPAREN ListaParametros SYM_RPAREN
                      (Bloco | SYM_SEMICOLON)
*/
ast_node_t* parse_declaracao_funcao(parser_t *parser, int return_type) {
    /* Parse pointer declarators for function return type (e.g., int* f()) */
    int pointer_level = parse_asteriscos(parser);

    token_t name_tok = peek_token(parser);
    expect(parser, TK_ID);  /* Consume function name */

    ast_node_t *func_decl = create_ast_node(AST_FUNC_DECL, name_tok.lexeme, name_tok.linha, name_tok.coluna);
    func_decl->data_type = return_type;
    func_decl->operator_type = pointer_level;  /* store pointer level */

    expect(parser, SYM_LPAREN);  /* Consume '(' */
    
    /* Enter function scope */
    enter_local_scope(parser);
    
    /* Parse parameters */
    int param_types[32];
    int param_count = 0;
    parse_lista_parametros(parser, func_decl, param_types, &param_count);
    
    expect(parser, SYM_RPAREN);  // Consume ')'
    
    /* Function body or forward declaration */
    if (match(parser, SYM_LBRACE)) {
        /* Function definition with body */
        ast_node_t *body = parse_bloco(parser);
        add_ast_child(func_decl, body);
    } else {
        /* Forward declaration */
        expect(parser, SYM_SEMICOLON);
    }
    
    /* Exit function scope */
    exit_local_scope(parser);
    
    /* Add function to global symbol table */
    symbol_info_t *func_info = (symbol_info_t*)calloc(1, sizeof(symbol_info_t));
    func_info->data_type = return_type;
    func_info->is_pointer = pointer_level;
    func_info->kind = SYMBOL_FUNCTION;
    func_info->variable_type = VAR_GLOBAL;  /* Functions are global */
    func_info->scope_id = 0;
    func_info->is_function = 1;
    func_info->function_return_type = return_type;
    func_info->function_param_count = param_count;
    for (int i = 0; i < param_count; i++) {
        func_info->function_param_types[i] = param_types[i];
    }
    func_info->size_bytes = 0;  /* Functions don't have size */
    func_info->memory_address = 0;  /* Functions have code addresses, not data */
    
    /* NOTA: não reportamos redeclaração de funções aqui — protótipo seguido de
       definição (mesmo nome duas vezes) é legal em C. Distinguir protótipo de
       definição e validar assinaturas pertence à fase de análise semântica. */
    add_global_symbol(parser, name_tok.lexeme, func_info);

    enrich_symbol_type(parser, name_tok.lexeme, return_type, pointer_level);
    enrich_symbol_scope(parser, name_tok.lexeme, 0, VAR_GLOBAL);
    enrich_symbol_function(parser, name_tok.lexeme, return_type, param_types, param_count);
    
    return func_decl;
} 

/* ============================================================================
   DECLARAÇÃO DE STRUCT
   ============================================================================ */

/*
   DeclaracaoStruct → KW_STRUCT [TK_ID] [SYM_LBRACE <campos> SYM_RBRACE] SYM_SEMICOLON
*/
ast_node_t* parse_declaracao_struct(parser_t *parser) {
    expect(parser, KW_STRUCT);  // Consume 'struct'
    
    token_t struct_name_token = {0};
    char *struct_name = "(anonymous)";
    
    if (match(parser, TK_ID)) {
        struct_name_token = peek_token(parser);
        struct_name = struct_name_token.lexeme;
        consume_token(parser);
    }
    
    ast_node_t *struct_decl = create_ast_node(AST_STRUCT_DECL, struct_name, 0, 0);
    if (struct_name_token.linha) {
        struct_decl->line = struct_name_token.linha;
        struct_decl->column = struct_name_token.coluna;
    }
    
    if (match(parser, SYM_LBRACE)) {
        expect(parser, SYM_LBRACE);  // Consume '{'
        
        int total_size = 0;
        parse_campos_struct_union(parser, struct_decl, &total_size);
        
        expect(parser, SYM_RBRACE);  // Consume '}'
        expect(parser, SYM_SEMICOLON);  // Consume ';'
        
        /* Add struct to global symbol table with "struct:" prefix to avoid
           namespace collision with typedef aliases (e.g., typedef struct Ponto Ponto) */
        char struct_key[264];
        snprintf(struct_key, sizeof(struct_key), "struct:%s", struct_name);
        
        symbol_info_t *existing = lookup_global_symbol(parser, struct_key);
        if (existing) {
            existing->size_bytes = total_size;
            if (existing->fields) {
                free(existing->fields);
                existing->fields = NULL;
            }
            attach_struct_fields(existing, struct_decl);
            existing->is_struct_or_union = 1;
        } else {
            symbol_info_t *struct_info = (symbol_info_t*)calloc(1, sizeof(symbol_info_t));
            struct_info->data_type = KW_STRUCT;
            struct_info->kind = SYMBOL_STRUCT;
            struct_info->variable_type = VAR_GLOBAL;
            struct_info->scope_id = 0;
            struct_info->size_bytes = total_size;
            struct_info->is_struct_or_union = 1;
            
            attach_struct_fields(struct_info, struct_decl);
            add_global_symbol(parser, struct_key, struct_info);
        }
    } else {
        /* Forward declaration or reference */
        expect(parser, SYM_SEMICOLON);
    }
    
    return struct_decl;
}

/* ============================================================================
   HELPER: Attach field table to a struct/union symbol_info_t from its AST children
   ============================================================================ */

static void attach_struct_fields(symbol_info_t *info, ast_node_t *struct_node) {
    if (!info || !struct_node) return;

    /* Count VAR_DECL children */
    int count = 0;
    for (int i = 0; i < struct_node->child_count; i++) {
        if (struct_node->children[i] &&
            struct_node->children[i]->type == AST_VAR_DECL) {
            count++;
        }
    }
    if (count == 0) return;

    struct_field_t *fields = (struct_field_t*)calloc(count, sizeof(struct_field_t));
    if (!fields) return;

    int offset = 0;
    int idx = 0;
    for (int i = 0; i < struct_node->child_count; i++) {
        ast_node_t *m = struct_node->children[i];
        if (!m || m->type != AST_VAR_DECL) continue;

        strncpy(fields[idx].name,
                m->data.decl.name ? m->data.decl.name : "",
                sizeof(fields[idx].name) - 1);
        fields[idx].data_type  = m->data_type;
        fields[idx].is_pointer = m->operator_type;
        fields[idx].is_array   = (m->data.decl.array_dim_count > 0);
        fields[idx].array_dim_count = m->data.decl.array_dim_count;
        for (int d = 0; d < m->data.decl.array_dim_count && d < 8; d++)
            fields[idx].array_dimensions[d] = m->data.decl.array_dimensions[d];

        int sz = calculate_total_size(m->data_type, m->operator_type,
                                      m->data.decl.array_dimensions,
                                      m->data.decl.array_dim_count);
        fields[idx].size_bytes = sz;
        fields[idx].offset     = offset;
        offset += sz;
        idx++;
    }

    info->fields             = fields;
    info->field_count        = idx;
    info->is_struct_or_union = 1;
}

void parse_campos_struct_union(parser_t *parser, ast_node_t *parent, int *total_size) {
    /* Temporary storage for fields before we know the struct symbol name.
       Fields are collected here and attached to the symbol_info_t after
       parse_declaracao_struct / parse_declaracao_union calls add_global_symbol. */
    while (!match(parser, SYM_RBRACE) && !match(parser, TK_EOF)) {
        int member_type = parse_especificador_tipo(parser);

        /* Support multiple declarators per field: int x, *y; */
        do {
            int pointer_level = parse_asteriscos(parser);

            token_t member_name = peek_token(parser);
            expect(parser, TK_ID);

            /* Parse array dimensions */
            int dimensions[8];
            int dim_count = parse_sufixo_array(parser, dimensions);

            /* VALIDAÇÃO: Bloquear inicializadores em campos de struct */
            if (match(parser, OP_ASSIGN)) {
                token_t assign_token = peek_token(parser);
                syntax_error(parser,
                            "Erro: inicializadores nao permitidos em campos de struct/union (C puro nao permite)",
                            SYM_SEMICOLON, assign_token);
                while (!match(parser, SYM_SEMICOLON) && !match(parser, TK_EOF)) {
                    consume_token(parser);
                }
            }

            /* Create member AST node */
            ast_node_t *member = create_ast_node(AST_VAR_DECL, member_name.lexeme,
                                                member_name.linha, member_name.coluna);
            member->data_type = member_type;
            member->operator_type = pointer_level;
            member->data.decl.array_dim_count = dim_count;
            for (int i = 0; i < dim_count; i++)
                member->data.decl.array_dimensions[i] = dimensions[i];
            add_ast_child(parent, member);

            int member_size = calculate_total_size(member_type, pointer_level, dimensions, dim_count);
            *total_size += member_size;

            if (!match(parser, SYM_COMMA)) break;
            consume_token(parser);  /* consume ',' between declarators */
        } while (1);

        expect(parser, SYM_SEMICOLON);
    }
}

/* ============================================================================
   DECLARAÇÃO DE UNION
   ============================================================================ */

/*
   DeclaracaoUnion → KW_UNION [TK_ID] [SYM_LBRACE <campos> SYM_RBRACE] SYM_SEMICOLON
*/
ast_node_t* parse_declaracao_union(parser_t *parser) {
    expect(parser, KW_UNION);  // Consume 'union'
    
    token_t union_name_token = {0};
    char *union_name = "(anonymous)";
    
    if (match(parser, TK_ID)) {
        union_name_token = peek_token(parser);
        union_name = union_name_token.lexeme;
        consume_token(parser);
    }
    
    /* Create union node with generic name for now */
    ast_node_t *union_decl = create_ast_node(AST_STRUCT_DECL, union_name, 0, 0);
    if (union_name_token.linha) {
        union_decl->line = union_name_token.linha;
        union_decl->column = union_name_token.coluna;
    }
    
    if (match(parser, SYM_LBRACE)) {
        expect(parser, SYM_LBRACE);  // Consume '{'
        
        int max_size = 0;  /* Union size is max of all members */
        
        while (!match(parser, SYM_RBRACE) && !match(parser, TK_EOF)) {
            int member_type = parse_especificador_tipo(parser);
            int pointer_level = parse_asteriscos(parser);
            
            token_t member_name = peek_token(parser);
            expect(parser, TK_ID);
            
            /* Parse array dimensions */
            int dimensions[8];
            int dim_count = parse_sufixo_array(parser, dimensions);
            
            /* VALIDAÇÃO: Bloquear inicializadores em campos de union */
            if (match(parser, OP_ASSIGN)) {
                token_t assign_token = peek_token(parser);
                syntax_error(parser, 
                            "Erro: inicializadores nao permitidos em campos de struct/union (C puro nao permite)",
                            SYM_SEMICOLON, assign_token);
                /* Panic recovery: skip até semicolon */
                while (!match(parser, SYM_SEMICOLON) && !match(parser, TK_EOF)) {
                    consume_token(parser);
                }
            }
            
            expect(parser, SYM_SEMICOLON);
            
            /* Create member node */
            ast_node_t *member = create_ast_node(AST_VAR_DECL, member_name.lexeme,
                                                member_name.linha, member_name.coluna);
            member->data_type = member_type;
            member->operator_type = pointer_level;
            member->data.decl.array_dim_count = dim_count;
            for (int i = 0; i < dim_count; i++) {
                member->data.decl.array_dimensions[i] = dimensions[i];
            }
            add_ast_child(union_decl, member);
            
            /* Calculate size using helper function */
            int member_size = calculate_total_size(member_type, pointer_level, dimensions, dim_count);
            
            max_size = (member_size > max_size) ? member_size : max_size;
        }
        
        expect(parser, SYM_RBRACE);  // Consume '}'
        expect(parser, SYM_SEMICOLON);  // Consume ';'
        
        /* Add union to global symbol table with "union:" prefix */
        char union_key[264];
        snprintf(union_key, sizeof(union_key), "union:%s", union_name);
        
        symbol_info_t *existing = lookup_global_symbol(parser, union_key);
        if (existing) {
            existing->size_bytes = max_size;
            if (existing->fields) {
                free(existing->fields);
                existing->fields = NULL;
            }
            attach_struct_fields(existing, union_decl);
            existing->is_struct_or_union = 1;
        } else {
            symbol_info_t *union_info = (symbol_info_t*)calloc(1, sizeof(symbol_info_t));
            union_info->data_type = KW_UNION;
            union_info->kind = SYMBOL_UNION;
            union_info->variable_type = VAR_GLOBAL;
            union_info->scope_id = 0;
            union_info->size_bytes = max_size;
            union_info->is_struct_or_union = 1;
            
            attach_struct_fields(union_info, union_decl);
            add_global_symbol(parser, union_key, union_info);
        }
    } else {
        /* Forward declaration or reference */
        expect(parser, SYM_SEMICOLON);
    }
    
    return union_decl;
}

/* ============================================================================
   DECLARAÇÃO DE TYPEDEF
   ============================================================================ */

/*
   DeclaracaoTypedef → KW_TYPEDEF EspecificadorTipo [Asteriscos] TK_ID 
                       [SYM_LBRACKET Expressao SYM_RBRACKET]* SYM_SEMICOLON
*/
ast_node_t* parse_declaracao_typedef(parser_t *parser) {
    expect(parser, KW_TYPEDEF);  // Consume 'typedef'
    
    int base_type = KW_INT;
    ast_node_t *type_node = NULL;

    if (match(parser, KW_STRUCT) || match(parser, KW_UNION)) {
        token_t type_token = peek_token(parser);
        int is_union = match(parser, KW_UNION);
        base_type = is_union ? KW_UNION : KW_STRUCT;
        consume_token(parser);

        token_t tag_token = {0};
        char *tag_name = "(anonymous)";
        if (match(parser, TK_ID)) {
            tag_token = peek_token(parser);
            tag_name = tag_token.lexeme;
            consume_token(parser);
        }

        type_node = create_ast_node(AST_STRUCT_DECL, tag_name, type_token.linha, type_token.coluna);
        if (tag_token.linha) {
            type_node->line = tag_token.linha;
            type_node->column = tag_token.coluna;
        }

        if (match(parser, SYM_LBRACE)) {
            int temp_size = 0;
            expect(parser, SYM_LBRACE);
            parse_campos_struct_union(parser, type_node, &temp_size);
            expect(parser, SYM_RBRACE);
        }
    } else {
        base_type = parse_especificador_tipo(parser);
    }
    int pointer_level = parse_asteriscos(parser);
    
    token_t new_type_name = peek_token(parser);
    expect(parser, TK_ID);  // Consume new type name
    
    /* Parse array dimensions if any */
    int dimensions[8];
    int dim_count = parse_sufixo_array(parser, dimensions);
    
    expect(parser, SYM_SEMICOLON);  // Consume ';'
    
    /* Create typedef node */
    ast_node_t *typedef_decl = create_ast_node(AST_TYPEDEF_DECL, new_type_name.lexeme,
                                              new_type_name.linha, new_type_name.coluna);
    typedef_decl->data_type = base_type;
    typedef_decl->operator_type = pointer_level;  /* Store pointer level */
    if (type_node) {
        add_ast_child(typedef_decl, type_node);
    }
    
    /* Add to global symbol table as a type alias */
    symbol_info_t *type_info = (symbol_info_t*)calloc(1, sizeof(symbol_info_t));
    type_info->data_type = base_type;
    type_info->is_pointer = pointer_level;
    type_info->kind = SYMBOL_TYPEDEF;
    type_info->variable_type = VAR_GLOBAL;  /* Typedef is global */
    type_info->scope_id = 0;
    type_info->is_typedef = 1;
    type_info->is_array = (dim_count > 0);
    type_info->array_dim_count = dim_count;
    for (int i = 0; i < dim_count; i++) {
        type_info->array_dimensions[i] = dimensions[i];
    }
    /* Store the base struct/union tag name for semantic resolution */
    if (type_node && type_node->data.decl.name) {
        strncpy(type_info->typedef_base_name,
                type_node->data.decl.name,
                sizeof(type_info->typedef_base_name) - 1);
    }
    /* Inherit field table from the base struct/union if it was defined inline,
       and register the tag in its own struct/union namespace */
    if (type_node && (base_type == KW_STRUCT || base_type == KW_UNION)) {
        attach_struct_fields(type_info, type_node);
        type_info->is_struct_or_union = 1;

        if (type_node->data.decl.name && strcmp(type_node->data.decl.name, "(anonymous)") != 0) {
            symbol_info_t *tag_info = (symbol_info_t*)calloc(1, sizeof(symbol_info_t));
            tag_info->data_type = base_type;
            tag_info->kind = (base_type == KW_STRUCT) ? SYMBOL_STRUCT : SYMBOL_UNION;
            tag_info->variable_type = VAR_GLOBAL;
            tag_info->scope_id = 0;
            tag_info->is_struct_or_union = 1;
            attach_struct_fields(tag_info, type_node);
            
            /* Recalculate total struct size by summing member sizes */
            int total_sz = 0;
            for (int i = 0; i < tag_info->field_count; i++) {
                total_sz += tag_info->fields[i].size_bytes;
            }
            tag_info->size_bytes = total_sz;
            type_info->size_bytes = total_sz;

            char tag_key[264];
            snprintf(tag_key, sizeof(tag_key), "%s:%s",
                     (base_type == KW_STRUCT) ? "struct" : "union",
                     type_node->data.decl.name);
                     
            if (!lookup_global_symbol(parser, tag_key)) {
                add_global_symbol(parser, tag_key, tag_info);
            } else {
                free(tag_info);
            }
        }
    }
    
    if (!add_global_symbol(parser, new_type_name.lexeme, type_info)) {
        syntax_error(parser, "typedef ja declarado", -1, new_type_name);
        free(type_info);
    }
    
    return typedef_decl;
}

/* ============================================================================
   DECLARAÇÃO GLOBAL (DISPATCHER)
   ============================================================================ */
void parse_declaracao_global(parser_t *parser, ast_node_t *program_node) {
    int start_pos_global = parser->current_position;
    token_t token = peek_token(parser);

    /* Stray '}' at global scope: consume to guarantee progress */
    if (token.tipo == SYM_RBRACE) {
        syntax_error(parser, "chave '}' inesperada no escopo global", -1, token);
        if (!match(parser, TK_EOF)) {
            consume_token(parser);
        }
        return;
    }
    
    /* Preprocessor directives: keep in AST for traceability */
    if (token.tipo >= TK_PP_INCLUDE && token.tipo <= TK_PP_OTHER) {
        consume_token(parser);
        ast_node_t *pp = create_ast_node(AST_PP_DIRECTIVE, token.lexeme, token.linha, token.coluna);
        add_ast_child(program_node, pp);
        return;
    }
    
    /* Typedef declaration */
    if (token.tipo == KW_TYPEDEF) {
        ast_node_t *typedef_decl = parse_declaracao_typedef(parser);
        add_ast_child(program_node, typedef_decl);
        return;
    }
    
    /* Struct declaration or struct-typed variable */
    if (token.tipo == KW_STRUCT) {
        /* Lookahead: struct TAG { → struct definition
                     struct TAG ID → variable of struct type
                     struct TAG ;  → forward declaration */
        int save = parser->current_position;
        consume_token(parser);  /* consume 'struct' */
        if (match(parser, TK_ID)) {
            consume_token(parser);  /* consume TAG */
            if (match(parser, SYM_LBRACE) || match(parser, SYM_SEMICOLON)) {
                /* struct definition or forward decl — restore and dispatch */
                parser->current_position = save;
                ast_node_t *struct_decl = parse_declaracao_struct(parser);
                add_ast_child(program_node, struct_decl);
                return;
            } else {
                /* struct TAG var ... — treat as variable declaration */
                parser->current_position = save;
                ast_node_t *var = parse_declaracao_variavel_global(parser);
                add_ast_child(program_node, var);
                return;
            }
        } else if (match(parser, SYM_LBRACE)) {
            /* anonymous struct definition */
            parser->current_position = save;
            ast_node_t *struct_decl = parse_declaracao_struct(parser);
            add_ast_child(program_node, struct_decl);
            return;
        } else {
            parser->current_position = save;
        }
    }
    
    /* Union declaration or union-typed variable */
    if (token.tipo == KW_UNION) {
        int save = parser->current_position;
        consume_token(parser);  /* consume 'union' */
        if (match(parser, TK_ID)) {
            consume_token(parser);  /* consume TAG */
            if (match(parser, SYM_LBRACE) || match(parser, SYM_SEMICOLON)) {
                parser->current_position = save;
                ast_node_t *union_decl = parse_declaracao_union(parser);
                add_ast_child(program_node, union_decl);
                return;
            } else {
                parser->current_position = save;
                ast_node_t *var = parse_declaracao_variavel_global(parser);
                add_ast_child(program_node, var);
                return;
            }
        } else if (match(parser, SYM_LBRACE)) {
            parser->current_position = save;
            ast_node_t *union_decl = parse_declaracao_union(parser);
            add_ast_child(program_node, union_decl);
            return;
        } else {
            parser->current_position = save;
        }
    }
    
    /* Variable or function declaration */
    if (is_type_specifier(parser, token)) {
        int start_pos = parser->current_position;
        int type = parse_especificador_tipo(parser);
        int after_type_pos = parser->current_position;
        
        /* Skip pointer declarators to see identifier */
        parse_asteriscos(parser);
        
        if (match(parser, TK_ID)) {
            expect(parser, TK_ID);
            
            /* Check if it's a function (has '(') or variable */
            if (match(parser, SYM_LPAREN)) {
                /* Function declaration - backtrack to after type */
                parser->current_position = after_type_pos;
                ast_node_t *func = parse_declaracao_funcao(parser, type);
                add_ast_child(program_node, func);
            } else {
                /* Variable declaration - backtrack to start */
                parser->current_position = start_pos;
                ast_node_t *var = parse_declaracao_variavel_global(parser);
                add_ast_child(program_node, var);
            }
        } else {
            /* Not a valid declaration, restore position */
            parser->current_position = start_pos;
        }
    }

    /* Safety net: if we didn't consume anything, consume one token to avoid infinite loops */
    if (parser->current_position == start_pos_global) {
        token_t stuck = peek_token(parser);
        syntax_error(parser, "token inesperado em declaracao global", -1, stuck);
        if (!match(parser, TK_EOF)) {
            consume_token(parser);
        }
    }
}

/* ============================================================================
   LISTA DE DECLARAÇÕES GLOBAIS
   ============================================================================ */

/*
   ListaDeclaracoesGlobais → DeclaracaoGlobal*
*/
void parse_lista_declaracoes_globais(parser_t *parser, ast_node_t *program_node) {
    while (!match(parser, TK_EOF)) {
        int before = parser->current_position;
        parse_declaracao_global(parser, program_node);
        if (parser->current_position == before) {
            /* Absolute last-resort progress guarantee */
            if (!match(parser, TK_EOF)) {
                consume_token(parser);
            }
        }
    }
}

/* ============================================================================
   PROGRAMA (ENTRY POINT)
   ============================================================================ */

/*
   Programa → ListaDeclaracoesGlobais
*/
ast_node_t* parse_programa(parser_t *parser) {
    ast_node_t *program = create_ast_node(AST_PROGRAM, "program", 0, 0);
    
    parse_lista_declaracoes_globais(parser, program);
    
    return program;
}

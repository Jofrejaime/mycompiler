#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Forward declarations */
extern int calculate_total_size(int data_type, int is_pointer, 
                               int *dimensions, int dim_count);
extern int get_base_type_size(int data_type, int is_pointer);
void parse_campos_struct_union(parser_t *parser, ast_node_t *parent, int *total_size);
ast_node_t* parse_declaracao_typedef(parser_t *parser);
ast_node_t* parse_declaracao_union(parser_t *parser);

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
   Stores dimensions and returns count
*/
int parse_sufixo_array(parser_t *parser, int *dimensions) {
    int dim_count = 0;
    
    while (match(parser, SYM_LBRACKET)) {
        consume_token(parser);  // Consume '['
        
        /* Parse array size - can be empty or expression */
        if (match(parser, SYM_RBRACKET)) {
            /* Empty brackets [] */
            dimensions[dim_count++] = 0;
        } else {
            /* Consume an expression for the size */
            parse_expressao(parser);
            dimensions[dim_count++] = 0;  /* Mark as dynamic/expression */
        }
        
        expect(parser, SYM_RBRACKET);  // Consume ']'
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
*/
int parse_especificador_tipo(parser_t *parser) {
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
                expect(parser, TK_ID);  // Struct name
            }
            /* Note: Could be struct {...} but that's handled elsewhere */
            return KW_STRUCT;
        }
        
        case KW_UNION: {
            consume_token(parser);  // Consume 'union'
            if (match(parser, TK_ID)) {
                expect(parser, TK_ID);  // Union name
            }
            return KW_UNION;
        }
        
        case TK_ID:
            /* Only accept TK_ID here when it's a known typedef name. */
            if (!is_typedef_name(parser, token.lexeme)) {
                syntax_error(parser, "especificador de tipo esperado (typedef desconhecido)", -1, token);
                return KW_INT;  /* Default fallback */
            }
            consume_token(parser);
            return TK_ID;
        
        default:
            syntax_error(parser, "especificador de tipo esperado", -1, token);
            return KW_INT;  /* Default fallback */
    }
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
    int data_type = parse_especificador_tipo(parser);

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

        /* Optional initializer */
        if (match(parser, OP_ASSIGN)) {
            consume_token(parser);  // Consume '='
            ast_node_t *init_expr = parse_expressao(parser);
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

        int total_size = calculate_total_size(data_type, pointer_level, dimensions, dim_count);
        info->size_bytes = total_size;

        info->memory_address = parser->next_global_address;
        parser->next_global_address += total_size;

        add_global_symbol(parser, id_token.lexeme, info);
        enrich_symbol_type(parser, id_token.lexeme, data_type, pointer_level);
        enrich_symbol_scope(parser, id_token.lexeme, 0, VAR_GLOBAL);
        enrich_symbol_memory(parser, id_token.lexeme, info->memory_address, total_size);
        if (is_array)
            enrich_symbol_array(parser, id_token.lexeme, dimensions, dim_count);

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
*/
void parse_lista_parametros(parser_t *parser, ast_node_t *func_node, 
                            int *param_types, int *param_count) {
    *param_count = 0;
    
    /* Empty parameter list */
    if (match(parser, SYM_RPAREN)) {
        return;
    }
    
    /* void parameter list */
    if (match(parser, KW_VOID)) {
        consume_token(parser);
        if (match(parser, SYM_RPAREN)) {
            return;  /* void means no parameters */
        }
        /* Otherwise, it's a parameter named 'void' - error, but continue */
    }
    
    /* Parse parameters */
    do {
        int param_type = parse_especificador_tipo(parser);
        int pointer_level = parse_asteriscos(parser);
        token_t param_name = peek_token(parser);
        expect(parser, TK_ID);
        
        /* Check for array parameter */
        int dimensions[8];
        int dim_count = parse_sufixo_array(parser, dimensions);
        int is_array = (dim_count > 0);
        
        /* Create parameter node */
        ast_node_t *param = create_ast_node(AST_PARAM_DECL, param_name.lexeme,
                                           param_name.linha, param_name.coluna);
        param->data_type = param_type;
        param->operator_type = pointer_level;
        add_ast_child(func_node, param);
        
        /* Add to parameter types array */
        param_types[*param_count] = param_type;
        (*param_count)++;
        
        /* Add parameter to local symbol table */
        symbol_info_t *param_info = (symbol_info_t*)calloc(1, sizeof(symbol_info_t));
        param_info->data_type = param_type;
        param_info->is_pointer = pointer_level;
        param_info->kind = SYMBOL_PARAMETER;
        param_info->variable_type = VAR_PARAMETER;
        param_info->scope_id = parser->current_local_table ? parser->current_local_table->scope_id : 1;
        param_info->is_array = is_array;
        param_info->size_bytes = (pointer_level > 0) ? 8 : 4;  /* Parameters are typically 4 bytes */
        
        if (parser->current_local_table) {
            param_info->memory_address = scope_allocate_memory(parser->current_local_table, 4);
            add_local_symbol(parser, param_name.lexeme, param_info);
            enrich_symbol_type(parser, param_name.lexeme, param_type, pointer_level);
            enrich_symbol_scope(parser, param_name.lexeme, param_info->scope_id, VAR_PARAMETER);
            enrich_symbol_memory(parser, param_name.lexeme, param_info->memory_address, param_info->size_bytes);
            if (is_array) {
                enrich_symbol_array(parser, param_name.lexeme, dimensions, dim_count);
            }
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
        
        /* Add struct to global symbol table */
        symbol_info_t *struct_info = (symbol_info_t*)calloc(1, sizeof(symbol_info_t));
        struct_info->data_type = KW_STRUCT;
        struct_info->kind = SYMBOL_STRUCT;
        struct_info->variable_type = VAR_GLOBAL;
        struct_info->scope_id = 0;
        struct_info->size_bytes = total_size;
        
        add_global_symbol(parser, struct_name, struct_info);
    } else {
        /* Forward declaration or reference */
        expect(parser, SYM_SEMICOLON);
    }
    
    return struct_decl;
}

/* ============================================================================
   HELPER: Parse campos de struct/union
   ============================================================================ */

void parse_campos_struct_union(parser_t *parser, ast_node_t *parent, int *total_size) {
    while (!match(parser, SYM_RBRACE) && !match(parser, TK_EOF)) {
        int member_type = parse_especificador_tipo(parser);
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
        add_ast_child(parent, member);
        
        /* Calculate size using helper function */
        int member_size = calculate_total_size(member_type, pointer_level, dimensions, dim_count);
        
        *total_size += member_size;
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
        
        /* Add union to global symbol table */
        symbol_info_t *union_info = (symbol_info_t*)calloc(1, sizeof(symbol_info_t));
        union_info->data_type = KW_UNION;
        union_info->kind = SYMBOL_UNION;
        union_info->variable_type = VAR_GLOBAL;
        union_info->scope_id = 0;
        union_info->size_bytes = max_size;
        
        add_global_symbol(parser, union_name, union_info);
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
    
    /* Struct declaration */
    if (token.tipo == KW_STRUCT) {
        ast_node_t *struct_decl = parse_declaracao_struct(parser);
        add_ast_child(program_node, struct_decl);
        return;
    }
    
    /* Union declaration */
    if (token.tipo == KW_UNION) {
        ast_node_t *union_decl = parse_declaracao_union(parser);
        add_ast_child(program_node, union_decl);
        return;
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

#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

void parse_lista_instrucoes_declaracoes(parser_t *parser, ast_node_t *parent);

/* ============================================================================
   PARSER DE STATEMENTS (INSTRUÇÕES) - IMPLEMENTAÇÃO
   
   Implementa parsing de comandos/instruções:
   - Blocos { ... }
   - If-else
   - While
   - Do-while
   - For
   - Return
   - Break
   - Continue
   - Expressões
   
   ============================================================================ */

/* ============================================================================
   BLOCO DE INSTRUÇÕES
   ============================================================================ */

/*
   Bloco → SYM_LBRACE ListaInstrucoes SYM_RBRACE
   ListaInstrucoes → Instrucao ListaInstrucoes
                   | ε
*/
ast_node_t* parse_bloco(parser_t *parser) {
    token_t token = peek_token(parser);
    expect(parser, SYM_LBRACE);  // Consume '{'
    
    ast_node_t *block = create_ast_node(AST_BLOCK, NULL, token.linha, token.coluna);
    
    /* Enter new scope for block */
    enter_local_scope(parser);
    
    /* Parse declarations and statements until '}' */
    parse_lista_instrucoes_declaracoes(parser, block);
    
    expect(parser, SYM_RBRACE);  // Consume '}'
    
    /* Exit block scope */
    exit_local_scope(parser);
    
    return block;
}

/* ============================================================================
   INSTRUÇÃO IF-ELSE
   ============================================================================ */

/*
   InstrucaoIf → KW_IF SYM_LPAREN Expressao SYM_RPAREN Instrucao
                 (KW_ELSE Instrucao)?
*/
ast_node_t* parse_instrucao_if(parser_t *parser) {
    token_t token = peek_token(parser);
    expect(parser, KW_IF);  // Consume 'if'
    
    expect(parser, SYM_LPAREN);  // Consume '('
    ast_node_t *condition = parse_expressao(parser);
    expect(parser, SYM_RPAREN);  // Consume ')'
    
    ast_node_t *then_stmt = parse_instrucao(parser);
    
    ast_node_t *if_node = create_ast_node(AST_IF_STMT, NULL, token.linha, token.coluna);
    add_ast_child(if_node, condition);
    add_ast_child(if_node, then_stmt);
    
    /* Optional else */
    if (match(parser, KW_ELSE)) {
        consume_token(parser);  // Consume 'else'
        ast_node_t *else_stmt = parse_instrucao(parser);
        add_ast_child(if_node, else_stmt);
    }
    
    return if_node;
}

/* ============================================================================
   INSTRUÇÃO WHILE
   ============================================================================ */

/*
   InstrucaoWhile → KW_WHILE SYM_LPAREN Expressao SYM_RPAREN Instrucao
*/
ast_node_t* parse_instrucao_while(parser_t *parser) {
    token_t token = peek_token(parser);
    expect(parser, KW_WHILE);  // Consume 'while'
    
    expect(parser, SYM_LPAREN);  // Consume '('
    ast_node_t *condition = parse_expressao(parser);
    expect(parser, SYM_RPAREN);  // Consume ')'
    
    ast_node_t *body = parse_instrucao(parser);
    
    ast_node_t *while_node = create_ast_node(AST_WHILE_STMT, NULL, token.linha, token.coluna);
    add_ast_child(while_node, condition);
    add_ast_child(while_node, body);
    
    return while_node;
}

/* ============================================================================
   INSTRUÇÃO DO-WHILE
   ============================================================================ */

/*
   InstrucaoDo → KW_DO Instrucao KW_WHILE SYM_LPAREN Expressao SYM_RPAREN SYM_SEMICOLON
*/
ast_node_t* parse_instrucao_do(parser_t *parser) {
    token_t token = peek_token(parser);
    expect(parser, KW_DO);  // Consume 'do'
    
    ast_node_t *body = parse_instrucao(parser);
    
    expect(parser, KW_WHILE);  // Consume 'while'
    expect(parser, SYM_LPAREN);  // Consume '('
    ast_node_t *condition = parse_expressao(parser);
    expect(parser, SYM_RPAREN);  // Consume ')'
    expect(parser, SYM_SEMICOLON);  // Consume ';'
    
    ast_node_t *do_while_node = create_ast_node(AST_DO_WHILE_STMT, NULL, token.linha, token.coluna);
    add_ast_child(do_while_node, body);
    add_ast_child(do_while_node, condition);
    
    return do_while_node;
}

/* ============================================================================
   INSTRUÇÃO FOR
   ============================================================================ */

/*
   InstrucaoFor → KW_FOR SYM_LPAREN 
                  (Expressao | ε) SYM_SEMICOLON
                  (Expressao | ε) SYM_SEMICOLON
                  (Expressao | ε) SYM_RPAREN
                  Instrucao
*/
ast_node_t* parse_instrucao_for(parser_t *parser) {
    token_t token = peek_token(parser);
    expect(parser, KW_FOR);  // Consume 'for'
    
    expect(parser, SYM_LPAREN);  // Consume '('
    
    /* Initialization (optional) */
    ast_node_t *init = NULL;
    if (!match(parser, SYM_SEMICOLON)) {
        init = parse_expressao(parser);
    }
    expect(parser, SYM_SEMICOLON);  // Consume ';'
    
    /* Condition (optional) */
    ast_node_t *condition = NULL;
    if (!match(parser, SYM_SEMICOLON)) {
        condition = parse_expressao(parser);
    }
    expect(parser, SYM_SEMICOLON);  // Consume ';'
    
    /* Increment (optional) */
    ast_node_t *increment = NULL;
    if (!match(parser, SYM_RPAREN)) {
        increment = parse_expressao(parser);
    }
    expect(parser, SYM_RPAREN);  // Consume ')'
    
    /* Body */
    ast_node_t *body = parse_instrucao(parser);
    
    ast_node_t *for_node = create_ast_node(AST_FOR_STMT, NULL, token.linha, token.coluna);
    if (init) add_ast_child(for_node, init);
    if (condition) add_ast_child(for_node, condition);
    if (increment) add_ast_child(for_node, increment);
    add_ast_child(for_node, body);
    
    return for_node;
}

/* ============================================================================
   INSTRUÇÃO RETURN
   ============================================================================ */

/*
   InstrucaoReturn → KW_RETURN (Expressao | ε) SYM_SEMICOLON
*/
ast_node_t* parse_instrucao_return(parser_t *parser) {
    token_t token = peek_token(parser);
    expect(parser, KW_RETURN);  // Consume 'return'
    
    ast_node_t *return_node = create_ast_node(AST_RETURN_STMT, NULL, token.linha, token.coluna);
    
    /* Optional return value */
    if (!match(parser, SYM_SEMICOLON)) {
        ast_node_t *value = parse_expressao(parser);
        add_ast_child(return_node, value);
    }
    
    expect(parser, SYM_SEMICOLON);  // Consume ';'
    
    return return_node;
}

/* ============================================================================
   INSTRUÇÃO BREAK
   ============================================================================ */

/*
   InstrucaoBreak → KW_BREAK SYM_SEMICOLON
*/
ast_node_t* parse_instrucao_break(parser_t *parser) {
    token_t token = peek_token(parser);
    expect(parser, KW_BREAK);  // Consume 'break'
    expect(parser, SYM_SEMICOLON);  // Consume ';'
    
    return create_ast_node(AST_BREAK_STMT, NULL, token.linha, token.coluna);
}

/* ============================================================================
   INSTRUÇÃO CONTINUE
   ============================================================================ */

/*
   InstrucaoContinue → KW_CONTINUE SYM_SEMICOLON
*/
ast_node_t* parse_instrucao_continue(parser_t *parser) {
    token_t token = peek_token(parser);
    expect(parser, KW_CONTINUE);  // Consume 'continue'
    expect(parser, SYM_SEMICOLON);  // Consume ';'
    
    return create_ast_node(AST_CONTINUE_STMT, NULL, token.linha, token.coluna);
}

/* ============================================================================
   INSTRUÇÃO DE EXPRESSÃO
   ============================================================================ */

/*
   InstrucaoExpressao → Expressao SYM_SEMICOLON
                      | SYM_SEMICOLON
*/
ast_node_t* parse_instrucao_expressao(parser_t *parser) {
    token_t token = peek_token(parser);
    
    /* Empty statement (just semicolon) */
    if (match(parser, SYM_SEMICOLON)) {
        consume_token(parser);
        return NULL;  /* Empty statement */
    }
    
    /* Expression statement */
    ast_node_t *expr = parse_expressao(parser);
    expect(parser, SYM_SEMICOLON);  // Consume ';'
    
    ast_node_t *expr_stmt = create_ast_node(AST_EXPR_STMT, NULL, token.linha, token.coluna);
    add_ast_child(expr_stmt, expr);
    
    return expr_stmt;
}

/* ============================================================================
   INSTRUCAO GOTO
   ============================================================================ */

/*
   InstrucaoGoto -> KW_GOTO TK_ID SYM_SEMICOLON
*/
ast_node_t* parse_instrucao_goto(parser_t *parser) {
    token_t tok = peek_token(parser);
    expect(parser, KW_GOTO);

    token_t label = peek_token(parser);
    expect(parser, TK_ID);
    expect(parser, SYM_SEMICOLON);

    return create_ast_node(AST_GOTO_STMT, label.lexeme, tok.linha, tok.coluna);
}

/* ============================================================================
   LABEL (rotulo) para goto
   ============================================================================ */

/*
   Label -> TK_ID SYM_COLON Instrucao
*/
static ast_node_t* parse_label(parser_t *parser) {
    token_t label = peek_token(parser);
    expect(parser, TK_ID);
    expect(parser, SYM_COLON);

    ast_node_t *node = create_ast_node(AST_LABEL_STMT, label.lexeme, label.linha, label.coluna);
    ast_node_t *stmt = parse_instrucao(parser);
    if (stmt) {
        add_ast_child(node, stmt);
    }
    return node;
}

/* ============================================================================
   INSTRUÇÃO SWITCH
   ============================================================================ */

/*
   InstrucaoSwitch → KW_SWITCH SYM_LPAREN Expressao SYM_RPAREN SYM_LBRACE ListaCasos SYM_RBRACE
   ListaCasos → Caso ListaCasos | ε
   Caso → KW_CASE Expressao SYM_COLON ListaInstrucoes
        | KW_DEFAULT SYM_COLON ListaInstrucoes
*/

ast_node_t* parse_caso(parser_t *parser) {
    token_t token = peek_token(parser);
    
    if (match(parser, KW_CASE)) {
        consume_token(parser);  // Consume 'case'
        ast_node_t *case_expr = parse_expressao(parser);
        expect(parser, SYM_COLON);  // Consume ':'
        
        ast_node_t *case_node = create_ast_node(AST_CASE, NULL, token.linha, token.coluna);
        add_ast_child(case_node, case_expr);
        
        /* Parse statements until next case, default, or closing brace */
        while (!match(parser, KW_CASE) && !match(parser, KW_DEFAULT) && 
               !match(parser, SYM_RBRACE) && !match(parser, TK_EOF)) {
            ast_node_t *stmt = parse_instrucao(parser);
            if (stmt) {
                add_ast_child(case_node, stmt);
            }
        }
        
        return case_node;
    } else if (match(parser, KW_DEFAULT)) {
        consume_token(parser);  // Consume 'default'
        expect(parser, SYM_COLON);  // Consume ':'
        
        ast_node_t *default_node = create_ast_node(AST_DEFAULT, NULL, token.linha, token.coluna);
        
        /* Parse statements until next case, default, or closing brace */
        while (!match(parser, KW_CASE) && !match(parser, KW_DEFAULT) && 
               !match(parser, SYM_RBRACE) && !match(parser, TK_EOF)) {
            ast_node_t *stmt = parse_instrucao(parser);
            if (stmt) {
                add_ast_child(default_node, stmt);
            }
        }
        
        return default_node;
    }
    
    return NULL;
}

ast_node_t* parse_instrucao_switch(parser_t *parser) {
    token_t token = peek_token(parser);
    expect(parser, KW_SWITCH);  // Consume 'switch'
    
    expect(parser, SYM_LPAREN);  // Consume '('
    ast_node_t *switch_expr = parse_expressao(parser);
    expect(parser, SYM_RPAREN);  // Consume ')'
    
    expect(parser, SYM_LBRACE);  // Consume '{'
    
    ast_node_t *switch_node = create_ast_node(AST_SWITCH_STMT, NULL, token.linha, token.coluna);
    add_ast_child(switch_node, switch_expr);
    
    /* Parse list of cases */
    while (!match(parser, SYM_RBRACE) && !match(parser, TK_EOF)) {
        ast_node_t *case_node = parse_caso(parser);
        if (case_node) {
            add_ast_child(switch_node, case_node);
        }
    }
    
    expect(parser, SYM_RBRACE);  // Consume '}'
    
    return switch_node;
}

/* ============================================================================
   INSTRUÇÃO GENÉRICA (DISPATCHER)
   ============================================================================ */

/*
   Instrucao → InstrucaoIf
             | InstrucaoWhile
             | InstrucaoDo
             | InstrucaoFor
             | InstrucaoReturn
             | InstrucaoBreak
             | InstrucaoContinue
             | InstrucaoSwitch
             | Bloco
             | InstrucaoExpressao
*/
ast_node_t* parse_instrucao(parser_t *parser) {
    token_t token = peek_token(parser);

    /* Label lookahead: IDENT ':' */
    if (token.tipo == TK_ID) {
        int next_pos = parser->current_position + 1;
        if (next_pos < parser->token_stream->quantidade &&
            parser->token_stream->tokens[next_pos].tipo == SYM_COLON) {
            return parse_label(parser);
        }
    }
    
    switch (token.tipo) {
        case KW_IF:
            return parse_instrucao_if(parser);
        
        case KW_WHILE:
            return parse_instrucao_while(parser);
        
        case KW_DO:
            return parse_instrucao_do(parser);
        
        case KW_FOR:
            return parse_instrucao_for(parser);
        
        case KW_SWITCH:
            return parse_instrucao_switch(parser);
        
        case KW_RETURN:
            return parse_instrucao_return(parser);
        
        case KW_BREAK:
            return parse_instrucao_break(parser);
        
        case KW_CONTINUE:
            return parse_instrucao_continue(parser);

        case KW_GOTO:
            return parse_instrucao_goto(parser);
        
        case SYM_LBRACE:
            return parse_bloco(parser);
        
        default:
            /* Expression statement or empty statement */
            return parse_instrucao_expressao(parser);
    }
}

/* ============================================================================
   DECLARAÇÃO LOCAL DE VARIÁVEL (dentro de função)
   ============================================================================ */

/*
   DeclaracaoLocal → EspecificadorTipo TK_ID (SYM_LBRACKET TK_NUM_INT SYM_RBRACKET)* 
                     (OP_ASSIGN Expressao)? SYM_SEMICOLON
*/
ast_node_t* parse_declaracao_local(parser_t *parser) {
    int data_type = parse_especificador_tipo(parser);
    int pointer_level = parse_asteriscos(parser);
    
    token_t id_token = peek_token(parser);
    expect(parser, TK_ID);  // Consume identifier
    
    ast_node_t *var_decl = create_ast_node(AST_VAR_DECL, id_token.lexeme, 
                                           id_token.linha, id_token.coluna);
    var_decl->data_type = data_type;
    var_decl->operator_type = pointer_level;
    
    /* Check for array dimensions */
    int dimensions[8];
    int dim_count = parse_sufixo_array(parser, dimensions);
    int is_array = (dim_count > 0);
    
    /* Optional initialization */
    if (match(parser, OP_ASSIGN)) {
        consume_token(parser);  // Consume '='
        ast_node_t *init_expr = parse_inicializacao(parser);
        add_ast_child(var_decl, init_expr);
    }
    
    expect(parser, SYM_SEMICOLON);  // Consume ';'
    
    /* Add to local symbol table */
    symbol_info_t *info = (symbol_info_t*)calloc(1, sizeof(symbol_info_t));
    info->data_type = data_type;
    info->is_pointer = pointer_level;
    info->kind = SYMBOL_VARIABLE;
    info->variable_type = VAR_LOCAL;
    info->scope_id = parser->current_local_table ? parser->current_local_table->scope_id : 0;
    info->is_array = is_array;
    info->array_dim_count = dim_count;
    for (int i = 0; i < dim_count; i++) {
        info->array_dimensions[i] = dimensions[i];
    } 
    
    /* Calculate size */
    int element_size = 4;  /* Default: int/float = 4 bytes */
    if (data_type == KW_CHAR) element_size = 1;
    if (data_type == KW_DOUBLE) element_size = 8;
    if (pointer_level > 0) element_size = 8;
    
    int total_size = element_size;
    for (int i = 0; i < dim_count; i++) {
        if (dimensions[i] > 0) {
            total_size *= dimensions[i];
        }
    }
    info->size_bytes = total_size;
    
    /* Allocate memory in current scope */
    if (parser->current_local_table) {
        info->memory_address = scope_allocate_memory(parser->current_local_table, total_size);
        add_local_symbol(parser, id_token.lexeme, info);
        enrich_symbol_type(parser, id_token.lexeme, data_type, pointer_level);
        enrich_symbol_scope(parser, id_token.lexeme, info->scope_id, VAR_LOCAL);
        enrich_symbol_memory(parser, id_token.lexeme, info->memory_address, total_size);
        if (is_array) {
            enrich_symbol_array(parser, id_token.lexeme, dimensions, dim_count);
        }
    }
    
    return var_decl;
}

/* ============================================================================
   LISTA DE INSTRUÇÕES E DECLARAÇÕES (dentro de bloco/função)
   ============================================================================ */

/*
   ListaInstrucoesDeclaracoes → (DeclaracaoLocal | Instrucao)*
*/
void parse_lista_instrucoes_declaracoes(parser_t *parser, ast_node_t *parent) {
    while (!match(parser, SYM_RBRACE) && !match(parser, TK_EOF)) {
        int before = parser->current_position;
        token_t token = peek_token(parser);
        
        /* Check if it's a type specifier (declaration) */
        if (is_type_specifier(parser, token)) {
            ast_node_t *decl = parse_declaracao_local(parser);
            if (decl) {
                add_ast_child(parent, decl);
            }
        } else {
            /* It's a statement */
            ast_node_t *stmt = parse_instrucao(parser);
            if (stmt) {
                add_ast_child(parent, stmt);
            }
        }

        /* Progress guarantee: if nothing was consumed, consume one token. */
        if (parser->current_position == before) {
            token_t stuck = peek_token(parser);
            syntax_error(parser, "token inesperado em bloco", -1, stuck);
            if (!match(parser, SYM_RBRACE) && !match(parser, TK_EOF)) {
                consume_token(parser);
            }
        }
    }
}

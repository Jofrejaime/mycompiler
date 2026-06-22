#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

/* ============================================================================
   PARSER DE EXPRESSÕES - IMPLEMENTAÇÃO
   
   Implementa parsing de expressões com 15 níveis de precedência
   usando Recursive Descent sem backtracking
   
    Precedência (do menor para o maior):
    1. Atribuição (=, +=, -=, *=, /=, &=, |=, ^=)
    2. Ternário (? :)
    3. Logical OR (||)
    4. Logical AND (&&)
    5. Bitwise OR (|)
    6. Bitwise XOR (^)
    7. Bitwise AND (&)
    8. Igualdade (==, !=)
    9. Relacional (<, >, <=, >=)
    10. Shift (<<, >>)
    11. Aditivo (+, -)
    12. Multiplicativo (*, /, %)
    13. Unário (!, ~, ++, --, sizeof, +, -, *, &)
    14. Postfix ([], (), ., ->, ++, --)
    15. Primário (id, num, string, char, (expr))
   
   ============================================================================ */

/* ============================================================================
    NÍVEL 15: EXPRESSÕES PRIMÁRIAS
   ============================================================================ */

/*
   Primario → TK_ID
            | TK_NUM_INT
            | TK_NUM_FLOAT
            | TK_NUM_EXP
            | TK_STRING
            | TK_CHAR
            | SYM_LPAREN Expressao SYM_RPAREN
*/
ast_node_t* parse_primario(parser_t *parser) {
    token_t token = peek_token(parser);
    
    switch (token.tipo) {
        case TK_ID: {
            consume_token(parser);
            return create_identifier_node(token.lexeme, token.linha, token.coluna);
        }
        
        case TK_NUM_INT: {
            consume_token(parser);
            return create_literal_node(AST_INT_LITERAL, token.lexeme, token.linha, token.coluna);
        }
        
        case TK_NUM_FLOAT:
        case TK_NUM_EXP: {
            consume_token(parser);
            return create_literal_node(AST_FLOAT_LITERAL, token.lexeme, token.linha, token.coluna);
        }
        
        case TK_STRING: {
            consume_token(parser);
            return create_literal_node(AST_STRING_LITERAL, token.lexeme, token.linha, token.coluna);
        }
        
        case TK_CHAR: {
            consume_token(parser);
            return create_literal_node(AST_CHAR_LITERAL, token.lexeme, token.linha, token.coluna);
        }
        
        case SYM_LPAREN: {
            consume_token(parser);  // Consume '('
            ast_node_t *expr = parse_expressao(parser);
            expect(parser, SYM_RPAREN);  // Expect ')'
            return expr;
        }
        
        default:
            syntax_error(parser, "expressão primária esperada", -1, token);
            return NULL;
    }
}

/* ============================================================================
    NÍVEL 14: EXPRESSÕES POSTFIX
   ============================================================================ */

/*
   PosFix → Primario PosFixR
   PosFixR → SYM_LBRACKET Expressao SYM_RBRACKET PosFixR
           | SYM_LPAREN ListaArgumentos SYM_RPAREN PosFixR
           | SYM_DOT TK_ID PosFixR
       | OP_ARROW TK_ID PosFixR
           | OP_INC PosFixR
           | OP_DEC PosFixR
           | ε
*/
ast_node_t* parse_pos_fixo_r(parser_t *parser, ast_node_t *left) {
    token_t token = peek_token(parser);
    
    switch (token.tipo) {
        case SYM_LBRACKET: {
            // Array access: left[index]
            consume_token(parser);
            ast_node_t *index = parse_expressao(parser);
            expect(parser, SYM_RBRACKET);
            
            ast_node_t *array_access = create_ast_node(AST_ARRAY_ACCESS, NULL, token.linha, token.coluna);
            add_ast_child(array_access, left);
            add_ast_child(array_access, index);
            
            return parse_pos_fixo_r(parser, array_access);
        }
        
        case SYM_LPAREN: {
            // Function call: left(args)
            consume_token(parser);
            
            ast_node_t *call = create_ast_node(AST_CALL, NULL, token.linha, token.coluna);
            add_ast_child(call, left);
            
            // Parse arguments
            if (!match(parser, SYM_RPAREN)) {
                do {
                    ast_node_t *arg = parse_expressao(parser);
                    add_ast_child(call, arg);
                    
                    if (match(parser, SYM_COMMA)) {
                        consume_token(parser);
                    } else {
                        break;
                    }
                } while (1);
            }
            
            expect(parser, SYM_RPAREN);
            return parse_pos_fixo_r(parser, call);
        }
        
        case SYM_DOT:
        case OP_ARROW: {
            // Member access: left.member
            consume_token(parser);
            token_t member = peek_token(parser);
            expect(parser, TK_ID);
            
            ast_node_t *member_access = create_ast_node(AST_MEMBER_ACCESS, NULL, token.linha, token.coluna);
            member_access->operator_type = token.tipo;
            add_ast_child(member_access, left);
            add_ast_child(member_access, create_identifier_node(member.lexeme, member.linha, member.coluna));
            
            return parse_pos_fixo_r(parser, member_access);
        }
        
        case OP_INC:
        case OP_DEC: {
            // Postfix increment/decrement: left++ or left--
            consume_token(parser);
            ast_node_t *postfix = create_unary_op_node(token.tipo, left, token.linha, token.coluna);
            return parse_pos_fixo_r(parser, postfix);
        }
        
        default:
            // ε (epsilon) - end of postfix
            return left;
    }
}

ast_node_t* parse_pos_fixo(parser_t *parser) {
    ast_node_t *left = parse_primario(parser);
    return parse_pos_fixo_r(parser, left);
}

/* ============================================================================
    NÍVEL 13: EXPRESSÕES UNÁRIAS
   ============================================================================ */

/*
   Unario → OP_INC Unario
          | OP_DEC Unario
          | OP_PLUS Unario
          | OP_MINUS Unario
          | OP_NOT Unario
          | OP_BITNOT Unario
          | OP_MULT Unario      (dereference)
          | OP_BITAND Unario    (address-of)
          | KW_SIZEOF Unario
          | PosFix
*/
ast_node_t* parse_unario(parser_t *parser) {
    token_t token = peek_token(parser);
    
    switch (token.tipo) {
        case OP_INC:
        case OP_DEC:
        case OP_PLUS:
        case OP_MINUS:
        case OP_NOT:
        case OP_BITNOT:
        case OP_MULT:      // Dereference *ptr
        case OP_BITAND: {  // Address-of &var
            consume_token(parser);
            ast_node_t *operand = parse_unario(parser);
            return create_unary_op_node(token.tipo, operand, token.linha, token.coluna);
        }
        
        case KW_SIZEOF: {
            consume_token(parser);  /* Consume 'sizeof' */

            /*
             * Disambiguate sizeof(type) from sizeof(expr):
             *   sizeof '(' type_specifier ...')' → AST_SIZEOF with type info
             *   sizeof expr                     → AST_SIZEOF wrapping an expr
             */
            if (match(parser, SYM_LPAREN)) {
                /* Look one token ahead inside the '(' */
                int save_pos = parser->current_position;
                consume_token(parser);  /* consume '(' */
                token_t inner = peek_token(parser);

                if (is_type_specifier(parser, inner)) {
                    /* sizeof ( type ) */
                    int sz_type = parse_especificador_tipo(parser);
                    int sz_ptr  = parse_asteriscos(parser);
                    expect(parser, SYM_RPAREN);

                    ast_node_t *sz_node = create_ast_node(
                        AST_UNARY_OP, NULL, token.linha, token.coluna);
                    sz_node->operator_type = KW_SIZEOF;
                    sz_node->data_type     = sz_type;
                    sz_node->data.expr.operator = KW_SIZEOF;
                    /* Store pointer level in literal_type field */
                    sz_node->literal_type  = sz_ptr;
                    /* No child: it's a type operand, not an expression */
                    return sz_node;
                } else {
                    /* sizeof ( expr ) — restore and fall through to expr path */
                    parser->current_position = save_pos;
                    ast_node_t *operand = parse_unario(parser);
                    return create_unary_op_node(KW_SIZEOF, operand,
                                                token.linha, token.coluna);
                }
            } else {
                /* sizeof expr (no parentheses) */
                ast_node_t *operand = parse_unario(parser);
                return create_unary_op_node(KW_SIZEOF, operand,
                                            token.linha, token.coluna);
            }
        }
        
        default:
            return parse_pos_fixo(parser);
    }
}

/* ============================================================================
    NÍVEL 12: EXPRESSÕES MULTIPLICATIVAS
   ============================================================================ */

/*
   Multiplicativo → Unario MultiplicativoR
   MultiplicativoR → OP_MULT Unario MultiplicativoR
                   | OP_DIV Unario MultiplicativoR
                   | OP_MOD Unario MultiplicativoR
                   | ε
*/
ast_node_t* parse_multiplicativo_r(parser_t *parser, ast_node_t *left) {
    token_t token = peek_token(parser);
    
    if (token.tipo == OP_MULT || token.tipo == OP_DIV || token.tipo == OP_MOD) {
        consume_token(parser);
        ast_node_t *right = parse_unario(parser);
        ast_node_t *binary = create_binary_op_node(token.tipo, left, right, token.linha, token.coluna);
        return parse_multiplicativo_r(parser, binary);
    }
    
    return left;  // ε
}

ast_node_t* parse_multiplicativo(parser_t *parser) {
    ast_node_t *left = parse_unario(parser);
    return parse_multiplicativo_r(parser, left);
}

/* ============================================================================
    NÍVEL 11: EXPRESSÕES ADITIVAS
   ============================================================================ */

/*
   Aditivo → Multiplicativo AditivoR
   AditivoR → OP_PLUS Multiplicativo AditivoR
            | OP_MINUS Multiplicativo AditivoR
            | ε
*/
ast_node_t* parse_aditivo_r(parser_t *parser, ast_node_t *left) {
    token_t token = peek_token(parser);
    
    if (token.tipo == OP_PLUS || token.tipo == OP_MINUS) {
        consume_token(parser);
        ast_node_t *right = parse_multiplicativo(parser);
        ast_node_t *binary = create_binary_op_node(token.tipo, left, right, token.linha, token.coluna);
        return parse_aditivo_r(parser, binary);
    }
    
    return left;  // ε
}

ast_node_t* parse_aditivo(parser_t *parser) {
    ast_node_t *left = parse_multiplicativo(parser);
    return parse_aditivo_r(parser, left);
}

/* ============================================================================
    NÍVEL 10: EXPRESSÕES SHIFT
   ============================================================================ */

/*
   Shift → Aditivo ShiftR
   ShiftR → OP_LSHIFT Aditivo ShiftR
          | OP_RSHIFT Aditivo ShiftR
          | ε
*/
ast_node_t* parse_shift_r(parser_t *parser, ast_node_t *left) {
    token_t token = peek_token(parser);

    if (token.tipo == OP_LSHIFT || token.tipo == OP_RSHIFT) {
        consume_token(parser);
        ast_node_t *right = parse_aditivo(parser);
        ast_node_t *binary = create_binary_op_node(token.tipo, left, right, token.linha, token.coluna);
        return parse_shift_r(parser, binary);
    }

    return left;  // ε
}

ast_node_t* parse_shift(parser_t *parser) {
    ast_node_t *left = parse_aditivo(parser);
    return parse_shift_r(parser, left);
}

/* ============================================================================
    NÍVEL 9: EXPRESSÕES RELACIONAIS
   ============================================================================ */

/*
   Relacional → Shift RelacionalR
   RelacionalR → OP_LT Shift RelacionalR
               | OP_LE Shift RelacionalR
               | OP_GT Shift RelacionalR
               | OP_GE Shift RelacionalR
               | ε
*/
ast_node_t* parse_relacional_r(parser_t *parser, ast_node_t *left) {
    token_t token = peek_token(parser);
    
    if (token.tipo == OP_LT || token.tipo == OP_LE || 
        token.tipo == OP_GT || token.tipo == OP_GE) {
        consume_token(parser);
        ast_node_t *right = parse_shift(parser);
        ast_node_t *binary = create_binary_op_node(token.tipo, left, right, token.linha, token.coluna);
        return parse_relacional_r(parser, binary);
    }
    
    return left;  // ε
}

ast_node_t* parse_relacional(parser_t *parser) {
    ast_node_t *left = parse_shift(parser);
    return parse_relacional_r(parser, left);
}

/* ============================================================================
    NÍVEL 8: EXPRESSÕES DE IGUALDADE
   ============================================================================ */

/*
   Igualdade → Relacional IgualdadeR
   IgualdadeR → OP_EQ Relacional IgualdadeR
              | OP_NE Relacional IgualdadeR
              | ε
*/
ast_node_t* parse_igualdade_r(parser_t *parser, ast_node_t *left) {
    token_t token = peek_token(parser);
    
    if (token.tipo == OP_EQ || token.tipo == OP_NE) {
        consume_token(parser);
        ast_node_t *right = parse_relacional(parser);
        ast_node_t *binary = create_binary_op_node(token.tipo, left, right, token.linha, token.coluna);
        return parse_igualdade_r(parser, binary);
    }
    
    return left;  // ε
}

ast_node_t* parse_igualdade(parser_t *parser) {
    ast_node_t *left = parse_relacional(parser);
    return parse_igualdade_r(parser, left);
}

/* ============================================================================
    NÍVEL 7: EXPRESSÕES BITWISE AND
   ============================================================================ */

/*
   BitAnd → Igualdade BitAndR
   BitAndR → OP_BITAND Igualdade BitAndR
           | ε
*/
ast_node_t* parse_bitand_r(parser_t *parser, ast_node_t *left) {
    token_t token = peek_token(parser);

    if (token.tipo == OP_BITAND) {
        consume_token(parser); 
        ast_node_t *right = parse_igualdade(parser);
        ast_node_t *binary = create_binary_op_node(token.tipo, left, right, token.linha, token.coluna);
        return parse_bitand_r(parser, binary);
    }

    return left;
}

ast_node_t* parse_bitand(parser_t *parser) {
    ast_node_t *left = parse_igualdade(parser);
    return parse_bitand_r(parser, left);
}

/* ============================================================================
    NÍVEL 6: EXPRESSÕES BITWISE XOR
   ============================================================================ */

/*
   BitXor → BitAnd BitXorR
   BitXorR → OP_BITXOR BitAnd BitXorR
           | ε
*/
ast_node_t* parse_bitxor_r(parser_t *parser, ast_node_t *left) {
    token_t token = peek_token(parser);

    if (token.tipo == OP_BITXOR) {
        consume_token(parser);
        ast_node_t *right = parse_bitand(parser);
        ast_node_t *binary = create_binary_op_node(token.tipo, left, right, token.linha, token.coluna);
        return parse_bitxor_r(parser, binary);
    }

    return left;  // ε
}

ast_node_t* parse_bitxor(parser_t *parser) {
    ast_node_t *left = parse_bitand(parser);
    return parse_bitxor_r(parser, left);
}

/* ============================================================================
    NÍVEL 5: EXPRESSÕES BITWISE OR
   ============================================================================ */

/*
   BitOr → BitXor BitOrR
   BitOrR → OP_BITOR BitXor BitOrR
          | ε
*/
ast_node_t* parse_bitor_r(parser_t *parser, ast_node_t *left) {
    token_t token = peek_token(parser);

    if (token.tipo == OP_BITOR) {
        consume_token(parser);
        ast_node_t *right = parse_bitxor(parser);
        ast_node_t *binary = create_binary_op_node(token.tipo, left, right, token.linha, token.coluna);
        return parse_bitor_r(parser, binary);
    }

    return left;  // ε
}

ast_node_t* parse_bitor(parser_t *parser) {
    ast_node_t *left = parse_bitxor(parser);
    return parse_bitor_r(parser, left);
}

/* ============================================================================
    NÍVEL 4: EXPRESSÕES LÓGICAS AND
   ============================================================================ */

/*
   LogicoE → BitOr LogicoER
   LogicoER → OP_AND BitOr LogicoER
            | ε
*/
ast_node_t* parse_logico_e_r(parser_t *parser, ast_node_t *left) {
    token_t token = peek_token(parser);
    
    if (token.tipo == OP_AND) {
        consume_token(parser); 
        ast_node_t *right = parse_bitor(parser);
        ast_node_t *binary = create_binary_op_node(token.tipo, left, right, token.linha, token.coluna);
        return parse_logico_e_r(parser, binary);
    }
    
    return left;  // ε
}

ast_node_t* parse_logico_e(parser_t *parser) {
    ast_node_t *left = parse_bitor(parser);
    return parse_logico_e_r(parser, left);
}

/* ============================================================================
    NÍVEL 3: EXPRESSÕES LÓGICAS OR
   ============================================================================ */

/*
   LogicoOu → LogicoE LogicoOuR
   LogicoOuR → OP_OR LogicoE LogicoOuR
             | ε
*/
ast_node_t* parse_logico_ou_r(parser_t *parser, ast_node_t *left) {
    token_t token = peek_token(parser);
    
    if (token.tipo == OP_OR) {
        consume_token(parser);
        ast_node_t *right = parse_logico_e(parser);
        ast_node_t *binary = create_binary_op_node(token.tipo, left, right, token.linha, token.coluna);
        return parse_logico_ou_r(parser, binary);
    }
    
    return left;  // ε
}

ast_node_t* parse_logico_ou(parser_t *parser) {
    ast_node_t *left = parse_logico_e(parser);
    return parse_logico_ou_r(parser, left);
}

/* ============================================================================
    NÍVEL 2: EXPRESSÕES TERNÁRIAS
   ============================================================================ */

/*
   Ternario → LogicoOu TernarioR
   TernarioR → SYM_TERNARY Expressao SYM_COLON Ternario
             | ε
*/
ast_node_t* parse_ternario(parser_t *parser) {
    ast_node_t *condition = parse_logico_ou(parser);

    if (match(parser, SYM_TERNARY)) {
        token_t token = consume_token(parser);
        ast_node_t *true_expr = parse_expressao(parser);
        expect(parser, SYM_COLON);
        ast_node_t *false_expr = parse_ternario(parser);

        ast_node_t *ternary = create_ast_node(AST_TERNARY, NULL, token.linha, token.coluna);
        add_ast_child(ternary, condition);
        add_ast_child(ternary, true_expr);
        add_ast_child(ternary, false_expr);
        return ternary;
    }

    return condition;
}

/* ============================================================================
    NÍVEL 1: EXPRESSÕES DE ATRIBUIÇÃO
   ============================================================================ */
 
/*
   Atribuicao → Ternario AtribuicaoR
   AtribuicaoR → OP_ASSIGN Atribuicao
               | OP_PLUS_ASSIGN Atribuicao
               | OP_MINUS_ASSIGN Atribuicao
               | OP_MULT_ASSIGN Atribuicao
               | OP_DIV_ASSIGN Atribuicao
               | OP_AND_ASSIGN Atribuicao
               | OP_OR_ASSIGN Atribuicao
               | OP_XOR_ASSIGN Atribuicao
               | ε
*/
ast_node_t* parse_atribuicao_r(parser_t *parser, ast_node_t *left) {
    token_t token = peek_token(parser);
    
    if (token.tipo == OP_ASSIGN || token.tipo == OP_PLUS_ASSIGN || 
        token.tipo == OP_MINUS_ASSIGN || token.tipo == OP_MULT_ASSIGN || 
        token.tipo == OP_DIV_ASSIGN || token.tipo == OP_AND_ASSIGN || 
        token.tipo == OP_OR_ASSIGN || token.tipo == OP_XOR_ASSIGN) {
        consume_token(parser);
        ast_node_t *right = parse_atribuicao(parser);
        
        ast_node_t *assign = create_ast_node(AST_ASSIGN, NULL, token.linha, token.coluna);
        assign->operator_type = token.tipo;
        add_ast_child(assign, left);
        add_ast_child(assign, right);
        
        return assign;
    }
    
    return left;  // ε
}

ast_node_t* parse_atribuicao(parser_t *parser) {
    ast_node_t *left = parse_ternario(parser);
    return parse_atribuicao_r(parser, left);
}

/* ============================================================================
   NÍVEL 1: EXPRESSÃO COMPLETA
   ============================================================================ */

/*
   Expressao → Atribuicao
*/
ast_node_t* parse_expressao(parser_t *parser) {
    return parse_atribuicao(parser);
}

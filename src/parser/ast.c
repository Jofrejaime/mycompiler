#define _POSIX_C_SOURCE 200809L
#include "ast.h"
#include "../lexer/tokens.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ============================================================================
   ABSTRACT SYNTAX TREE (AST) - IMPLEMENTAÇÃO
   
   Implementa as funções para criação, manipulação e liberação de nós da AST.
   Requisitos atendidos: 14.1, 14.2, 14.3, 14.4, 14.6, 19.1, 19.3
   ============================================================================ */
#define _POSIX_C_SOURCE 200809L
#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ============================================================================
   ABSTRACT SYNTAX TREE (AST) - IMPLEMENTAÇÃO
   ============================================================================ */

ast_node_t* create_ast_node(node_type_t type, const void *meta, int line, int column) {
    ast_node_t *node = (ast_node_t*)malloc(sizeof(ast_node_t));
    if (!node) return NULL;

    node->type = type;
    node->line = line;
    node->column = column;
    node->operator_type = 0;
    node->data_type = 0;
    node->literal_type = 0;
    node->name = NULL;
    node->children = NULL;
    node->child_count = 0;
    memset(&node->data, 0, sizeof(node->data));

    if (meta) {
        if (type == NODE_VAR_DECL || type == NODE_FUNC_DECL || type == NODE_STRUCT_DECL || type == NODE_TYPEDEF_DECL || type == NODE_PARAM_DECL ||
            type == NODE_GOTO_STMT || type == NODE_LABEL_STMT || type == NODE_PP_DIRECTIVE) {
            node->name = strdup((const char*)meta);
            node->data.decl.name = node->name;
        } else if (type == NODE_IDENTIFIER) {
            node->name = strdup((const char*)meta);
            node->data.expr.identifier = node->name;
        }
    }

    return node;
}

void add_ast_child(ast_node_t *parent, ast_node_t *child) {
    if (!parent || !child) return;

    /* realloc to hold one more child */
    ast_node_t **new_children = (ast_node_t**)realloc(parent->children, sizeof(ast_node_t*) * (parent->child_count + 1));
    if (!new_children) return;
    parent->children = new_children;
    parent->children[parent->child_count++] = child;
}

ast_node_t* create_binary_op_node(int operator, ast_node_t *left, ast_node_t *right, int line, int column) {
    ast_node_t *node = create_ast_node(NODE_BINARY_OP, NULL, line, column);
    if (!node) return NULL;
    node->data.expr.operator = operator;
    node->data.expr.left = left;
    node->data.expr.right = right;
    return node;
}

ast_node_t* create_unary_op_node(int operator, ast_node_t *operand, int line, int column) {
    ast_node_t *node = create_ast_node(NODE_UNARY_OP, NULL, line, column);
    if (!node) return NULL;
    node->data.expr.operator = operator;
    node->data.expr.left = operand;
    return node;
}

ast_node_t* create_identifier_node(const char *name, int line, int column) {
    return create_ast_node(NODE_IDENTIFIER, (void*)name, line, column);
}

ast_node_t* create_literal_node(int literal_type, const char *value, int line, int column) {
    ast_node_t *node = create_ast_node(NODE_LITERAL, NULL, line, column);
    if (!node) return NULL;
    if (value) node->data.expr.value.string_value = strdup(value);
    node->literal_type = literal_type;
    return node;
}

void free_ast(ast_node_t *node) {
    if (!node) return;

    for (int i = 0; i < node->child_count; i++) free_ast(node->children[i]);
    free(node->children);

    switch (node->type) {
        case NODE_VAR_DECL:
        case NODE_FUNC_DECL:
        case NODE_STRUCT_DECL:
        case NODE_TYPEDEF_DECL:
            free(node->data.decl.name);
            free_ast(node->data.decl.initializer);
            free_ast(node->data.decl.function_body);
            if (node->data.decl.parameters) {
                for (int i = 0; i < node->data.decl.param_count; i++) free_ast(node->data.decl.parameters[i]);
                free(node->data.decl.parameters);
            }
            break;

        case NODE_BLOCK:
        case NODE_IF_STMT:
        case NODE_WHILE_STMT:
        case NODE_FOR_STMT:
        case NODE_DO_WHILE_STMT:
        case NODE_RETURN_STMT:
        case NODE_EXPR_STMT:
            free_ast(node->data.stmt.condition);
            free_ast(node->data.stmt.then_branch);
            free_ast(node->data.stmt.else_branch);
            free_ast(node->data.stmt.init);
            free_ast(node->data.stmt.increment);
            if (node->data.stmt.statements) {
                for (int i = 0; i < node->data.stmt.stmt_count; i++) free_ast(node->data.stmt.statements[i]);
                free(node->data.stmt.statements);
            }
            break;

        case NODE_BINARY_OP:
        case NODE_UNARY_OP:
        case NODE_ASSIGN:
        case NODE_CALL:
        case NODE_ARRAY_ACCESS:
        case NODE_MEMBER_ACCESS:
            free_ast(node->data.expr.left);
            free_ast(node->data.expr.right);
            free(node->data.expr.identifier);
            break;

        case NODE_IDENTIFIER:
            free(node->data.expr.identifier);
            break;

        case NODE_LITERAL:
            free(node->data.expr.value.string_value);
            break;

        default:
            break;
    }

    free(node);
}

/* ============================================================================
   HELPER FUNCTIONS - TYPE AND FORMATTING
   ============================================================================ */

const char* data_type_to_string(int data_type) {
    switch (data_type) {
        case KW_INT:     return "int";
        case KW_CHAR:    return "char";
        case KW_FLOAT:   return "float";
        case KW_DOUBLE:  return "double";
        case KW_VOID:    return "void";
        case KW_STRUCT:  return "struct";
        case KW_UNION:   return "union";
        default:         return "unknown";
    }
}

const char* ast_node_type_name(node_type_t type) {
    switch (type) {
        case NODE_PROGRAM: return "PROGRAM";
        case NODE_VAR_DECL: return "VAR_DECL";
        case NODE_FUNC_DECL: return "FUNC_DECL";
        case NODE_STRUCT_DECL: return "STRUCT_DECL";
        case NODE_TYPEDEF_DECL: return "TYPEDEF_DECL";
        case NODE_PARAM_DECL: return "PARAM_DECL";
        case NODE_BLOCK: return "BLOCK";
        case NODE_IF_STMT: return "IF_STMT";
        case NODE_WHILE_STMT: return "WHILE_STMT";
        case NODE_DO_WHILE_STMT: return "DO_WHILE_STMT";
        case NODE_FOR_STMT: return "FOR_STMT";
        case NODE_RETURN_STMT: return "RETURN_STMT";
        case NODE_EXPR_STMT: return "EXPR_STMT";
        case NODE_SWITCH_STMT: return "SWITCH_STMT";
        case NODE_CASE: return "CASE";
        case NODE_DEFAULT: return "DEFAULT";
        case NODE_GOTO_STMT: return "GOTO_STMT";
        case NODE_LABEL_STMT: return "LABEL_STMT";
        case NODE_PP_DIRECTIVE: return "PP_DIRECTIVE";
        case NODE_BINARY_OP: return "BINARY_OP";
        case NODE_UNARY_OP: return "UNARY_OP";
        case NODE_ASSIGN: return "ASSIGN";
        case NODE_TERNARY: return "TERNARY";
        case NODE_CALL: return "CALL";
        case NODE_ARRAY_ACCESS: return "ARRAY_ACCESS";
        case NODE_MEMBER_ACCESS: return "MEMBER_ACCESS";
        case NODE_IDENTIFIER: return "IDENTIFIER";
        case NODE_LITERAL: return "LITERAL";
        case NODE_INIT_LIST: return "INIT_LIST";
        case NODE_BREAK_STMT: return "BREAK_STMT";
        case NODE_CONTINUE_STMT: return "CONTINUE_STMT";
        default: return "UNKNOWN";
    }
}

void print_ast(ast_node_t *node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) printf("  ");
    printf("%s", ast_node_type_name(node->type));

    switch (node->type) {
        case NODE_VAR_DECL:
        case NODE_FUNC_DECL:
        case NODE_STRUCT_DECL:
        case NODE_TYPEDEF_DECL:
        case NODE_PARAM_DECL: {
            if (node->data.decl.name) printf(" '%s'", node->data.decl.name);
            
            /* Print type information for declarations */
            if (node->data_type != 0) {
                printf(" (type=%s", data_type_to_string(node->data_type));
                
                /* Add pointer info */
                if (node->operator_type > 0) {
                    printf(", ptr=%d", node->operator_type);
                }
                
                /* Add array info */
                if (node->data.decl.array_dim_count > 0) {
                    printf(", dims=");
                    for (int i = 0; i < node->data.decl.array_dim_count; i++) {
                        printf("[%d]%s", node->data.decl.array_dimensions[i],
                               (i < node->data.decl.array_dim_count - 1) ? "" : "");
                    }
                }
                printf(")");
            }
            break;
        }
        case NODE_GOTO_STMT:
        case NODE_LABEL_STMT:
        case NODE_PP_DIRECTIVE:
            if (node->name) printf(" '%s'", node->name);
            break;
        case NODE_IDENTIFIER:
            if (node->data.expr.identifier) printf(" '%s'", node->data.expr.identifier);
            break;
        case NODE_LITERAL:
            if (node->data.expr.value.string_value) printf(" '%s'", node->data.expr.value.string_value);
            break;
        case NODE_BINARY_OP:
        case NODE_UNARY_OP:
        case NODE_ASSIGN:
            printf(" (op=%d)", node->data.expr.operator);
            break;
        default:
            break;
    }

    printf(" [%d:%d]\n", node->line, node->column);

    for (int i = 0; i < node->child_count; i++) print_ast(node->children[i], indent + 1);

    switch (node->type) {
        case NODE_VAR_DECL:
        case NODE_FUNC_DECL:
        case NODE_STRUCT_DECL:
            if (node->data.decl.initializer) {
                for (int i = 0; i < indent + 1; i++) printf("  ");
                printf("initializer:\n");
                print_ast(node->data.decl.initializer, indent + 2);
            }
            if (node->data.decl.function_body) {
                for (int i = 0; i < indent + 1; i++) printf("  ");
                printf("body:\n");
                print_ast(node->data.decl.function_body, indent + 2);
            }
            if (node->data.decl.parameters) {
                for (int i = 0; i < indent + 1; i++) printf("  ");
                printf("parameters:\n");
                for (int i = 0; i < node->data.decl.param_count; i++) print_ast(node->data.decl.parameters[i], indent + 2);
            }
            break;
        case NODE_IF_STMT:
        case NODE_WHILE_STMT:
        case NODE_DO_WHILE_STMT:
        case NODE_FOR_STMT:
            if (node->data.stmt.condition) { for (int i = 0; i < indent + 1; i++) printf("  "); printf("condition:\n"); print_ast(node->data.stmt.condition, indent + 2); }
            if (node->data.stmt.init) { for (int i = 0; i < indent + 1; i++) printf("  "); printf("init:\n"); print_ast(node->data.stmt.init, indent + 2); }
            if (node->data.stmt.increment) { for (int i = 0; i < indent + 1; i++) printf("  "); printf("increment:\n"); print_ast(node->data.stmt.increment, indent + 2); }
            if (node->data.stmt.then_branch) { for (int i = 0; i < indent + 1; i++) printf("  "); printf("then:\n"); print_ast(node->data.stmt.then_branch, indent + 2); }
            if (node->data.stmt.else_branch) { for (int i = 0; i < indent + 1; i++) printf("  "); printf("else:\n"); print_ast(node->data.stmt.else_branch, indent + 2); }
            break;
        case NODE_BINARY_OP:
        case NODE_ASSIGN:
        case NODE_ARRAY_ACCESS:
        case NODE_MEMBER_ACCESS:
            if (node->data.expr.left) { for (int i = 0; i < indent + 1; i++) printf("  "); printf("left:\n"); print_ast(node->data.expr.left, indent + 2); }
            if (node->data.expr.right) { for (int i = 0; i < indent + 1; i++) printf("  "); printf("right:\n"); print_ast(node->data.expr.right, indent + 2); }
            break;
        case NODE_UNARY_OP:
            if (node->data.expr.left) { for (int i = 0; i < indent + 1; i++) printf("  "); printf("operand:\n"); print_ast(node->data.expr.left, indent + 2); }
            break;
        default:
            break;
    }
}
 

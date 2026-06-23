#ifndef AST_H
#define AST_H

/* ============================================================================
   ABSTRACT SYNTAX TREE (AST) - HEADER
   
   Define as estruturas e funções para construção da Árvore Sintática Abstrata
   
   Requisitos atendidos: 14.2, 14.3, 14.4
   ============================================================================ */

/* ============================================================================
   TIPOS DE NÓS DA AST
   ============================================================================ */

typedef enum {
    /* Program */
    NODE_PROGRAM,
    
    /* Declarations */
    NODE_VAR_DECL,
    NODE_FUNC_DECL,
    NODE_STRUCT_DECL,
    NODE_TYPEDEF_DECL,
    
    /* Statements */
    NODE_BLOCK,
    NODE_IF_STMT,
    NODE_WHILE_STMT,
    NODE_FOR_STMT,
    NODE_DO_WHILE_STMT,
    NODE_RETURN_STMT,
    NODE_EXPR_STMT,
    NODE_SWITCH_STMT,
    NODE_CASE,
    NODE_DEFAULT,

    /* Preprocessor (kept in AST for debug/traceability) */
    NODE_PP_DIRECTIVE,

    /* Statements (extra) */
    NODE_GOTO_STMT,
    NODE_LABEL_STMT,
    
    /* Expressions */
    NODE_BINARY_OP,      /* +, -, *, /, %, <, >, ==, !=, &&, || */
    NODE_UNARY_OP,       /* -, !, &, * */
   NODE_ASSIGN,         /* =, +=, -=, *=, /= */
   NODE_TERNARY,        /* ? : */
    NODE_CALL,           /* function_call(args) */
    NODE_ARRAY_ACCESS,   /* array[index] */
    NODE_MEMBER_ACCESS,  /* struct.member */
    NODE_IDENTIFIER,
   NODE_LITERAL,        /* int, float, char, string */
   NODE_INIT_LIST       /* { expr, expr, ... } */
   ,NODE_BREAK_STMT
   ,NODE_CONTINUE_STMT
   ,NODE_PARAM_DECL
} node_type_t;

/* Backwards-compatible AST_* aliases used by parser_*.c */
#define AST_PROGRAM        NODE_PROGRAM
#define AST_VAR_DECL       NODE_VAR_DECL
#define AST_FUNC_DECL      NODE_FUNC_DECL
#define AST_STRUCT_DECL    NODE_STRUCT_DECL
#define AST_TYPEDEF_DECL   NODE_TYPEDEF_DECL
#define AST_BLOCK          NODE_BLOCK
#define AST_IF_STMT        NODE_IF_STMT
#define AST_WHILE_STMT     NODE_WHILE_STMT
#define AST_FOR_STMT       NODE_FOR_STMT
#define AST_DO_WHILE_STMT  NODE_DO_WHILE_STMT
#define AST_RETURN_STMT    NODE_RETURN_STMT
#define AST_EXPR_STMT      NODE_EXPR_STMT
#define AST_SWITCH_STMT    NODE_SWITCH_STMT
#define AST_CASE           NODE_CASE
#define AST_DEFAULT        NODE_DEFAULT
#define AST_PP_DIRECTIVE   NODE_PP_DIRECTIVE
#define AST_GOTO_STMT      NODE_GOTO_STMT
#define AST_LABEL_STMT     NODE_LABEL_STMT
#define AST_BINARY_OP      NODE_BINARY_OP
#define AST_UNARY_OP       NODE_UNARY_OP
#define AST_ASSIGN         NODE_ASSIGN
#define AST_TERNARY        NODE_TERNARY
#define AST_CALL           NODE_CALL
#define AST_ARRAY_ACCESS   NODE_ARRAY_ACCESS
#define AST_MEMBER_ACCESS  NODE_MEMBER_ACCESS
#define AST_IDENTIFIER     NODE_IDENTIFIER
#define AST_LITERAL        NODE_LITERAL
#define AST_INIT_LIST      NODE_INIT_LIST

#define AST_BREAK_STMT     NODE_BREAK_STMT
#define AST_CONTINUE_STMT  NODE_CONTINUE_STMT
#define AST_PARAM_DECL     NODE_PARAM_DECL

/* Literal subtype constants expected by parser code */
typedef enum {
   AST_INT_LITERAL,
   AST_FLOAT_LITERAL,
   AST_STRING_LITERAL,
   AST_CHAR_LITERAL
} ast_literal_type_t;

/* ============================================================================
   ESTRUTURA DO NÓ DA AST
   
   Estrutura unificada para representar todos os tipos de nós da árvore
   sintática abstrata. Usa union para otimizar memória, armazenando apenas
   os dados relevantes para cada tipo de nó.
   
   Campos principais:
   - type: tipo do nó (NODE_PROGRAM, NODE_VAR_DECL, etc.)
   - line, column: posição no código-fonte para mensagens de erro
   - data: union contendo dados específicos do tipo de nó
   - children: array genérico de filhos (usado quando necessário)
   - child_count: número de filhos no array
   ============================================================================ */

typedef struct ast_node_s {
    node_type_t type;               /* Tipo do nó */
    int line;                       /* Linha no código-fonte */
    int column;                     /* Coluna no código-fonte */
   /* Backwards-compatible convenience fields used by parser code */
   int operator_type;               /* operador associado (quando aplicável) */
   int data_type;                   /* tipo de dado associado (quando aplicável) */
   int literal_type;                /* subtipo de literal (AST_INT_LITERAL, ... ) */
   char *name;                      /* nome/identificador auxiliar */
    
    /* Union para diferentes tipos de nós */
    union {
        /* For declarations (NODE_VAR_DECL, NODE_FUNC_DECL, NODE_STRUCT_DECL) */
        struct {
            char *name;                         /* Nome do identificador */
            int data_type;                      /* KW_INT, KW_CHAR, KW_FLOAT, etc. */
            int is_pointer;                     /* Nível de indireção (0 = não é ponteiro) */
            int array_dimensions[8];            /* Dimensões do array (se aplicável) */
            int array_dim_count;                /* Número de dimensões */
            struct ast_node_s *initializer;     /* Expressão de inicialização */
            struct ast_node_s *function_body;   /* Corpo da função (NODE_BLOCK) */
            struct ast_node_s **parameters;     /* Array de parâmetros da função */
            int param_count;                    /* Número de parâmetros */
        } decl;
        
        /* For statements (NODE_IF_STMT, NODE_WHILE_STMT, NODE_FOR_STMT, etc.) */
        struct {
            struct ast_node_s *condition;       /* Condição (if, while, for, do-while) */
            struct ast_node_s *then_branch;     /* Ramo then (if) ou corpo (loops) */
            struct ast_node_s *else_branch;     /* Ramo else (if) */
            struct ast_node_s *init;            /* Inicialização (for) */
            struct ast_node_s *increment;       /* Incremento (for) */
            struct ast_node_s **statements;     /* Array de instruções (blocks) */
            int stmt_count;                     /* Número de instruções */
        } stmt;
        
        /* For expressions (NODE_BINARY_OP, NODE_UNARY_OP, NODE_ASSIGN, etc.) */
        struct {
            int operator;                       /* Tipo do operador (token type) */
            struct ast_node_s *left;            /* Operando esquerdo */
            struct ast_node_s *right;           /* Operando direito */
            char *identifier;                   /* Nome do identificador */
            union {
                int int_value;                  /* Valor inteiro */
                double float_value;             /* Valor float */
                char char_value;                /* Valor char */
                char *string_value;             /* Valor string */
            } value;
        } expr;
    } data;
    
    /* Children array (generic) - usado para nós que precisam de lista flexível de filhos */
    struct ast_node_s **children;
    int child_count;
} ast_node_t;

/* ============================================================================
   FUNÇÕES DE CRIAÇÃO DE NÓS
   ============================================================================ */

/*
   Criar nó genérico da AST
   
   Entrada: type - tipo do nó
            line, column - posição no código-fonte
   Saída: ponteiro para nó criado, ou NULL se erro
*/
ast_node_t* create_ast_node(node_type_t type, const void *meta, int line, int column);

/*
   Adicionar filho a um nó
   
   Entrada: parent - nó pai
            child - nó filho a adicionar
   Saída: void
*/
void add_ast_child(ast_node_t *parent, ast_node_t *child);

/*
   Criar nó de operador binário
   
   Entrada: operator - tipo do operador (OP_PLUS, OP_MINUS, etc.)
            left - operando esquerdo
            right - operando direito
            line, column - posição no código-fonte
   Saída: ponteiro para nó criado
*/
ast_node_t* create_binary_op_node(int operator, ast_node_t *left, ast_node_t *right, 
                                   int line, int column);

/*
   Criar nó de operador unário
   
   Entrada: operator - tipo do operador (OP_NOT, OP_BITNOT, etc.)
            operand - operando
            line, column - posição no código-fonte
   Saída: ponteiro para nó criado
*/
ast_node_t* create_unary_op_node(int operator, ast_node_t *operand, 
                                  int line, int column);

/*
   Criar nó de identificador
   
   Entrada: name - nome do identificador
            line, column - posição no código-fonte
   Saída: ponteiro para nó criado
*/
ast_node_t* create_identifier_node(const char *name, int line, int column);

/*
   Criar nó de literal
   
   Entrada: value - valor do literal como string
            line, column - posição no código-fonte
   Saída: ponteiro para nó criado
*/
ast_node_t* create_literal_node(int literal_type, const char *value, int line, int column);

/* ============================================================================
   FUNÇÕES DE LIBERAÇÃO DE MEMÓRIA
   ============================================================================ */

/*
   Liberar árvore AST recursivamente
   
   Entrada: node - raiz da árvore a liberar
   Saída: void
*/
void free_ast(ast_node_t *node);

/* ============================================================================
   FUNÇÕES DE IMPRESSÃO (DEBUG)
   ============================================================================ */

#ifdef DEBUG_AST
void print_ast(ast_node_t *node, int indent);
#else
static inline void print_ast(ast_node_t *node, int indent) { (void)node; (void)indent; }
#endif

const char* ast_node_type_name(node_type_t type);

#endif /* AST_H */

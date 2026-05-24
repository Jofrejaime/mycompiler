# Design Document - Analisador Sintático (Parser)

## Overview

Este documento especifica o design técnico da **Fase 2 do compilador: Análise Sintática (Parser)**. O parser implementa o método **Top-Down Recursivo Descendente sem backtracking (LL(1))**, consumindo tokens da tabela de símbolos gerada pelo Lexer e construindo uma Abstract Syntax Tree (AST) enquanto enriquece a tabela de símbolos com informações semânticas.

**Objetivos principais:**
- Validar a sintaxe do código-fonte C conforme gramática EBNF transformada
- Construir AST representando a estrutura hierárquica do programa
- Enriquecer tabela de símbolos com tipos, escopos e endereços de memória
- Reportar erros sintáticos de forma clara e precisa
- Preparar dados para a fase de análise semântica

**Método de parsing:** Recursive Descent Predictive Parser (LL(1))
- Cada não-terminal da gramática → função C
- Decisões de parsing baseadas em FIRST sets
- Sem backtracking (análise preditiva)
- Uma única passagem sobre os tokens

**Entrada:** `tabla_simbolos_t` preenchida pelo Lexer com tokens

**Saída:** 
- AST (Abstract Syntax Tree) representando o programa
- `tabla_simbolos_t` enriquecida com informações semânticas

---

## Architecture

### System Components

```
┌─────────────────────────────────────────────────────────────┐
│                      PARSER SYSTEM                          │
│                                                             │
│  ┌──────────────┐      ┌──────────────┐                   │
│  │   Parser     │      │  AST Builder │                   │
│  │   Engine     │─────▶│              │                   │
│  │              │      │  (ast_node_t)│                   │
│  └──────┬───────┘      └──────────────┘                   │
│         │                                                   │
│         │ consume_token()                                  │
│         │ peek_token()                                     │
│         ▼                                                   │
│  ┌──────────────┐      ┌──────────────┐                   │
│  │ Symbol Table │      │ Scope Stack  │                   │
│  │ (from Lexer) │◀────▶│ (scope_t)    │                   │
│  │tabla_simbolos│      │              │                   │
│  └──────────────┘      └──────────────┘                   │
│         ▲                      │                            │
│         │                      │                            │
│         │ enrich_symbol()      │ enter_scope()             │
│         │                      │ exit_scope()              │
│         └──────────────────────┘                            │
└─────────────────────────────────────────────────────────────┘
```


### Core Data Structures

#### 1. Parser State Structure

```c
typedef struct parser_s {
    tabla_simbolos_t *symbol_table;    /* Tabela de símbolos do Lexer */
    int current_position;               /* Posição atual de leitura */
    
    /* Scope management */
    scope_t **scope_stack;              /* Pilha de escopos */
    int scope_stack_size;               /* Tamanho atual da pilha */
    int scope_stack_capacity;           /* Capacidade da pilha */
    int next_scope_id;                  /* Próximo ID de escopo */
    
    /* Memory allocation */
    int next_memory_address;            /* Próximo endereço de memória */
    
    /* Error handling */
    int error_count;                    /* Contador de erros sintáticos */
    int panic_mode;                     /* Flag de modo pânico */
    
    /* AST root */
    ast_node_t *ast_root;               /* Raiz da árvore sintática */
} parser_t;
```

#### 2. AST Node Structure

```c
typedef enum {
    /* Declarations */
    NODE_PROGRAM,
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
    
    /* Expressions */
    NODE_BINARY_OP,      /* +, -, *, /, %, <, >, ==, !=, &&, || */
    NODE_UNARY_OP,       /* -, !, &, * */
    NODE_ASSIGN,         /* =, +=, -=, *=, /= */
    NODE_CALL,           /* function_call(args) */
    NODE_ARRAY_ACCESS,   /* array[index] */
    NODE_MEMBER_ACCESS,  /* struct.member */
    NODE_IDENTIFIER,
    NODE_LITERAL         /* int, float, char, string */
} node_type_t;

typedef struct ast_node_s {
    node_type_t type;
    int line;
    int column;
    
    union {
        /* For declarations */
        struct {
            char *name;
            int data_type;           /* KW_INT, KW_CHAR, KW_FLOAT, etc. */
            int is_pointer;
            int array_dimensions[8];
            int array_dim_count;
            ast_node_t *initializer;
            ast_node_t *function_body;  /* For function declarations */
            ast_node_t **parameters;    /* For function parameters */
            int param_count;
        } decl;
        
        /* For statements */
        struct {
            ast_node_t *condition;
            ast_node_t *then_branch;
            ast_node_t *else_branch;
            ast_node_t *init;        /* For 'for' loop */
            ast_node_t *increment;   /* For 'for' loop */
            ast_node_t **statements; /* For blocks */
            int stmt_count;
        } stmt;
        
        /* For expressions */
        struct {
            int operator;            /* Token type of operator */
            ast_node_t *left;
            ast_node_t *right;
            char *identifier;
            union {
                int int_value;
                double float_value;
                char char_value;
                char *string_value;
            } value;
        } expr;
    } data;
    
    ast_node_t **children;
    int child_count;
} ast_node_t;
```


#### 3. Scope Structure

```c
typedef struct scope_s {
    int scope_id;                    /* Identificador único do escopo */
    struct scope_s *parent;          /* Escopo pai (NULL para global) */
    
    /* Local symbol table for this scope */
    char **local_symbols;            /* Array de nomes de símbolos */
    int symbol_count;
    int symbol_capacity;
} scope_t;
```

#### 4. Extended Symbol Information

A estrutura `token_t` existente será estendida com campos semânticos:

```c
typedef struct symbol_info_s {
    /* Type information */
    int data_type;              /* KW_INT, KW_CHAR, KW_FLOAT, KW_VOID, KW_STRUCT */
    int is_pointer;             /* 0 = não, 1+ = nível de indireção */
    
    /* Variable classification */
    enum {
        VAR_GLOBAL,
        VAR_LOCAL,
        VAR_PARAMETER,
        VAR_STRUCT_FIELD
    } variable_type;
    
    /* Scope information */
    int scope_id;               /* ID do escopo onde foi declarada */
    
    /* Memory allocation */
    int memory_address;         /* Endereço de memória atribuído */
    int size_bytes;             /* Tamanho em bytes */
    
    /* Array information */
    int is_array;
    int array_dimensions[8];    /* Até 8 dimensões */
    int array_dim_count;
    
    /* Function information */
    int is_function;
    int function_return_type;
    int function_param_types[32];  /* Tipos dos parâmetros */
    int function_param_count;
} symbol_info_t;
```

**Nota:** Esta estrutura será adicionada à `token_t` existente ou mantida em tabela paralela indexada por nome de símbolo.

---

## Grammar Transformation for LL(1)

### Original Grammar Analysis

A gramática original em `Syntactic-Grammar.md` contém **recursividade à esquerda** em várias produções de expressões, o que impede parsing preditivo LL(1). As seguintes produções precisam ser transformadas:

**Recursividade à esquerda imediata identificada:**

1. **Expressões de atribuição:**
   ```ebnf
   <atribuicao> ::= <logico_ou> <atribuicao_r>
   <atribuicao_r> ::= TOKEN_ASSIGN <atribuicao>
                    | TOKEN_PLUS_ASSIGN <atribuicao>
                    | ...
                    | ε
   ```
   ✅ **Já está transformada** (usa produção auxiliar `_r`)

2. **Expressões lógicas OR:**
   ```ebnf
   <logico_ou> ::= <logico_e> <logico_ou_r>
   <logico_ou_r> ::= TOKEN_OR <logico_e> <logico_ou_r>
                   | ε
   ```
   ✅ **Já está transformada**

3. **Expressões lógicas AND:**
   ```ebnf
   <logico_e> ::= <igualdade> <logico_e_r>
   <logico_e_r> ::= TOKEN_AND <igualdade> <logico_e_r>
                  | ε
   ```
   ✅ **Já está transformada**

4. **Expressões de igualdade:**
   ```ebnf
   <igualdade> ::= <relacional> <igualdade_r>
   <igualdade_r> ::= TOKEN_EQ <relacional> <igualdade_r>
                   | TOKEN_NEQ <relacional> <igualdade_r>
                   | ε
   ```
   ✅ **Já está transformada**

5. **Expressões relacionais:**
   ```ebnf
   <relacional> ::= <aditivo> <relacional_r>
   <relacional_r> ::= TOKEN_LT <aditivo> <relacional_r>
                    | TOKEN_GT <aditivo> <relacional_r>
                    | TOKEN_LEQ <aditivo> <relacional_r>
                    | TOKEN_GEQ <aditivo> <relacional_r>
                    | ε
   ```
   ✅ **Já está transformada**

6. **Expressões aditivas:**
   ```ebnf
   <aditivo> ::= <multiplicativo> <aditivo_r>
   <aditivo_r> ::= TOKEN_PLUS <multiplicativo> <aditivo_r>
                 | TOKEN_MINUS <multiplicativo> <aditivo_r>
                 | ε
   ```
   ✅ **Já está transformada**

7. **Expressões multiplicativas:**
   ```ebnf
   <multiplicativo> ::= <unario> <multiplicativo_r>
   <multiplicativo_r> ::= TOKEN_STAR <unario> <multiplicativo_r>
                        | TOKEN_SLASH <unario> <multiplicativo_r>
                        | TOKEN_PERCENT <unario> <multiplicativo_r>
                        | ε
   ```
   ✅ **Já está transformada**


8. **Expressões pós-fixas:**
   ```ebnf
   <pos_fixo> ::= <primario> <pos_fixo_r>
   <pos_fixo_r> ::= TOKEN_LBRACKET <expressao> TOKEN_RBRACKET <pos_fixo_r>
                  | TOKEN_DOT TOKEN_IDENTIFIER <pos_fixo_r>
                  | TOKEN_LPAREN <argumentos_opcionais> TOKEN_RPAREN <pos_fixo_r>
                  | ε
   ```
   ✅ **Já está transformada**

### Conclusion on Grammar Transformation

**A gramática fornecida em `Syntactic-Grammar.md` já está completamente transformada para LL(1)!**

Todas as produções recursivas à esquerda foram eliminadas usando o padrão:
- Produção original: `A ::= A α | β`
- Transformação: `A ::= β A'` e `A' ::= α A' | ε`

As produções auxiliares usam sufixo `_r` (ex: `aditivo_r`, `multiplicativo_r`, `logico_ou_r`).

**Não são necessárias transformações adicionais.**

---

## FIRST Sets Calculation

Para garantir parsing preditivo LL(1), calculamos os FIRST sets de cada não-terminal. Isso permite decidir qual produção seguir baseado no token atual (lookahead).

### FIRST Sets for Key Non-Terminals

| Non-Terminal | FIRST Set |
|--------------|-----------|
| `<programa>` | `{TOKEN_HASH, KW_INT, KW_CHAR, KW_FLOAT, KW_VOID, KW_STRUCT, KW_TYPEDEF, TOKEN_IDENTIFIER, TOKEN_EOF}` |
| `<declaracao_global>` | `{TOKEN_HASH, KW_INT, KW_CHAR, KW_FLOAT, KW_VOID, KW_STRUCT, KW_TYPEDEF, TOKEN_IDENTIFIER}` |
| `<diretiva_include>` | `{TOKEN_HASH}` |
| `<diretiva_define>` | `{TOKEN_HASH}` |
| `<especificador_tipo>` | `{KW_INT, KW_CHAR, KW_FLOAT, KW_VOID, KW_STRUCT, TOKEN_IDENTIFIER}` |
| `<declaracao_typedef>` | `{KW_TYPEDEF}` |
| `<bloco>` | `{TOKEN_LBRACE}` |
| `<instrucao>` | `{TOKEN_IF, TOKEN_WHILE, TOKEN_FOR, TOKEN_DO, TOKEN_RETURN, TOKEN_LBRACE, TOKEN_SEMICOLON, TOKEN_IDENTIFIER, TOKEN_INT_LITERAL, TOKEN_FLOAT_LITERAL, TOKEN_CHAR_LITERAL, TOKEN_STRING_LITERAL, TOKEN_LPAREN, TOKEN_NOT, TOKEN_MINUS, TOKEN_AMP, TOKEN_STAR}` |
| `<instrucao_if>` | `{TOKEN_IF}` |
| `<instrucao_while>` | `{TOKEN_WHILE}` |
| `<instrucao_for>` | `{TOKEN_FOR}` |
| `<instrucao_do>` | `{TOKEN_DO}` |
| `<instrucao_return>` | `{TOKEN_RETURN}` |
| `<expressao>` | `{TOKEN_IDENTIFIER, TOKEN_INT_LITERAL, TOKEN_FLOAT_LITERAL, TOKEN_CHAR_LITERAL, TOKEN_STRING_LITERAL, TOKEN_LPAREN, TOKEN_NOT, TOKEN_MINUS, TOKEN_AMP, TOKEN_STAR}` |
| `<primario>` | `{TOKEN_IDENTIFIER, TOKEN_INT_LITERAL, TOKEN_FLOAT_LITERAL, TOKEN_CHAR_LITERAL, TOKEN_STRING_LITERAL, TOKEN_LPAREN}` |


### Predictive Parsing Table (Excerpt)

Para decisões de parsing, usamos o token atual (lookahead) para escolher a produção:

**Exemplo: `<instrucao>`**

| Current Token | Production to Use |
|---------------|-------------------|
| `TOKEN_IF` | `<instrucao_if>` |
| `TOKEN_WHILE` | `<instrucao_while>` |
| `TOKEN_FOR` | `<instrucao_for>` |
| `TOKEN_DO` | `<instrucao_do>` |
| `TOKEN_RETURN` | `<instrucao_return>` |
| `TOKEN_LBRACE` | `<bloco>` |
| `TOKEN_SEMICOLON` | `TOKEN_SEMICOLON` (empty statement) |
| `TOKEN_IDENTIFIER`, literals, `TOKEN_LPAREN`, unary ops | `<instrucao_expressao>` |

**Exemplo: `<especificador_tipo>`**

| Current Token | Production to Use |
|---------------|-------------------|
| `KW_INT` | `TOKEN_INT` |
| `KW_CHAR` | `TOKEN_CHAR` |
| `KW_FLOAT` | `TOKEN_FLOAT` |
| `KW_VOID` | `TOKEN_VOID` |
| `KW_STRUCT` | `TOKEN_STRUCT <nome_ou_corpo_struct>` |
| `TOKEN_IDENTIFIER` | `TOKEN_IDENTIFIER` (typedef name) |

### LL(1) Verification

**Disjoint FIRST Sets:** Para cada não-terminal com múltiplas produções alternativas, os FIRST sets das alternativas são disjuntos, garantindo parsing preditivo sem ambiguidade.

**Exemplo:** `<instrucao>` tem 8 alternativas, cada uma com FIRST set disjunto:
- `TOKEN_IF` → apenas `<instrucao_if>`
- `TOKEN_WHILE` → apenas `<instrucao_while>`
- `TOKEN_FOR` → apenas `<instrucao_for>`
- etc.

✅ **A gramática é LL(1) válida.**

---

## Components and Interfaces

### Parser Engine Functions

#### Token Consumption Interface

```c
/* Peek at current token without consuming */
token_t peek_token(parser_t *parser);

/* Consume current token and advance position */
token_t consume_token(parser_t *parser);

/* Check if current token matches expected type */
int match(parser_t *parser, int token_type);

/* Consume token if it matches, otherwise report error */
void expect(parser_t *parser, int token_type);
```

**Implementation details:**
- `peek_token()`: Returns `symbol_table->tokens[current_position]`
- `consume_token()`: Returns current token and increments `current_position`
- `match()`: Returns 1 if `peek_token().tipo == token_type`, else 0
- `expect()`: Calls `consume_token()` if match, else calls `syntax_error()`

#### Parsing Functions (Non-Terminal → Function Mapping)

Cada não-terminal da gramática é implementado como uma função:

```c
/* Entry point */
ast_node_t* parse_programa(parser_t *parser);

/* Global declarations */
void parse_lista_declaracoes_globais(parser_t *parser, ast_node_t *program_node);
void parse_declaracao_global(parser_t *parser, ast_node_t *program_node);
void parse_diretiva_include(parser_t *parser);
void parse_diretiva_define(parser_t *parser);
void parse_declaracao_typedef(parser_t *parser);

/* Type specifiers */
int parse_especificador_tipo(parser_t *parser);
void parse_nome_ou_corpo_struct(parser_t *parser, ast_node_t *struct_node);

/* Declarations */
ast_node_t* parse_declaracao_geral(parser_t *parser, int type_spec);
ast_node_t* parse_declaracao_variavel_local(parser_t *parser);
ast_node_t* parse_declaracao_funcao(parser_t *parser, int return_type, char *func_name);

/* Statements */
ast_node_t* parse_bloco(parser_t *parser);
ast_node_t* parse_instrucao(parser_t *parser);
ast_node_t* parse_instrucao_if(parser_t *parser);
ast_node_t* parse_instrucao_while(parser_t *parser);
ast_node_t* parse_instrucao_for(parser_t *parser);
ast_node_t* parse_instrucao_do(parser_t *parser);
ast_node_t* parse_instrucao_return(parser_t *parser);

/* Expressions (with precedence) */
ast_node_t* parse_expressao(parser_t *parser);
ast_node_t* parse_atribuicao(parser_t *parser);
ast_node_t* parse_logico_ou(parser_t *parser);
ast_node_t* parse_logico_e(parser_t *parser);
ast_node_t* parse_igualdade(parser_t *parser);
ast_node_t* parse_relacional(parser_t *parser);
ast_node_t* parse_aditivo(parser_t *parser);
ast_node_t* parse_multiplicativo(parser_t *parser);
ast_node_t* parse_unario(parser_t *parser);
ast_node_t* parse_pos_fixo(parser_t *parser);
ast_node_t* parse_primario(parser_t *parser);

/* Auxiliary productions (for right recursion elimination) */
void parse_atribuicao_r(parser_t *parser, ast_node_t **left);
void parse_logico_ou_r(parser_t *parser, ast_node_t **left);
void parse_logico_e_r(parser_t *parser, ast_node_t **left);
void parse_igualdade_r(parser_t *parser, ast_node_t **left);
void parse_relacional_r(parser_t *parser, ast_node_t **left);
void parse_aditivo_r(parser_t *parser, ast_node_t **left);
void parse_multiplicativo_r(parser_t *parser, ast_node_t **left);
void parse_pos_fixo_r(parser_t *parser, ast_node_t **left);
```


#### Scope Management Functions

```c
/* Create new scope and push to stack */
void enter_scope(parser_t *parser);

/* Pop current scope from stack */
void exit_scope(parser_t *parser);

/* Get current scope ID */
int get_current_scope_id(parser_t *parser);

/* Lookup symbol in current scope and parent scopes */
symbol_info_t* lookup_symbol(parser_t *parser, const char *name);

/* Add symbol to current scope */
void add_symbol_to_scope(parser_t *parser, const char *name);
```

#### Symbol Table Enrichment Functions

```c
/* Enrich symbol with type information */
void enrich_symbol_type(parser_t *parser, const char *name, int data_type, int is_pointer);

/* Enrich symbol with scope information */
void enrich_symbol_scope(parser_t *parser, const char *name, int scope_id, int var_type);

/* Enrich symbol with memory address */
void enrich_symbol_memory(parser_t *parser, const char *name, int address, int size);

/* Enrich symbol with array information */
void enrich_symbol_array(parser_t *parser, const char *name, int *dimensions, int dim_count);

/* Enrich symbol with function information */
void enrich_symbol_function(parser_t *parser, const char *name, int return_type, 
                            int *param_types, int param_count);
```

#### AST Construction Functions

```c
/* Create AST node */
ast_node_t* create_ast_node(node_type_t type, int line, int column);

/* Add child to AST node */
void add_ast_child(ast_node_t *parent, ast_node_t *child);

/* Create binary operator node */
ast_node_t* create_binary_op_node(int operator, ast_node_t *left, ast_node_t *right);

/* Create unary operator node */
ast_node_t* create_unary_op_node(int operator, ast_node_t *operand);

/* Create identifier node */
ast_node_t* create_identifier_node(const char *name);

/* Create literal node */
ast_node_t* create_literal_node(token_t token);

/* Free AST tree */
void free_ast(ast_node_t *root);
```

#### Error Handling Functions

```c
/* Report syntax error */
void syntax_error(parser_t *parser, const char *expected, const char *found);

/* Enter panic mode recovery */
void panic_mode_recovery(parser_t *parser);

/* Synchronize to next safe point */
void synchronize(parser_t *parser);
```

---

## Data Models

### AST Node Types and Structure

A AST é construída durante o parsing usando uma abordagem **bottom-up**: nós folha (literais, identificadores) são criados primeiro, depois combinados em nós de expressões, instruções e declarações.

**Hierarquia de nós:**

```
NODE_PROGRAM (raiz)
├── NODE_VAR_DECL (declarações globais)
├── NODE_FUNC_DECL (funções)
│   └── NODE_BLOCK (corpo da função)
│       ├── NODE_VAR_DECL (variáveis locais)
│       ├── NODE_IF_STMT
│       │   ├── NODE_BINARY_OP (condição)
│       │   ├── NODE_BLOCK (then)
│       │   └── NODE_BLOCK (else)
│       ├── NODE_WHILE_STMT
│       ├── NODE_RETURN_STMT
│       └── NODE_EXPR_STMT
│           └── NODE_ASSIGN
│               ├── NODE_IDENTIFIER (left)
│               └── NODE_BINARY_OP (right)
└── NODE_STRUCT_DECL
```

### Symbol Table Enrichment Model

Durante o parsing, a tabela de símbolos é enriquecida progressivamente:

**Fase 1 (Lexer):** Token básico
```
{tipo: TK_ID, lexeme: "x", linha: 10, coluna: 5}
```

**Fase 2 (Parser):** Enriquecimento semântico
```
{
  tipo: TK_ID, 
  lexeme: "x", 
  linha: 10, 
  coluna: 5,
  /* Informações adicionadas pelo parser: */
  data_type: KW_INT,
  is_pointer: 0,
  variable_type: VAR_LOCAL,
  scope_id: 3,
  memory_address: 1024,
  size_bytes: 4,
  is_array: 0
}
```

### Memory Address Allocation Model

Endereços de memória são atribuídos sequencialmente durante parsing:

- **Variáveis globais:** Endereços começam em 0
- **Variáveis locais:** Endereços começam em 0 dentro de cada função
- **Parâmetros:** Endereços negativos (convenção de pilha)

**Algoritmo de cálculo:**
```
address = next_memory_address
next_memory_address += size_of_type(data_type) * array_total_elements
```

**Tamanhos de tipos:**
- `int`: 4 bytes
- `char`: 1 byte
- `float`: 4 bytes
- `pointer`: 8 bytes (arquitetura 64-bit)
- `struct`: soma dos campos
- `union`: máximo dos campos

---

## Pseudocode for Critical Functions

### 1. parse_programa (Entry Point)

```
FUNCTION parse_programa(parser):
    // <programa> ::= <lista_declaracoes_globais> TOKEN_EOF
    
    // Create root AST node
    program_node = create_ast_node(NODE_PROGRAM, 0, 0)
    parser.ast_root = program_node
    
    // Initialize global scope
    enter_scope(parser)  // scope_id = 0 (global)
    
    // Parse global declarations
    parse_lista_declaracoes_globais(parser, program_node)
    
    // Expect end of file
    expect(parser, TK_EOF)
    
    // Exit global scope
    exit_scope(parser)
    
    RETURN program_node
END FUNCTION
```

### 2. parse_declaracao_variavel (Symbol Table Enrichment)

```
FUNCTION parse_declaracao_variavel_local(parser):
    // <declaracao_variavel_local> ::= <especificador_tipo> <asteriscos> 
    //                                  TOKEN_IDENTIFIER <sufixo_array_opcional> 
    //                                  <inicializacao_opcional> <mais_declaradores> 
    //                                  TOKEN_SEMICOLON
    
    // Parse type specifier
    data_type = parse_especificador_tipo(parser)
    
    // Parse pointer levels
    pointer_level = 0
    WHILE match(parser, OP_MULT):
        consume_token(parser)
        pointer_level++
    END WHILE
    
    // Parse identifier
    expect(parser, TK_ID)
    var_name = previous_token.lexeme
    var_line = previous_token.linha
    var_column = previous_token.coluna
    
    // Parse array dimensions
    array_dims = []
    WHILE match(parser, SYM_LBRACKET):
        consume_token(parser)
        size_expr = parse_expressao(parser)
        array_dims.append(evaluate_constant_expr(size_expr))
        expect(parser, SYM_RBRACKET)
    END WHILE
    
    // Calculate size
    base_size = sizeof_type(data_type)
    IF pointer_level > 0:
        total_size = 8  // pointer size
    ELSE IF array_dims.length > 0:
        total_size = base_size
        FOR EACH dim IN array_dims:
            total_size *= dim
        END FOR
    ELSE:
        total_size = base_size
    END IF
    
    // Allocate memory address
    memory_addr = parser.next_memory_address
    parser.next_memory_address += total_size
    
    // Enrich symbol table
    enrich_symbol_type(parser, var_name, data_type, pointer_level)
    enrich_symbol_scope(parser, var_name, get_current_scope_id(parser), VAR_LOCAL)
    enrich_symbol_memory(parser, var_name, memory_addr, total_size)
    IF array_dims.length > 0:
        enrich_symbol_array(parser, var_name, array_dims, array_dims.length)
    END IF
    
    // Add to current scope
    add_symbol_to_scope(parser, var_name)
    
    // Parse optional initialization
    initializer = NULL
    IF match(parser, OP_ASSIGN):
        consume_token(parser)
        initializer = parse_expressao(parser)
    END IF
    
    // Create AST node
    decl_node = create_ast_node(NODE_VAR_DECL, var_line, var_column)
    decl_node.data.decl.name = var_name
    decl_node.data.decl.data_type = data_type
    decl_node.data.decl.is_pointer = pointer_level
    decl_node.data.decl.array_dimensions = array_dims
    decl_node.data.decl.initializer = initializer
    
    // Parse more declarators (int x, y, z;)
    WHILE match(parser, SYM_COMMA):
        consume_token(parser)
        // Repeat process for next variable...
    END WHILE
    
    expect(parser, SYM_SEMICOLON)
    
    RETURN decl_node
END FUNCTION
```


### 3. parse_bloco (Scope Management)

```
FUNCTION parse_bloco(parser):
    // <bloco> ::= TOKEN_LBRACE <lista_itens_bloco> TOKEN_RBRACE
    
    expect(parser, SYM_LBRACE)
    
    // Enter new scope
    enter_scope(parser)
    
    // Create block AST node
    block_node = create_ast_node(NODE_BLOCK, previous_token.linha, previous_token.coluna)
    
    // Parse block items (declarations and statements)
    WHILE NOT match(parser, SYM_RBRACE) AND NOT match(parser, TK_EOF):
        IF is_type_specifier(peek_token(parser)):
            // Local variable declaration
            decl_node = parse_declaracao_variavel_local(parser)
            add_ast_child(block_node, decl_node)
        ELSE:
            // Statement
            stmt_node = parse_instrucao(parser)
            add_ast_child(block_node, stmt_node)
        END IF
    END WHILE
    
    expect(parser, SYM_RBRACE)
    
    // Exit scope
    exit_scope(parser)
    
    RETURN block_node
END FUNCTION

FUNCTION is_type_specifier(token):
    RETURN token.tipo IN {KW_INT, KW_CHAR, KW_FLOAT, KW_VOID, KW_STRUCT, TK_ID}
END FUNCTION
```

### 4. parse_expressao (Operator Precedence)

```
FUNCTION parse_expressao(parser):
    // <expressao> ::= <atribuicao>
    RETURN parse_atribuicao(parser)
END FUNCTION

FUNCTION parse_atribuicao(parser):
    // <atribuicao> ::= <logico_ou> <atribuicao_r>
    
    left = parse_logico_ou(parser)
    parse_atribuicao_r(parser, &left)
    
    RETURN left
END FUNCTION

FUNCTION parse_atribuicao_r(parser, left_ptr):
    // <atribuicao_r> ::= TOKEN_ASSIGN <atribuicao>
    //                  | TOKEN_PLUS_ASSIGN <atribuicao>
    //                  | ...
    //                  | ε
    
    IF match(parser, OP_ASSIGN) OR match(parser, OP_PLUS_ASSIGN) OR 
       match(parser, OP_MINUS_ASSIGN) OR match(parser, OP_MULT_ASSIGN) OR
       match(parser, OP_DIV_ASSIGN):
        
        op_token = consume_token(parser)
        right = parse_atribuicao(parser)
        
        // Create assignment node
        assign_node = create_ast_node(NODE_ASSIGN, op_token.linha, op_token.coluna)
        assign_node.data.expr.operator = op_token.tipo
        assign_node.data.expr.left = *left_ptr
        assign_node.data.expr.right = right
        
        *left_ptr = assign_node
    END IF
    // else: ε production, do nothing
END FUNCTION

FUNCTION parse_logico_ou(parser):
    // <logico_ou> ::= <logico_e> <logico_ou_r>
    
    left = parse_logico_e(parser)
    parse_logico_ou_r(parser, &left)
    
    RETURN left
END FUNCTION

FUNCTION parse_logico_ou_r(parser, left_ptr):
    // <logico_ou_r> ::= TOKEN_OR <logico_e> <logico_ou_r>
    //                 | ε
    
    WHILE match(parser, OP_OR):
        op_token = consume_token(parser)
        right = parse_logico_e(parser)
        
        // Create binary operator node
        op_node = create_binary_op_node(op_token.tipo, *left_ptr, right)
        op_node.line = op_token.linha
        op_node.column = op_token.coluna
        
        *left_ptr = op_node
    END WHILE
END FUNCTION
```


FUNCTION parse_aditivo(parser):
    // <aditivo> ::= <multiplicativo> <aditivo_r>
    
    left = parse_multiplicativo(parser)
    parse_aditivo_r(parser, &left)
    
    RETURN left
END FUNCTION

FUNCTION parse_aditivo_r(parser, left_ptr):
    // <aditivo_r> ::= TOKEN_PLUS <multiplicativo> <aditivo_r>
    //               | TOKEN_MINUS <multiplicativo> <aditivo_r>
    //               | ε
    
    WHILE match(parser, OP_PLUS) OR match(parser, OP_MINUS):
        op_token = consume_token(parser)
        right = parse_multiplicativo(parser)
        
        op_node = create_binary_op_node(op_token.tipo, *left_ptr, right)
        op_node.line = op_token.linha
        op_node.column = op_token.coluna
        
        *left_ptr = op_node
    END WHILE
END FUNCTION

FUNCTION parse_multiplicativo(parser):
    // <multiplicativo> ::= <unario> <multiplicativo_r>
    
    left = parse_unario(parser)
    parse_multiplicativo_r(parser, &left)
    
    RETURN left
END FUNCTION

FUNCTION parse_multiplicativo_r(parser, left_ptr):
    // <multiplicativo_r> ::= TOKEN_STAR <unario> <multiplicativo_r>
    //                      | TOKEN_SLASH <unario> <multiplicativo_r>
    //                      | TOKEN_PERCENT <unario> <multiplicativo_r>
    //                      | ε
    
    WHILE match(parser, OP_MULT) OR match(parser, OP_DIV) OR match(parser, OP_MOD):
        op_token = consume_token(parser)
        right = parse_unario(parser)
        
        op_node = create_binary_op_node(op_token.tipo, *left_ptr, right)
        op_node.line = op_token.linha
        op_node.column = op_token.coluna
        
        *left_ptr = op_node
    END WHILE
END FUNCTION

FUNCTION parse_primario(parser):
    // <primario> ::= TOKEN_IDENTIFIER
    //              | TOKEN_INT_LITERAL
    //              | TOKEN_FLOAT_LITERAL
    //              | TOKEN_CHAR_LITERAL
    //              | TOKEN_STRING_LITERAL
    //              | TOKEN_LPAREN <expressao> TOKEN_RPAREN
    
    token = peek_token(parser)
    
    IF match(parser, TK_ID):
        consume_token(parser)
        RETURN create_identifier_node(token.lexeme)
        
    ELSE IF match(parser, TK_NUM_INT) OR match(parser, TK_NUM_FLOAT) OR 
            match(parser, TK_CHAR) OR match(parser, TK_STRING):
        consume_token(parser)
        RETURN create_literal_node(token)
        
    ELSE IF match(parser, SYM_LPAREN):
        consume_token(parser)
        expr = parse_expressao(parser)
        expect(parser, SYM_RPAREN)
        RETURN expr
        
    ELSE:
        syntax_error(parser, "expressão primária", token_type_to_string(token.tipo))
        RETURN NULL
    END IF
END FUNCTION
```

---

## Error Handling

### Syntax Error Detection and Reporting

**Error Detection Points:**
1. `expect()` function: quando token esperado não é encontrado
2. FIRST set mismatch: quando token atual não pertence ao FIRST set de nenhuma produção válida
3. Unexpected EOF: quando arquivo termina no meio de uma construção

**Error Message Format (Portuguese):**
```
Erro sintático na linha <linha>, coluna <coluna>:
  Esperado: <token_esperado>
  Encontrado: <token_encontrado>
```

**Example:**
```
Erro sintático na linha 15, coluna 8:
  Esperado: ';'
  Encontrado: '}'
```

### Panic Mode Recovery

Quando um erro sintático é detectado, o parser entra em **modo pânico** para se recuperar e continuar a análise:

**Algoritmo de recuperação:**

```
FUNCTION panic_mode_recovery(parser):
    parser.panic_mode = TRUE
    parser.error_count++
    
    // Synchronize to next safe point
    synchronize(parser)
    
    parser.panic_mode = FALSE
END FUNCTION

FUNCTION synchronize(parser):
    // Skip tokens until we find a synchronization point
    
    WHILE NOT match(parser, TK_EOF):
        current = peek_token(parser)
        
        // Synchronization points
        IF current.tipo == SYM_SEMICOLON:
            consume_token(parser)  // consume ';' and continue
            RETURN
        ELSE IF current.tipo == SYM_RBRACE:
            // Don't consume '}', let block parser handle it
            RETURN
        ELSE IF current.tipo IN {KW_IF, KW_WHILE, KW_FOR, KW_RETURN, 
                                  KW_INT, KW_CHAR, KW_FLOAT, KW_VOID}:
            // Start of new statement or declaration
            RETURN
        END IF
        
        consume_token(parser)  // skip this token
    END WHILE
END FUNCTION
```

**Synchronization Points:**
- `;` (semicolon): end of statement
- `}` (right brace): end of block
- Keywords starting new statements: `if`, `while`, `for`, `return`
- Keywords starting declarations: `int`, `char`, `float`, `void`
- `EOF`: end of file

### Error Handling in Parsing Functions

```
FUNCTION syntax_error(parser, expected, found):
    token = peek_token(parser)
    
    PRINT "Erro sintático na linha " + token.linha + ", coluna " + token.coluna + ":"
    PRINT "  Esperado: " + expected
    PRINT "  Encontrado: " + found
    
    panic_mode_recovery(parser)
END FUNCTION

FUNCTION expect(parser, token_type):
    IF match(parser, token_type):
        RETURN consume_token(parser)
    ELSE:
        current = peek_token(parser)
        syntax_error(parser, 
                    token_type_to_string(token_type), 
                    token_type_to_string(current.tipo))
        RETURN NULL
    END IF
END FUNCTION
```

---

## Testing Strategy

### Testing Approach

O parser será testado usando uma abordagem dual:

1. **Unit Tests**: Testes específicos para funções individuais de parsing
2. **Integration Tests**: Testes de programas C completos

### Unit Testing Strategy

**Funções a testar:**
- `parse_expressao()`: expressões aritméticas, lógicas, relacionais
- `parse_declaracao_variavel()`: declarações de variáveis simples, arrays, ponteiros
- `parse_bloco()`: blocos com declarações e instruções
- `parse_instrucao_if()`: if com e sem else
- `parse_instrucao_while()`: loops while
- `parse_instrucao_for()`: loops for com expressões opcionais
- `enter_scope()` / `exit_scope()`: gerenciamento de escopos

**Exemplos de testes unitários:**

```c
// Test 1: Simple variable declaration
void test_parse_simple_variable() {
    parser_t *parser = create_test_parser("int x;");
    ast_node_t *node = parse_declaracao_variavel_local(parser);
    
    assert(node != NULL);
    assert(node->type == NODE_VAR_DECL);
    assert(strcmp(node->data.decl.name, "x") == 0);
    assert(node->data.decl.data_type == KW_INT);
}

// Test 2: Array declaration
void test_parse_array_declaration() {
    parser_t *parser = create_test_parser("int arr[10][20];");
    ast_node_t *node = parse_declaracao_variavel_local(parser);
    
    assert(node != NULL);
    assert(node->data.decl.array_dim_count == 2);
    assert(node->data.decl.array_dimensions[0] == 10);
    assert(node->data.decl.array_dimensions[1] == 20);
}

// Test 3: Expression with operator precedence
void test_parse_expression_precedence() {
    parser_t *parser = create_test_parser("2 + 3 * 4");
    ast_node_t *expr = parse_expressao(parser);
    
    // Should parse as: 2 + (3 * 4)
    assert(expr->type == NODE_BINARY_OP);
    assert(expr->data.expr.operator == OP_PLUS);
    assert(expr->data.expr.right->type == NODE_BINARY_OP);
    assert(expr->data.expr.right->data.expr.operator == OP_MULT);
}

// Test 4: Scope management
void test_scope_management() {
    parser_t *parser = create_parser();
    
    enter_scope(parser);  // scope 0 (global)
    assert(get_current_scope_id(parser) == 0);
    
    enter_scope(parser);  // scope 1
    assert(get_current_scope_id(parser) == 1);
    
    exit_scope(parser);
    assert(get_current_scope_id(parser) == 0);
}
```

### Integration Testing Strategy

**Programas C completos a testar:**

1. **Programa mínimo válido:**
```c
int main() {
    return 0;
}
```

2. **Declarações de variáveis:**
```c
int x;
float y = 3.14;
char str[100];
int matrix[10][20];
int *ptr;
```

3. **Estruturas de controle:**
```c
int main() {
    int x = 10;
    
    if (x > 5) {
        x = x + 1;
    } else {
        x = x - 1;
    }
    
    while (x < 20) {
        x = x + 2;
    }
    
    for (int i = 0; i < 10; i++) {
        x = x * 2;
    }
    
    return x;
}
```


4. **Funções:**
```c
int soma(int a, int b) {
    return a + b;
}

int main() {
    int resultado = soma(5, 10);
    return resultado;
}
```

5. **Structs:**
```c
struct Ponto {
    int x;
    int y;
};

int main() {
    struct Ponto p;
    p.x = 10;
    p.y = 20;
    return 0;
}
```

6. **Expressões complexas:**
```c
int main() {
    int a = 5;
    int b = 10;
    int c = (a + b) * 2 - (a * b) / 3;
    int d = a > b && c < 100 || a == 5;
    return d;
}
```

### Error Testing Strategy

**Programas com erros sintáticos a testar:**

1. **Falta de ponto-e-vírgula:**
```c
int main() {
    int x = 10  // Erro: falta ';'
    return 0;
}
```
Esperado: `Erro sintático na linha 2, coluna 16: Esperado: ';' Encontrado: 'return'`

2. **Parênteses não balanceados:**
```c
int main() {
    int x = (5 + 3;  // Erro: falta ')'
    return 0;
}
```
Esperado: `Erro sintático na linha 2, coluna 19: Esperado: ')' Encontrado: ';'`

3. **Tipo inválido:**
```c
int main() {
    invalid_type x;  // Erro: tipo desconhecido
    return 0;
}
```
Esperado: `Erro sintático na linha 2, coluna 5: Esperado: tipo válido Encontrado: 'invalid_type'`

4. **Expressão incompleta:**
```c
int main() {
    int x = 5 +;  // Erro: operando faltando
    return 0;
}
```
Esperado: `Erro sintático na linha 2, coluna 16: Esperado: expressão Encontrado: ';'`

### Test Coverage Goals

- **Grammar coverage:** 100% das produções gramaticais testadas
- **FIRST set coverage:** Todas as decisões de parsing testadas
- **Error recovery:** Todos os pontos de sincronização testados
- **Scope management:** Todos os níveis de aninhamento testados (até 10 níveis)
- **Symbol table enrichment:** Todos os tipos de símbolos testados

---

## Implementation Details

### Parser Initialization

```c
parser_t* create_parser(tabla_simbolos_t *symbol_table) {
    parser_t *parser = (parser_t*)malloc(sizeof(parser_t));
    
    parser->symbol_table = symbol_table;
    parser->current_position = 0;
    
    // Initialize scope stack
    parser->scope_stack_capacity = 256;
    parser->scope_stack = (scope_t**)malloc(sizeof(scope_t*) * parser->scope_stack_capacity);
    parser->scope_stack_size = 0;
    parser->next_scope_id = 0;
    
    // Initialize memory allocation
    parser->next_memory_address = 0;
    
    // Initialize error handling
    parser->error_count = 0;
    parser->panic_mode = 0;
    
    // AST root will be created by parse_programa
    parser->ast_root = NULL;
    
    return parser;
}

void free_parser(parser_t *parser) {
    // Free scope stack
    for (int i = 0; i < parser->scope_stack_size; i++) {
        free_scope(parser->scope_stack[i]);
    }
    free(parser->scope_stack);
    
    // Free AST
    if (parser->ast_root != NULL) {
        free_ast(parser->ast_root);
    }
    
    free(parser);
}
```

### Scope Management Implementation

```c
void enter_scope(parser_t *parser) {
    scope_t *new_scope = (scope_t*)malloc(sizeof(scope_t));
    
    new_scope->scope_id = parser->next_scope_id++;
    
    // Set parent to current scope (or NULL if this is first scope)
    if (parser->scope_stack_size > 0) {
        new_scope->parent = parser->scope_stack[parser->scope_stack_size - 1];
    } else {
        new_scope->parent = NULL;
    }
    
    // Initialize local symbol table
    new_scope->symbol_capacity = 64;
    new_scope->local_symbols = (char**)malloc(sizeof(char*) * new_scope->symbol_capacity);
    new_scope->symbol_count = 0;
    
    // Push to stack
    if (parser->scope_stack_size >= parser->scope_stack_capacity) {
        // Resize stack
        parser->scope_stack_capacity *= 2;
        parser->scope_stack = (scope_t**)realloc(parser->scope_stack, 
                                                  sizeof(scope_t*) * parser->scope_stack_capacity);
    }
    
    parser->scope_stack[parser->scope_stack_size++] = new_scope;
}

void exit_scope(parser_t *parser) {
    if (parser->scope_stack_size > 0) {
        parser->scope_stack_size--;
        // Note: we don't free the scope here, it will be freed when parser is destroyed
    }
}

int get_current_scope_id(parser_t *parser) {
    if (parser->scope_stack_size > 0) {
        return parser->scope_stack[parser->scope_stack_size - 1]->scope_id;
    }
    return -1;  // No scope
}

void add_symbol_to_scope(parser_t *parser, const char *name) {
    if (parser->scope_stack_size == 0) return;
    
    scope_t *current_scope = parser->scope_stack[parser->scope_stack_size - 1];
    
    // Resize if needed
    if (current_scope->symbol_count >= current_scope->symbol_capacity) {
        current_scope->symbol_capacity *= 2;
        current_scope->local_symbols = (char**)realloc(current_scope->local_symbols,
                                                       sizeof(char*) * current_scope->symbol_capacity);
    }
    
    // Add symbol
    current_scope->local_symbols[current_scope->symbol_count++] = strdup(name);
}
```


### Token Consumption Implementation

```c
token_t peek_token(parser_t *parser) {
    if (parser->current_position < parser->symbol_table->quantidade) {
        return parser->symbol_table->tokens[parser->current_position];
    }
    
    // Return EOF token if we've reached the end
    token_t eof_token;
    eof_token.tipo = TK_EOF;
    strcpy(eof_token.lexeme, "EOF");
    eof_token.linha = -1;
    eof_token.coluna = -1;
    return eof_token;
}

token_t consume_token(parser_t *parser) {
    token_t token = peek_token(parser);
    
    if (parser->current_position < parser->symbol_table->quantidade) {
        parser->current_position++;
    }
    
    return token;
}

int match(parser_t *parser, int token_type) {
    token_t current = peek_token(parser);
    return current.tipo == token_type;
}

void expect(parser_t *parser, int token_type) {
    if (match(parser, token_type)) {
        consume_token(parser);
    } else {
        token_t current = peek_token(parser);
        syntax_error(parser, 
                    tipo_token_para_string(token_type),
                    tipo_token_para_string(current.tipo));
    }
}
```

### AST Construction Implementation

```c
ast_node_t* create_ast_node(node_type_t type, int line, int column) {
    ast_node_t *node = (ast_node_t*)calloc(1, sizeof(ast_node_t));
    
    node->type = type;
    node->line = line;
    node->column = column;
    node->children = NULL;
    node->child_count = 0;
    
    return node;
}

void add_ast_child(ast_node_t *parent, ast_node_t *child) {
    if (child == NULL) return;
    
    parent->child_count++;
    parent->children = (ast_node_t**)realloc(parent->children, 
                                             sizeof(ast_node_t*) * parent->child_count);
    parent->children[parent->child_count - 1] = child;
}

ast_node_t* create_binary_op_node(int operator, ast_node_t *left, ast_node_t *right) {
    ast_node_t *node = create_ast_node(NODE_BINARY_OP, left->line, left->column);
    
    node->data.expr.operator = operator;
    node->data.expr.left = left;
    node->data.expr.right = right;
    
    return node;
}

ast_node_t* create_identifier_node(const char *name) {
    ast_node_t *node = create_ast_node(NODE_IDENTIFIER, 0, 0);
    node->data.expr.identifier = strdup(name);
    return node;
}

void free_ast(ast_node_t *root) {
    if (root == NULL) return;
    
    // Free children recursively
    for (int i = 0; i < root->child_count; i++) {
        free_ast(root->children[i]);
    }
    free(root->children);
    
    // Free node-specific data
    if (root->type == NODE_IDENTIFIER && root->data.expr.identifier != NULL) {
        free(root->data.expr.identifier);
    }
    
    free(root);
}
```

---

## Execution Flow

### Complete Parsing Pipeline

```
1. INITIALIZATION
   ├─ Lexer runs and produces tabla_simbolos_t with tokens
   ├─ Parser is created with create_parser(symbol_table)
   └─ Parser state initialized (scope stack, memory counter, etc.)

2. PARSING
   ├─ parse_programa() called (entry point)
   ├─ Global scope entered (scope_id = 0)
   ├─ For each global declaration:
   │  ├─ Identify type (include, define, typedef, variable, function)
   │  ├─ Parse according to grammar rules
   │  ├─ Build AST nodes
   │  └─ Enrich symbol table with semantic info
   ├─ For each function:
   │  ├─ Enter function scope (scope_id++)
   │  ├─ Parse parameters
   │  ├─ Parse function body (block)
   │  │  ├─ For each local declaration:
   │  │  │  ├─ Allocate memory address
   │  │  │  ├─ Add to current scope
   │  │  │  └─ Enrich symbol table
   │  │  └─ For each statement:
   │  │     ├─ Parse according to type (if, while, for, etc.)
   │  │     ├─ Build AST subtree
   │  │     └─ Handle nested blocks (enter/exit scopes)
   │  └─ Exit function scope
   └─ Expect TOKEN_EOF

3. ERROR HANDLING (if syntax error occurs)
   ├─ Report error with line/column
   ├─ Enter panic mode
   ├─ Synchronize to next safe point (;, }, keyword)
   └─ Continue parsing

4. OUTPUT
   ├─ AST root returned (parser->ast_root)
   ├─ Symbol table enriched with:
   │  ├─ Data types
   │  ├─ Scope IDs
   │  ├─ Memory addresses
   │  ├─ Array dimensions
   │  └─ Function signatures
   └─ Error count returned

5. CLEANUP
   ├─ free_ast(ast_root)
   ├─ free_parser(parser)
   └─ Symbol table remains for next phase
```

### Example Execution Trace

**Input program:**
```c
int x = 10;

int soma(int a, int b) {
    int resultado = a + b;
    return resultado;
}

int main() {
    int y = soma(x, 5);
    return y;
}
```

**Execution trace:**

```
1. parse_programa()
   ├─ enter_scope() → scope_id = 0 (global)
   ├─ parse_declaracao_global()
   │  ├─ parse_especificador_tipo() → KW_INT
   │  ├─ consume TOKEN_IDENTIFIER "x"
   │  ├─ enrich_symbol_type("x", KW_INT, 0)
   │  ├─ enrich_symbol_scope("x", 0, VAR_GLOBAL)
   │  ├─ enrich_symbol_memory("x", 0, 4)
   │  └─ next_memory_address = 4
   │
   ├─ parse_declaracao_global()
   │  ├─ parse_especificador_tipo() → KW_INT
   │  ├─ consume TOKEN_IDENTIFIER "soma"
   │  ├─ consume TOKEN_LPAREN
   │  ├─ enter_scope() → scope_id = 1 (function soma)
   │  ├─ parse_parametro() → int a
   │  │  ├─ enrich_symbol_type("a", KW_INT, 0)
   │  │  ├─ enrich_symbol_scope("a", 1, VAR_PARAMETER)
   │  │  └─ enrich_symbol_memory("a", -4, 4)
   │  ├─ parse_parametro() → int b
   │  │  ├─ enrich_symbol_type("b", KW_INT, 0)
   │  │  ├─ enrich_symbol_scope("b", 1, VAR_PARAMETER)
   │  │  └─ enrich_symbol_memory("b", -8, 4)
   │  ├─ parse_bloco()
   │  │  ├─ enter_scope() → scope_id = 2 (block inside soma)
   │  │  ├─ parse_declaracao_variavel_local() → int resultado
   │  │  │  ├─ enrich_symbol_type("resultado", KW_INT, 0)
   │  │  │  ├─ enrich_symbol_scope("resultado", 2, VAR_LOCAL)
   │  │  │  └─ enrich_symbol_memory("resultado", 0, 4)
   │  │  ├─ parse_instrucao_return()
   │  │  │  └─ parse_expressao() → identifier "resultado"
   │  │  └─ exit_scope() → back to scope 1
   │  └─ exit_scope() → back to scope 0
   │
   └─ parse_declaracao_global()
      ├─ parse_especificador_tipo() → KW_INT
      ├─ consume TOKEN_IDENTIFIER "main"
      ├─ enter_scope() → scope_id = 3 (function main)
      ├─ parse_bloco()
      │  ├─ enter_scope() → scope_id = 4 (block inside main)
      │  ├─ parse_declaracao_variavel_local() → int y
      │  ├─ parse_instrucao_return()
      │  └─ exit_scope() → back to scope 3
      └─ exit_scope() → back to scope 0

2. expect(TK_EOF)
3. exit_scope() → exit global scope
4. Return AST root
```

---

## Performance Considerations

### Time Complexity

**Parsing complexity:** O(n) onde n = número de tokens
- Uma única passagem sobre a sequência de tokens
- Cada token é consumido exatamente uma vez
- Decisões de parsing em tempo constante (baseadas em FIRST sets)

**Scope lookup:** O(d) onde d = profundidade de aninhamento de escopos
- Busca linear na pilha de escopos (do mais interno ao mais externo)
- Profundidade típica: 3-5 níveis
- Profundidade máxima suportada: 256 níveis

**Symbol table enrichment:** O(1) por símbolo
- Acesso direto ao token na tabela de símbolos por nome
- Atualização de campos em tempo constante

**Overall:** O(n) para parsing completo de programa com n tokens

### Space Complexity

**Parser state:** O(1)
- Estrutura `parser_t` de tamanho fixo
- Ponteiro para tabela de símbolos (não duplicada)

**Scope stack:** O(d) onde d = profundidade de aninhamento
- Cada escopo armazena lista de símbolos locais
- Espaço proporcional ao número de variáveis por escopo

**AST:** O(n) onde n = número de nós
- Cada token/operador/declaração → 1 nó AST
- Nós armazenam ponteiros para filhos (não duplicam dados)

**Overall:** O(n + d) ≈ O(n) para programas típicos

### Memory Management Strategy

**Allocation:**
- AST nodes: alocados dinamicamente durante parsing
- Scope structures: alocados ao entrar em novo escopo
- Symbol table: reutilizada do Lexer (não duplicada)

**Deallocation:**
- AST: liberada recursivamente após uso (ou mantida para próxima fase)
- Scopes: liberados ao destruir parser
- Symbol table: mantida para próxima fase (não liberada pelo parser)

**Memory safety:**
- Verificação de NULL após cada malloc/calloc
- Uso de calloc para inicialização automática com zeros
- Liberação em ordem reversa da alocação

### Optimization Strategies

1. **Token lookahead caching:**
   - `peek_token()` não avança posição
   - Evita múltiplas leituras do mesmo token

2. **Scope stack pre-allocation:**
   - Capacidade inicial de 256 escopos
   - Reduz realocações durante parsing

3. **AST node pooling (opcional):**
   - Pool de nós pré-alocados para reduzir chamadas malloc
   - Útil para programas grandes (>10.000 linhas)

4. **Symbol table indexing (opcional):**
   - Hash table para lookup O(1) de símbolos por nome
   - Útil para programas com muitas variáveis

### Performance Targets

**Parsing speed:**
- 10.000 linhas em < 5 segundos
- ~2000 linhas/segundo

**Memory usage:**
- < 100 MB para programas de 10.000 linhas
- ~10 KB por 1000 linhas

**Error recovery:**
- Continuar parsing após erro em < 10ms
- Reportar múltiplos erros em uma única passagem

---

## Integration with Lexer

### Interface Contract

**Input from Lexer:**
```c
typedef struct tabla_simbolos_s {
    token_t *tokens;              /* Array de tokens */
    int quantidade;               /* Número de tokens */
    int capacidade;               /* Capacidade do array */
    int posicao_leitura;          /* Posição atual (usado pelo parser) */
} tabla_simbolos_t;
```

**Parser usage:**
```c
// Lexer phase
tabla_simbolos_t *symbol_table = processar_arquivo_completo("programa.c");

// Parser phase
parser_t *parser = create_parser(symbol_table);
ast_node_t *ast = parse_programa(parser);

if (parser->error_count == 0) {
    printf("Parsing bem-sucedido!\n");
    // Continue to semantic analysis
} else {
    printf("Parsing falhou com %d erros.\n", parser->error_count);
}

free_parser(parser);
// symbol_table is kept for next phase
```

### Token Type Mapping

O parser usa os tipos de token definidos em `tokens.h`:

| Token Category | Token Types Used by Parser |
|----------------|---------------------------|
| Keywords | `KW_INT`, `KW_CHAR`, `KW_FLOAT`, `KW_VOID`, `KW_IF`, `KW_WHILE`, `KW_FOR`, `KW_DO`, `KW_RETURN`, `KW_STRUCT`, `KW_UNION`, `KW_TYPEDEF` |
| Identifiers | `TK_ID` |
| Literals | `TK_NUM_INT`, `TK_NUM_FLOAT`, `TK_CHAR`, `TK_STRING` |
| Operators | `OP_PLUS`, `OP_MINUS`, `OP_MULT`, `OP_DIV`, `OP_MOD`, `OP_ASSIGN`, `OP_EQ`, `OP_NE`, `OP_LT`, `OP_LE`, `OP_GT`, `OP_GE`, `OP_AND`, `OP_OR`, `OP_NOT` |
| Symbols | `SYM_LPAREN`, `SYM_RPAREN`, `SYM_LBRACE`, `SYM_RBRACE`, `SYM_LBRACKET`, `SYM_RBRACKET`, `SYM_SEMICOLON`, `SYM_COMMA`, `SYM_DOT` |
| Preprocessor | `TK_PP_INCLUDE`, `TK_PP_DEFINE` |
| Special | `TK_EOF`, `TK_ERROR` |

### Symbol Table Extension

O parser estende a tabela de símbolos sem modificar a estrutura `token_t` original. Duas abordagens possíveis:

**Approach 1: Parallel table**
```c
typedef struct symbol_table_extended_s {
    tabla_simbolos_t *base_table;     /* Tabela do Lexer */
    symbol_info_t *semantic_info;     /* Array paralelo de infos semânticas */
    int info_count;
} symbol_table_extended_t;
```

**Approach 2: Hash table by name**
```c
typedef struct semantic_table_s {
    char *symbol_name;
    symbol_info_t info;
    struct semantic_table_s *next;  /* For collision resolution */
} semantic_table_entry_t;

typedef struct semantic_table_s {
    semantic_table_entry_t **buckets;
    int bucket_count;
} semantic_table_t;
```

**Recomendação:** Approach 2 (hash table) para lookup eficiente O(1) por nome de símbolo.

---

## Diagrams

### Parser Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                         PARSER SYSTEM                           │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │                    parse_programa()                      │  │
│  │                    (Entry Point)                         │  │
│  └────────────────────────┬─────────────────────────────────┘  │
│                           │                                     │
│                           ▼                                     │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │         parse_lista_declaracoes_globais()                │  │
│  └────────────────────────┬─────────────────────────────────┘  │
│                           │                                     │
│         ┌─────────────────┼─────────────────┐                  │
│         ▼                 ▼                 ▼                  │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │  #include   │  │  Variable   │  │  Function   │           │
│  │  #define    │  │  Decl       │  │  Decl       │           │
│  └─────────────┘  └──────┬──────┘  └──────┬──────┘           │
│                          │                 │                   │
│                          │                 ▼                   │
│                          │         ┌─────────────┐             │
│                          │         │ parse_bloco │             │
│                          │         └──────┬──────┘             │
│                          │                │                    │
│                          │    ┌───────────┼───────────┐        │
│                          │    ▼           ▼           ▼        │
│                          │  ┌────┐    ┌────┐    ┌────┐        │
│                          │  │Var │    │Stmt│    │Expr│        │
│                          │  │Decl│    │    │    │    │        │
│                          │  └────┘    └────┘    └────┘        │
│                          │                                     │
│                          ▼                                     │
│                  ┌──────────────┐                              │
│                  │ Enrich Symbol│                              │
│                  │    Table     │                              │
│                  └──────────────┘                              │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │                    AST Construction                      │  │
│  │  ┌────────┐  ┌────────┐  ┌────────┐  ┌────────┐        │  │
│  │  │Program │─▶│ Decl   │─▶│ Stmt   │─▶│ Expr   │        │  │
│  │  │  Node  │  │ Nodes  │  │ Nodes  │  │ Nodes  │        │  │
│  │  └────────┘  └────────┘  └────────┘  └────────┘        │  │
│  └──────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

### Expression Parsing Flow (Operator Precedence)

```
parse_expressao()
    │
    ▼
parse_atribuicao()
    │
    ▼
parse_logico_ou()          (lowest precedence: ||)
    │
    ▼
parse_logico_e()           (&&)
    │
    ▼
parse_igualdade()          (==, !=)
    │
    ▼
parse_relacional()         (<, >, <=, >=)
    │
    ▼
parse_aditivo()            (+, -)
    │
    ▼
parse_multiplicativo()     (*, /, %)
    │
    ▼
parse_unario()             (-, !, &, *)
    │
    ▼
parse_pos_fixo()           ([], ., ())
    │
    ▼
parse_primario()           (highest precedence: literals, identifiers, ())
```


### Scope Stack Visualization

```
Program execution:

int x = 10;              // Global scope (scope_id = 0)

int soma(int a, int b) { // Enter function scope (scope_id = 1)
    int resultado;       // Local variable in scope 1
    {                    // Enter block scope (scope_id = 2)
        int temp;        // Local variable in scope 2
    }                    // Exit scope 2
    return resultado;
}                        // Exit scope 1

int main() {             // Enter function scope (scope_id = 3)
    int y;               // Local variable in scope 3
    return 0;
}                        // Exit scope 3


Scope Stack Evolution:

Step 1: Parse "int x = 10;"
┌──────────┐
│ Scope 0  │ ← Global scope
│ (global) │
└──────────┘

Step 2: Enter function "soma"
┌──────────┐
│ Scope 1  │ ← Function soma scope
│ (soma)   │
├──────────┤
│ Scope 0  │
│ (global) │
└──────────┘

Step 3: Enter block inside "soma"
┌──────────┐
│ Scope 2  │ ← Block scope
│ (block)  │
├──────────┤
│ Scope 1  │
│ (soma)   │
├──────────┤
│ Scope 0  │
│ (global) │
└──────────┘

Step 4: Exit block, exit function
┌──────────┐
│ Scope 0  │ ← Back to global
│ (global) │
└──────────┘

Step 5: Enter function "main"
┌──────────┐
│ Scope 3  │ ← Function main scope
│ (main)   │
├──────────┤
│ Scope 0  │
│ (global) │
└──────────┘
```

### AST Example for Expression: `2 + 3 * 4`

```
Grammar ensures correct precedence:
parse_expressao() → parse_atribuicao() → parse_logico_ou() → ... → parse_aditivo()

parse_aditivo():
  left = parse_multiplicativo()  → returns node for "2"
  parse_aditivo_r():
    sees TOKEN_PLUS
    right = parse_multiplicativo()
      left = parse_unario() → returns node for "3"
      parse_multiplicativo_r():
        sees TOKEN_STAR
        right = parse_unario() → returns node for "4"
        creates NODE_BINARY_OP(*, 3, 4)
      returns NODE_BINARY_OP(*, 3, 4)
    creates NODE_BINARY_OP(+, 2, NODE_BINARY_OP(*, 3, 4))

Resulting AST:

         NODE_BINARY_OP
         (operator: +)
         /           \
        /             \
   NODE_LITERAL   NODE_BINARY_OP
   (value: 2)     (operator: *)
                   /           \
                  /             \
             NODE_LITERAL   NODE_LITERAL
             (value: 3)     (value: 4)

Evaluation: 2 + (3 * 4) = 2 + 12 = 14 ✓
```

---

## Summary

### Key Design Decisions

1. **Parsing Method:** Top-Down Recursive Descent (LL(1))
   - Cada não-terminal → função C
   - Decisões baseadas em FIRST sets
   - Sem backtracking

2. **Grammar Transformation:** Já está em forma LL(1)
   - Recursividade à esquerda eliminada com produções auxiliares `_r`
   - FIRST sets disjuntos garantem parsing preditivo

3. **AST Construction:** Bottom-up durante parsing
   - Nós folha criados primeiro (literais, identificadores)
   - Nós internos criados ao combinar subexpressões
   - Estrutura reflete precedência de operadores

4. **Scope Management:** Pilha de escopos hierárquicos
   - Escopo global (id=0) sempre presente
   - Novos escopos criados ao entrar em funções/blocos
   - Lookup percorre pilha do topo (mais interno) à base (global)

5. **Symbol Table Enrichment:** Informações adicionadas durante parsing
   - Tipos de dados
   - Classificação de variáveis (global/local/parâmetro)
   - Endereços de memória
   - Dimensões de arrays
   - Assinaturas de funções

6. **Error Handling:** Modo pânico com sincronização
   - Erros reportados com linha/coluna
   - Parsing continua após erro
   - Sincronização em `;`, `}`, keywords

### Implementation Roadmap

**Phase 1: Core Infrastructure**
- Implementar estruturas de dados (`parser_t`, `ast_node_t`, `scope_t`)
- Implementar funções de consumo de tokens
- Implementar gerenciamento de escopos

**Phase 2: Expression Parsing**
- Implementar funções de parsing de expressões (precedência de operadores)
- Implementar construção de nós AST para expressões
- Testar com expressões aritméticas, lógicas, relacionais

**Phase 3: Statement Parsing**
- Implementar parsing de instruções (if, while, for, return, etc.)
- Implementar parsing de blocos
- Testar com programas simples

**Phase 4: Declaration Parsing**
- Implementar parsing de declarações de variáveis
- Implementar parsing de declarações de funções
- Implementar enriquecimento da tabela de símbolos
- Testar com programas completos

**Phase 5: Error Handling**
- Implementar detecção e reporte de erros
- Implementar modo pânico e sincronização
- Testar com programas com erros sintáticos

**Phase 6: Integration and Testing**
- Integrar com Lexer
- Testes de integração com programas C completos
- Otimizações de performance
- Documentação (manuais de usuário e programador)

### Next Steps

Após completar o design, as próximas etapas são:

1. **Criar tasks.md:** Decompor o design em tarefas implementáveis
2. **Implementar parser:** Seguir roadmap acima
3. **Testar:** Executar suite de testes unitários e de integração
4. **Documentar:** Escrever manuais de usuário e programador
5. **Integrar:** Conectar com fase de análise semântica (Fase 3)

---

**Documento de Design Completo - Pronto para Revisão**


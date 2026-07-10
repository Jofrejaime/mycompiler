#ifndef SCOPE_H
#define SCOPE_H

/* Forward declarations */
struct ast_node_s;

/* ============================================================================
   SCOPE MANAGEMENT - HEADER
   
   Gerenciamento de escopos hierárquicos (global, função, bloco)
   
   ============================================================================ */

/* ============================================================================
   ESTRUTURA DE ESCOPO
   ============================================================================ */

typedef struct parser_s parser_t;

typedef enum {
   SYMBOL_VARIABLE,
   SYMBOL_FUNCTION,
   SYMBOL_PARAMETER,
   SYMBOL_TYPEDEF,
   SYMBOL_STRUCT,
   SYMBOL_UNION
} symbol_kind_t;

typedef enum {
   VAR_GLOBAL,
   VAR_LOCAL,
   VAR_PARAMETER,
   VAR_STRUCT_FIELD
} variable_type_t;

typedef struct symbol_info_s {
   int data_type;
   int is_pointer;
   symbol_kind_t kind;
   variable_type_t variable_type;
   int scope_id;
   int memory_address;
   int size_bytes;
   int is_array;
   int array_dimensions[8];
   int array_dim_count;
   int is_function;
   int function_return_type;
   int function_param_types[32];
   int function_param_count;
   int is_typedef;
   /* Struct/union fields table */
   int is_struct_or_union;          /* 1 if this symbol IS a struct/union type */
   struct struct_field_s *fields;   /* Array of fields (NULL if not struct/union) */
   int field_count;                 /* Number of fields */
   /* Typedef base name (for typedef → struct resolution) */
   char typedef_base_name[64];      /* e.g. "Node" for typedef struct Node Node_t */
   /* Struct/union tag name for variable declarations: "Ponto" for struct Ponto p */
   char struct_tag_name[64];        /* set when data_type == KW_STRUCT or KW_UNION */
   /* Constant initializer (set only when initializer is a single literal) */
   int has_const_init;              /* 1 if constant value is known at parse time */
   union {
       long long int_value;         /* KW_INT, KW_CHAR literals */
       double      double_value;    /* KW_FLOAT, KW_DOUBLE literals */
   } const_value;
} symbol_info_t;

/* ============================================================================
   ESTRUTURA DE CAMPO DE STRUCT/UNION
   ============================================================================ */

typedef struct struct_field_s {
    char name[64];          /* Field name */
    int  data_type;         /* KW_INT, KW_CHAR, etc. */
    int  is_pointer;        /* Pointer level */
    int  is_array;
    int  array_dimensions[8];
    int  array_dim_count;
    int  size_bytes;        /* Size of this field */
    int  offset;            /* Byte offset within struct */
} struct_field_t;

typedef struct scope_s {
    int scope_id;                   /* ID único do escopo */
    int parent_scope_id;            /* ID do escopo pai (-1 se global) */
    
    /* Symbol table for this scope */
    struct symbol_entry_s **symbols;  /* Hash table de símbolos */
    int symbol_count;               /* Número de símbolos */
    int symbol_capacity;            /* Capacidade da tabela */
    
    /* Memory allocation for this scope */
    int base_address;               /* Endereço base do escopo */
    int current_offset;             /* Offset atual */
} scope_t;

/* ============================================================================
   ESTRUTURA DE ENTRADA DE SÍMBOLO NO ESCOPO
   ============================================================================ */

typedef struct symbol_entry_s {
    char *name;                     /* Nome do símbolo */
    void *info;                     /* Ponteiro para symbol_info_t */
    struct symbol_entry_s *next;    /* Próximo na lista (colisão hash) */
} symbol_entry_t;

/* ============================================================================
   FUNÇÕES DE GERENCIAMENTO DE ESCOPO
   ============================================================================ */

/*
   Criar novo escopo
   
   Entrada: scope_id - ID do escopo
            parent_scope_id - ID do escopo pai (-1 se global)
            base_address - endereço base de memória
   Saída: ponteiro para escopo criado, ou NULL se erro
*/
scope_t* scope_create(int scope_id, int parent_scope_id, int base_address);

/*
   Liberar escopo
   
   Entrada: scope - escopo a liberar
   Saída: void
*/
void scope_free(scope_t *scope);

/*
   Adicionar símbolo ao escopo
   
   Entrada: scope - escopo
            name - nome do símbolo
            info - informações do símbolo (symbol_info_t*)
   Saída: 1 se sucesso, 0 se erro (símbolo já existe)
*/
int scope_add_symbol(scope_t *scope, const char *name, void *info);

/*
   Adicionar nome de símbolo ao escopo (versão simplificada)
   
   Esta função adiciona apenas o nome do símbolo ao escopo atual,
   sem informações adicionais. Útil para registrar símbolos antes
   de enriquecê-los com informações semânticas.
   
   Entrada: scope - escopo
            name - nome do símbolo
   Saída: 1 se sucesso, 0 se erro (símbolo já existe ou erro de memória)
*/
int add_symbol_to_scope(scope_t *scope, const char *name);

/*
   Buscar símbolo no escopo (apenas neste escopo, não nos pais)
   
   Entrada: scope - escopo
            name - nome do símbolo
   Saída: ponteiro para informações do símbolo, ou NULL se não encontrado
*/
void* scope_lookup_symbol(scope_t *scope, const char *name);

/*
   Verificar se símbolo existe no escopo
   
   Entrada: scope - escopo
            name - nome do símbolo
   Saída: 1 se símbolo existe, 0 caso contrário
*/
int scope_symbol_exists(scope_t *scope, const char *name);

/*
   Alocar memória no escopo
   
   Entrada: scope - escopo
            size - tamanho em bytes
   Saída: endereço alocado
*/
int scope_allocate_memory(scope_t *scope, int size);

/* ============================================================================
   FUNÇÕES UTILITÁRIAS
   ============================================================================ */

const char* type_to_string(int type);
int get_base_type_size(int data_type, int is_pointer);
int calculate_total_size(int data_type, int is_pointer, int *dimensions, int dim_count);

struct_field_t* lookup_struct_field(struct parser_s *parser,
                                    const char *struct_name,
                                    const char *field_name);

void try_set_const_init(symbol_info_t *info, struct ast_node_s *init_node);

/* ============================================================================
   FUNÇÕES DE IMPRESSÃO (controladas em runtime pela flag CLI --symbols)
   ============================================================================ */

/* Sempre disponíveis: a impressão é controlada em runtime pela flag CLI --symbols. */
void print_symbol_info(symbol_info_t *info);
void print_scope(scope_t *scope);
void print_all_scopes(parser_t *parser);
void print_scope_stack(scope_t **scope_stack, int stack_size);

#endif /* SCOPE_H */
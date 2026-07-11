#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"
#include "../lexer/tokens.h"
#include "ast.h"
#include "scope.h"

/* ============================================================================
   ANALISADOR SINTÁTICO (PARSER) - HEADER
   
   Implementa a SEGUNDA FASE do compilador: Análise Sintática
   
   Método: Top-Down Recursivo Descendente sem backtracking (LL(1))
   
   Arquitetura de Tabelas de Símbolos:
   - TABELA GLOBAL: Variáveis globais, funções, structs, typedefs
   - TABELA LOCAL: Variáveis locais, parâmetros, variáveis de bloco
   
   Funções principais:
   - parser_init()           Inicializar parser com stream de tokens
   - parser_run()            Executar análise sintática completa
   - parser_free()           Liberar recursos do parser
   
   ============================================================================ */

/* ============================================================================
   ESTRUTURA DO PARSER
   ============================================================================ */

typedef struct parser_s {
    /* Token stream from Lexer */
    tabla_simbolos_t *token_stream;     /* Stream de tokens do Lexer */
    int current_position;               /* Posição atual de leitura */
    
    /* Symbol Tables - DUAL TABLE ARCHITECTURE */
    scope_t *global_symbol_table;       /* Tabela de símbolos GLOBAL */
    scope_t *current_local_table;       /* Tabela de símbolos LOCAL atual */

   scope_t *all_scopes[1024];          /* Registro hierárquico de escopos */
   int total_scopes;                   /* Quantidade de escopos registrados */

   scope_t *global_scope;              /* Escopo global real */
   scope_t *current_scope;             /* Escopo ativo atual */
    
    /* Scope management */
    scope_t **scope_stack;              /* Pilha de escopos locais */
    int scope_stack_size;               /* Tamanho atual da pilha */
    int scope_stack_capacity;           /* Capacidade da pilha */
    int next_scope_id;                  /* Próximo ID de escopo */
    
    /* Memory allocation */
    int next_global_address;            /* Próximo endereço global (.data) */
    int next_local_offset;              /* Próximo offset local (stack) */
    
    /* Error handling */
    int error_count;                    /* Contador de erros sintáticos */
    int panic_mode;                     /* Flag de modo pânico */
    
    /* AST root */
    ast_node_t *ast_root;               /* Raiz da árvore sintática */

   /* Source file path (for detailed error reporting) */
   const char *source_path;
} parser_t;

/* ============================================================================
   ESTRUTURA DE INFORMAÇÃO SEMÂNTICA DE SÍMBOLOS
   ============================================================================ */

/* ============================================================================
   FUNÇÕES DE INICIALIZAÇÃO E LIMPEZA
   ============================================================================ */

/*
   Inicializar parser com tabela de símbolos do Lexer
   
   Entrada: symbol_table - tabela de símbolos preenchida pelo Lexer
   Saída: ponteiro para parser_t inicializado, ou NULL se erro
*/
parser_t* parser_init(tabla_simbolos_t *symbol_table);

/*
   Executar análise sintática completa
   
   Entrada: parser - estrutura do parser inicializada
   Saída: 0 se sucesso, código de erro caso contrário
*/
int parser_run(parser_t *parser);

/*
   Liberar recursos do parser
   
   Entrada: parser - estrutura a liberar
   Saída: void
*/
void parser_free(parser_t *parser);

/* ============================================================================
   FUNÇÕES DE CONSUMO DE TOKENS
   ============================================================================ */

/*
   Consultar token atual sem consumi-lo (lookahead)
   
   Entrada: parser - estrutura do parser
   Saída: token atual
*/
token_t peek_token(parser_t *parser);

/*
   Consumir token atual e avançar posição
   
   Entrada: parser - estrutura do parser
   Saída: token consumido
*/
token_t consume_token(parser_t *parser);

/*
   Verificar se token atual corresponde ao tipo esperado
   
   Entrada: parser - estrutura do parser
            token_type - tipo de token esperado
   Saída: 1 se match, 0 caso contrário
*/
int match(parser_t *parser, int token_type);

/*
   Consumir token esperado ou reportar erro
   
   Entrada: parser - estrutura do parser
            token_type - tipo de token esperado
   Saída: void (reporta erro se não houver match)
*/
void expect(parser_t *parser, int token_type);

/* Detailed syntax error reporting */
void syntax_error(parser_t *parser, const char *message, int expected_type, token_t found);

/* Redeclaration reporting (non-panic: declaration is syntactically valid) */
void report_redeclaration(parser_t *parser, const char *name, token_t found);

/* Error recovery (panic mode) */
void synchronize(parser_t *parser);
void panic_mode_recovery(parser_t *parser);

/* ============================================================================
   FUNÇÕES DE GERENCIAMENTO DE TABELAS DE SÍMBOLOS
   ============================================================================ */

/*
   Buscar símbolo (primeiro local, depois global)
   
   Entrada: parser - estrutura do parser
            name - nome do símbolo
   Saída: ponteiro para symbol_info_t, ou NULL se não encontrado
*/
symbol_info_t* lookup_symbol(parser_t *parser, const char *name);

/*
   Buscar símbolo apenas na tabela global
   
   Entrada: parser - estrutura do parser
            name - nome do símbolo
   Saída: ponteiro para symbol_info_t, ou NULL se não encontrado
*/
symbol_info_t* lookup_global_symbol(parser_t *parser, const char *name);

/*
   Buscar símbolo apenas na tabela local atual
   
   Entrada: parser - estrutura do parser
            name - nome do símbolo
   Saída: ponteiro para symbol_info_t, ou NULL se não encontrado
*/
symbol_info_t* lookup_local_symbol(parser_t *parser, const char *name);

/*
   Adicionar símbolo à tabela global
   
   Entrada: parser - estrutura do parser
            name - nome do símbolo
            info - informações do símbolo
   Saída: 1 se sucesso, 0 se erro (símbolo já existe)
*/
int add_global_symbol(parser_t *parser, const char *name, symbol_info_t *info);

/*
   Adicionar símbolo à tabela local atual
   
   Entrada: parser - estrutura do parser
            name - nome do símbolo
            info - informações do símbolo
   Saída: 1 se sucesso, 0 se erro (símbolo já existe)
*/
int add_local_symbol(parser_t *parser, const char *name, symbol_info_t *info);

/*
   Entrar em novo escopo local
   
   Entrada: parser - estrutura do parser
   Saída: void
*/
void enter_local_scope(parser_t *parser);

/*
   Sair do escopo local atual
   
   Entrada: parser - estrutura do parser
   Saída: void
*/
void exit_local_scope(parser_t *parser);

/* ============================================================================
   FUNÇÕES DE ENRIQUECIMENTO DA TABELA DE SÍMBOLOS
   ============================================================================ */

/*
   Enriquecer símbolo com informações de tipo
*/
void enrich_symbol_type(parser_t *parser, const char *name, int data_type, int is_pointer);

/*
   Enriquecer símbolo com informações de escopo
*/
void enrich_symbol_scope(parser_t *parser, const char *name, int scope_id, variable_type_t var_type);

/*
   Enriquecer símbolo com informações de memória
*/
void enrich_symbol_memory(parser_t *parser, const char *name, int address, int size);

/*
   Enriquecer símbolo com informações de array
*/
void enrich_symbol_array(parser_t *parser, const char *name, int *dimensions, int dim_count);

/*
   Enriquecer símbolo com informações de função
*/
void enrich_symbol_function(parser_t *parser, const char *name, int return_type, 
                            int *param_types, int param_count);

/* ============================================================================
   FUNÇÕES DE PARSING (Não-terminais da gramática)
   ============================================================================ */

/* Entry point */
ast_node_t* parse_programa(parser_t *parser);

/* Global declarations */
void parse_lista_declaracoes_globais(parser_t *parser, ast_node_t *program_node);
void parse_declaracao_global(parser_t *parser, ast_node_t *program_node);

/* Type specifiers */
int parse_especificador_tipo(parser_t *parser);
/* Variante estendida: captura o tag de struct/union (ou tag base de um typedef
   de struct) em tag_name_out, e soma em *ptr_extra_out o nível de ponteiro
   embutido num typedef (ex.: typedef char *texto → +1). Ambos opcionais (NULL). */
int parse_especificador_tipo_ex(parser_t *parser, char *tag_name_out, size_t out_len,
                                int *ptr_extra_out);

/* Statements */
ast_node_t* parse_bloco(parser_t *parser);
ast_node_t* parse_instrucao(parser_t *parser);

/* Expressions */
ast_node_t* parse_expressao(parser_t *parser);
ast_node_t* parse_atribuicao(parser_t *parser);

/* Utility functions */
int parse_asteriscos(parser_t *parser);
int parse_sufixo_array(parser_t *parser, int *dimensions);
ast_node_t* parse_inicializacao(parser_t *parser);
int is_type_specifier(parser_t *parser, token_t token);

/* Error recovery */
void panic_mode_recovery(parser_t *parser);

#endif /* PARSER_H */

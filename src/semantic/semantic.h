#ifndef SEMANTIC_H
#define SEMANTIC_H

/* ============================================================================
   ANALISADOR SEMÂNTICO (SEMANTIC ANALYZER) - HEADER

   Terceira fase do compilador. Recebe a AST produzida pelo parser e
   percorre-a recursivamente (visitor pattern) verificando:

     a) Variável não declarada (uso antes da declaração)
     b) Variável declarada duas vezes no mesmo escopo
     c) Incompatibilidade de tipos em operações binárias
     d) Tipos dos argumentos em chamadas de função
     e) Atribuição de tipo incompatível a uma variável
     f) Condições em estruturas de controlo (if/while/for/do-while/switch)

   O módulo é totalmente independente do scanner e do parser — apenas lê a
   AST e a tabela de símbolos já preenchidas.
   ============================================================================ */

#include "../parser/parser.h"
#include "../parser/ast.h"
#include "../parser/scope.h"

/* ============================================================================
   ESTRUTURA DO ANALISADOR SEMÂNTICO
   ============================================================================ */

/*
   Contexto global do analisador semântico.

   Campos:
   - parser       : acesso à AST e à hierarquia de escopos já construída
   - error_count  : número total de erros semânticos encontrados
   - source_path  : caminho do ficheiro fonte (para mensagens de erro)
   - current_return_type : tipo de retorno da função em análise (KW_VOID se
                    não estiver dentro de nenhuma função)
   - in_function  : 1 se o visitor está dentro do corpo de uma função
   - scope_stack  : espelho da pilha de escopos durante a travessia da AST
   - scope_stack_size / scope_stack_cap : gestão da pilha
*/
typedef struct semantic_s {
    parser_t   *parser;              /* parser com AST + tabela de símbolos  */
    int         error_count;         /* erros semânticos encontrados          */
    const char *source_path;         /* caminho do ficheiro fonte             */

    /* Contexto da função actual */
    int  current_return_type;        /* KW_INT, KW_VOID, etc.                */
    int  in_function;                /* 1 se dentro do corpo de uma função   */
    char current_func_name[128];     /* nome da função actual (para erros)   */

    /* Pilha de escopos semânticos (reflecte a travessia da AST) */
    scope_t **scope_stack;           /* pilha de scope_t*                    */
    int       scope_stack_size;      /* profundidade actual da pilha          */
    int       scope_stack_cap;       /* capacidade alocada                   */

    /* Índice sequencial do próximo escopo a empurrar de all_scopes[].
       O parser regista os escopos em DFS (mesma ordem da AST), por isso
       basta avançar este índice cada vez que entramos num bloco. */
    int next_scope_idx;              /* próximo escopo a consumir de all_scopes[] */

    /* Registo de declarações JÁ VISTAS nesta travessia, por (scope_id, nome).
       A tabela do parser guarda cada nome só uma vez (rejeita duplicados no
       registo), portanto a redeclaração (alínea b) tem de ser detectada aqui,
       comparando o que já foi visitado dentro do mesmo escopo. */
    struct seen_decl_s {
        int  scope_id;
        char name[64];
    } *seen_decls;
    int seen_count;
    int seen_cap;
} semantic_t;

/* ============================================================================
   FUNÇÕES PÚBLICAS
   ============================================================================ */

/*
   Inicializar o analisador semântico.

   Entrada: parser — parser com AST e tabela de símbolos preenchidos
   Saída  : ponteiro para semantic_t, ou NULL em caso de erro de memória
*/
semantic_t* semantic_init(parser_t *parser);

/*
   Executar a análise semântica completa sobre a AST.

   Entrada: sem — analisador semântico inicializado
   Saída  : número de erros semânticos encontrados (0 = programa válido)
*/
int semantic_run(semantic_t *sem);

/*
   Libertar os recursos do analisador semântico.
   Não liberta o parser — a sua posse continua em main.c.

   Entrada: sem — analisador semântico a libertar
   Saída  : void
*/
void semantic_free(semantic_t *sem);

/* ============================================================================
   FUNÇÕES INTERNAS (utilizadas entre semantic_*.c)
   Declaradas aqui para que semantic_expr.c, semantic_stmt.c e
   semantic_decl.c as possam chamar sem forward declarations repetidas.
   ============================================================================ */

/* Emitir erro semântico com formatação GCC-style */
void sem_error(semantic_t *sem, int line, int col, const char *fmt, ...);

/* Procurar símbolo na pilha de escopos semânticos (do mais interior para cima) */
symbol_info_t* sem_lookup(semantic_t *sem, const char *name);

/* Procurar símbolo apenas no escopo do topo da pilha (escopo actual) */
int sem_exists_current_scope(semantic_t *sem, const char *name);

/*
   Registar uma declaração (nome) no escopo actual durante a travessia.
   Devolve 1 se o nome JÁ tinha sido declarado neste escopo (redeclaração),
   0 se é a primeira vez. Usado para a alínea b).
*/
int sem_mark_declared(semantic_t *sem, const char *name);

/* Empurrar / retirar escopo da pilha semântica */
void sem_push_scope(semantic_t *sem, scope_t *scope);
void sem_pop_scope(semantic_t *sem);

/* ============================================================================
   FUNÇÕES DE VERIFICAÇÃO (implementadas em semantic_expr/stmt/decl.c)
   ============================================================================ */

/*
   Inferir o nível de indirecção (is_pointer) de uma expressão.
   Retorna 0 se não for ponteiro, > 0 para ponteiro(s).
*/
int inferir_is_pointer(semantic_t *sem, ast_node_t *node);

/*
   Inferir o tipo de retorno de uma expressão da AST.

   Retorna o tipo KW_* (KW_INT, KW_FLOAT, etc.) ou -1 se desconhecido.
   Não emite erros — é chamada pelas funções de verificação.
*/
int inferir_tipo(semantic_t *sem, ast_node_t *node);

/*
   Verificar um nó genérico da AST.
   Despacha para verificar_declaracao, verificar_instrucao ou verificar_expressao.
*/
void verificar_no(semantic_t *sem, ast_node_t *node);

/* Verificar declaração de variável ou função */
void verificar_declaracao(semantic_t *sem, ast_node_t *node);

/* Verificar instrução (if, while, for, do-while, switch, return, bloco) */
void verificar_instrucao(semantic_t *sem, ast_node_t *node);

/* Verificar expressão (binária, unária, atribuição, chamada, identificador) */
void verificar_expressao(semantic_t *sem, ast_node_t *node);

/* ============================================================================
   UTILITÁRIOS DE TIPO
   ============================================================================ */

/*
   Verificar se dois tipos são compatíveis para atribuição/operação.

   Regras aplicadas (subconjunto de C):
   - int  ↔ char  : compatível (promoção numérica)
   - int  ↔ float : compatível (promoção numérica)
   - char ↔ float : compatível (promoção numérica)
   - void ↔ outro : INCOMPATÍVEL
   - string       : compatível apenas com char* (is_pointer >= 1)

   Retorna 1 se compatível, 0 se incompatível.
*/
int tipos_compativeis(int tipo_a, int is_ptr_a, int tipo_b, int is_ptr_b);

/*
   Verificar se um tipo é escalar (válido como condição).

   Em C, qualquer tipo escalar (int, char, float, double, ponteiro) pode ser
   usado como condição. void é o único tipo inválido como condição.

   Retorna 1 se o tipo é válido como condição, 0 caso contrário.
*/
int tipo_valido_como_condicao(int tipo, int is_pointer);

/* Obter o nome legível de um tipo KW_* (para mensagens de erro) */
const char* sem_tipo_para_string(int tipo, int is_pointer);

#endif /* SEMANTIC_H */

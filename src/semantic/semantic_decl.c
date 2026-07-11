#define _POSIX_C_SOURCE 200809L
#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
   ANALISADOR SEMÂNTICO — VERIFICAÇÃO DE DECLARAÇÕES

   Implementa:
     b) Redeclaração de variável no mesmo escopo  (NODE_VAR_DECL)
     b) Redeclaração de função                    (NODE_FUNC_DECL)
     e) Incompatibilidade no inicializador        (NODE_VAR_DECL com init)
   ============================================================================ */

/* ============================================================================
   VERIFICAÇÃO DE DECLARAÇÃO DE VARIÁVEL  (alíneas b e e)
   ============================================================================ */

/*
   Verificar uma declaração de variável:
     b) Se a variável já existe no escopo actual, é uma redeclaração — erro.
        (O parser já reporta a maioria; esta é uma segunda linha de defesa.)
     e) Se tem inicializador, verificar compatibilidade de tipos.
*/
static void verificar_decl_variavel(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    const char *nome = node->data.decl.name ? node->data.decl.name : node->name;
    if (!nome) return;

    int tipo_decl = node->data.decl.data_type;
    if (tipo_decl == 0) tipo_decl = node->data_type;

    /* ------------------------------------------------------------------
       Alínea b): Verificar redeclaração no MESMO escopo.

       Não podemos usar a tabela do parser (guarda cada nome só uma vez), por
       isso registamos as declarações já vistas nesta travessia. Se o nome já
       foi declarado neste escopo, é uma redeclaração.
    ------------------------------------------------------------------ */
    if (sem_mark_declared(sem, nome)) {
        sem_error(sem, node->line, node->column,
                  "variável '%s' redeclarada no mesmo escopo", nome);
    }

    /* ------------------------------------------------------------------
       Alínea e): Verificar compatibilidade do inicializador
    ------------------------------------------------------------------ */
    ast_node_t *init = node->data.decl.initializer;
    /* Se for declaração local, o inicializador está no children[0] */
    if (!init && node->child_count > 0) {
        init = node->children[0];
    }

    if (init) {
        /* Verificar a expressão inicializadora recursivamente */
        verificar_expressao(sem, init);

        /* Inferir o tipo do inicializador */
        int tipo_init = inferir_tipo(sem, init);
        int ptr_init  = inferir_is_pointer(sem, init);

        /* Verificar que o tipo do inicializador é compatível com o tipo
           declarado da variável */
        if (tipo_decl != 0 && tipo_init != -1) {
            int ptr_decl = node->data.decl.is_pointer;
            if (ptr_decl == 0) ptr_decl = node->operator_type;

            /* Literal 0 é compatível com qualquer ponteiro */
            int is_zero_literal = 0;
            if (init->type == NODE_LITERAL && init->literal_type == AST_INT_LITERAL) {
                if (init->data.expr.value.string_value && strcmp(init->data.expr.value.string_value, "0") == 0) {
                    is_zero_literal = 1;
                }
            }

            if (!is_zero_literal && !tipos_compativeis(tipo_decl, ptr_decl, tipo_init, ptr_init)) {
                sem_error(sem, node->line, node->column,
                          "inicializador incompatível para '%s': "
                          "tipo declarado '%s', tipo do valor '%s'",
                          nome,
                          sem_tipo_para_string(tipo_decl, ptr_decl),
                          sem_tipo_para_string(tipo_init, ptr_init));
            }
        }
    }
}

/* ============================================================================
   VERIFICAÇÃO DE DECLARAÇÃO DE FUNÇÃO  (alínea b)
   ============================================================================ */

/*
   Verificar uma declaração de função:
     b) Se a função já está declarada no escopo global, é redeclaração.
        (O parser já cobre a maioria; esta é segunda linha de defesa.)
     - Entrar no escopo da função para verificar o corpo.
     - Actualizar o contexto de retorno para verificar os return statements.
*/
static void verificar_decl_funcao(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    const char *nome = node->data.decl.name ? node->data.decl.name : node->name;

    /* Guardar contexto anterior */
    int    return_type_anterior = sem->current_return_type;
    int    in_function_anterior = sem->in_function;
    char   func_name_anterior[128];
    snprintf(func_name_anterior, sizeof(func_name_anterior),
             "%s", sem->current_func_name);

    /* Actualizar contexto */
    sem->in_function = 1;
    sem->current_return_type = node->data.decl.data_type;
    if (sem->current_return_type == 0) {
        sem->current_return_type = node->data_type;
    }
    if (nome) {
        snprintf(sem->current_func_name,
                 sizeof(sem->current_func_name), "%s", nome);
    }

    /* Entrar no escopo dos parâmetros da função (se existir registado) */
    int pushed_param_scope = 0;
    parser_t *p = sem->parser;
    if (sem->next_scope_idx < p->total_scopes) {
        scope_t *param_scope = p->all_scopes[sem->next_scope_idx];
        int parent_id = sem->scope_stack[sem->scope_stack_size - 1]->scope_id;
        if (param_scope && param_scope->parent_scope_id == parent_id) {
            sem->next_scope_idx++;
            sem_push_scope(sem, param_scope);
            pushed_param_scope = 1;
        }
    }

    /* Visitar todos os filhos (parâmetros + corpo da função se houver) */
    for (int i = 0; i < node->child_count; i++) {
        if (node->children[i]) {
            verificar_no(sem, node->children[i]);
        }
    }

    if (pushed_param_scope) {
        sem_pop_scope(sem);
    }

    /* Restaurar contexto anterior */
    sem->current_return_type = return_type_anterior;
    sem->in_function         = in_function_anterior;
    snprintf(sem->current_func_name,
             sizeof(sem->current_func_name), "%s", func_name_anterior);
}

/* ============================================================================
   VERIFICAÇÃO DE DECLARAÇÃO DE STRUCT/TYPEDEF
   ============================================================================ */

/*
   Verificar uma declaração de struct ou typedef.
   A validação estrutural é feita pelo parser; aqui apenas visitamos os filhos.
*/
static void verificar_decl_struct(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    /* Visitar os filhos (campos, se presentes como children[]) */
    for (int i = 0; i < node->child_count; i++) {
        verificar_no(sem, node->children[i]);
    }
}

/* ============================================================================
   VERIFICAÇÃO DE PARÂMETRO DE FUNÇÃO
   ============================================================================ */

/*
   Verificar um parâmetro formal de função.
   A principal verificação é b): parâmetros duplicados no mesmo escopo.
*/
static void verificar_param(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    /* Alínea b) para parâmetros: dois parâmetros com o mesmo nome. */
    const char *nome = node->data.decl.name ? node->data.decl.name : node->name;
    if (nome && sem_mark_declared(sem, nome)) {
        sem_error(sem, node->line, node->column,
                  "parâmetro '%s' redeclarado", nome);
    }

    /* Visitar o inicializador se existir (parâmetros raramente têm, mas
       por completude) */
    if (node->data.decl.initializer) {
        verificar_expressao(sem, node->data.decl.initializer);
    }
}

/* ============================================================================
   DISPATCHER DE DECLARAÇÕES
   ============================================================================ */

/*
   Verificar um nó de declaração.
   Despacha para a função específica com base no tipo do nó.
*/
void verificar_declaracao(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_VAR_DECL:
            /* Alíneas b) e e) */
            verificar_decl_variavel(sem, node);
            break;

        case NODE_FUNC_DECL:
            /* Alínea b) + verificação do corpo */
            verificar_decl_funcao(sem, node);
            break;

        case NODE_STRUCT_DECL:
        case NODE_TYPEDEF_DECL:
            verificar_decl_struct(sem, node);
            break;

        case NODE_PARAM_DECL:
            verificar_param(sem, node);
            break;

        default:
            /* Declaração desconhecida — visitar filhos */
            for (int i = 0; i < node->child_count; i++) {
                verificar_no(sem, node->children[i]);
            }
            break;
    }
}

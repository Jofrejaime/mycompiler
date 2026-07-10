#define _POSIX_C_SOURCE 200809L
#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
   ANALISADOR SEMÂNTICO — VERIFICAÇÃO DE INSTRUÇÕES

   Implementa a alínea f): verificação dos argumentos/condições nas estruturas
   de controlo (if, while, for, do-while, switch).

   Também trata:
     - Blocos ({ ... }): gestão da pilha de escopos semânticos
     - return: verificação do tipo de retorno vs. assinatura da função
     - Instruções de expressão: delega em verificar_expressao
   ============================================================================ */

/* ============================================================================
   FUNÇÕES AUXILIARES LOCAIS
   ============================================================================ */

/*
   Verificar a condição de uma estrutura de controlo.
   Alínea f): a condição deve ser de um tipo válido (escalar).

   Emite erro se a condição for do tipo void.
*/
static void verificar_condicao(semantic_t *sem, ast_node_t *cond,
                                const char *estrutura) {
    if (!cond) return;

    /* Verificar a expressão da condição recursivamente */
    verificar_expressao(sem, cond);

    /* Inferir o tipo e verificar se é válido como condição */
    int tipo    = inferir_tipo(sem, cond);
    int is_ptr  = inferir_is_pointer(sem, cond) ;
    /* inferir_is_pointer está em semantic_expr.c — aqui chamamos inferir_tipo */

    if (tipo != -1 && !tipo_valido_como_condicao(tipo, is_ptr)) {
        sem_error(sem, cond->line, cond->column,
                  "condição do '%s' tem tipo inválido '%s' — "
                  "é necessário um tipo escalar",
                  estrutura,
                  sem_tipo_para_string(tipo, is_ptr));
    }
}

/* ============================================================================
   VERIFICAÇÃO DE BLOCO
   ============================================================================ */

/*
   Verificar um bloco { ... }.

   Ao entrar num bloco, tenta empurrar o próximo escopo registado na pilha.
   O parser regista escopos em DFS — a mesma ordem em que o visitor os visita.
   Por isso, o N-ésimo bloco visitado corresponde ao N-ésimo escopo em
   all_scopes[next_scope_idx].

   Ao sair do bloco, retira o escopo da pilha semântica.
*/
static void verificar_bloco(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    /* Tentar obter o próximo escopo registado (DFS order = AST order) */
    parser_t *p    = sem->parser;
    scope_t  *scope = NULL;
    int pushed = 0;

    if (sem->next_scope_idx < p->total_scopes) {
        scope = p->all_scopes[sem->next_scope_idx];
        /* Verificar que este escopo é filho do escopo actual */
        int parent_id = sem->scope_stack[sem->scope_stack_size - 1]->scope_id;
        if (scope && scope->parent_scope_id == parent_id) {
            sem->next_scope_idx++;
            sem_push_scope(sem, scope);
            pushed = 1;
        }
    }

    /* Verificar cada instrução/declaração dentro do bloco */
    for (int i = 0; i < node->child_count; i++) {
        verificar_no(sem, node->children[i]);
    }

    /* Verificar também instruções guardadas em data.stmt.statements */
    if (node->data.stmt.statements) {
        for (int i = 0; i < node->data.stmt.stmt_count; i++) {
            verificar_no(sem, node->data.stmt.statements[i]);
        }
    }

    if (pushed) sem_pop_scope(sem);
}

/* ============================================================================
   VERIFICAÇÃO DE IF-ELSE  (alínea f)
   ============================================================================ */

/*
   Verificar instrução if-else:
     - A condição deve ser de tipo escalar.
     - Visitar o ramo then e o ramo else (se existir).
*/
static void verificar_if(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    /* A condição é o primeiro filho */
    ast_node_t *cond      = (node->child_count > 0) ? node->children[0] : NULL;
    ast_node_t *then_stmt = (node->child_count > 1) ? node->children[1] : NULL;
    ast_node_t *else_stmt = (node->child_count > 2) ? node->children[2] : NULL;

    /* Fallback: campos do union (compatibilidade com o parser) */
    if (!cond)      cond      = node->data.stmt.condition;
    if (!then_stmt) then_stmt = node->data.stmt.then_branch;
    if (!else_stmt) else_stmt = node->data.stmt.else_branch;

    /* Alínea f): verificar a condição */
    verificar_condicao(sem, cond, "if");

    /* Visitar os ramos */
    verificar_no(sem, then_stmt);
    verificar_no(sem, else_stmt);
}

/* ============================================================================
   VERIFICAÇÃO DE WHILE  (alínea f)
   ============================================================================ */

static void verificar_while(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    ast_node_t *cond = (node->child_count > 0) ? node->children[0] : NULL;
    ast_node_t *body = (node->child_count > 1) ? node->children[1] : NULL;

    if (!cond) cond = node->data.stmt.condition;
    if (!body) body = node->data.stmt.then_branch;

    /* Alínea f): verificar a condição */
    verificar_condicao(sem, cond, "while");

    /* Visitar o corpo */
    verificar_no(sem, body);
}

/* ============================================================================
   VERIFICAÇÃO DE DO-WHILE  (alínea f)
   ============================================================================ */

static void verificar_do_while(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    ast_node_t *body = (node->child_count > 0) ? node->children[0] : NULL;
    ast_node_t *cond = (node->child_count > 1) ? node->children[1] : NULL;

    if (!body) body = node->data.stmt.then_branch;
    if (!cond) cond = node->data.stmt.condition;

    /* Visitar o corpo primeiro (do-while executa o corpo antes de verificar) */
    verificar_no(sem, body);

    /* Alínea f): verificar a condição */
    verificar_condicao(sem, cond, "do-while");
}

/* ============================================================================
   VERIFICAÇÃO DE FOR  (alínea f)
   ============================================================================ */

/*
   Verificar instrução for:
     - Inicialização: pode ser declaração ou expressão.
     - Condição: deve ser de tipo escalar (alínea f).
     - Incremento: expressão qualquer.
     - Corpo: instrução ou bloco.

   O parser constrói o for com filhos na ordem: [init, cond, incr, body].
*/
static void verificar_for(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    /* Os filhos do for estão em children[0..3] (com possíveis NULLs) */
    ast_node_t *init = NULL, *cond = NULL, *incr = NULL, *body = NULL;

    if (node->child_count >= 4) {
        init = node->children[0];
        cond = node->children[1];
        incr = node->children[2];
        body = node->children[3];
    } else {
        /* Fallback para campos do union */
        init = node->data.stmt.init;
        cond = node->data.stmt.condition;
        incr = node->data.stmt.increment;
        body = node->data.stmt.then_branch;
    }

    /* Verificar inicialização */
    if (init) verificar_no(sem, init);

    /* Alínea f): verificar condição */
    if (cond) verificar_condicao(sem, cond, "for");

    /* Verificar incremento */
    if (incr) verificar_expressao(sem, incr);

    /* Verificar corpo */
    verificar_no(sem, body);
}

/* ============================================================================
   VERIFICAÇÃO DE SWITCH  (alínea f)
   ============================================================================ */

/*
   Verificar instrução switch:
     - A expressão de controlo deve ser de tipo inteiro (int ou char).
     - Visitar todos os casos (children[1..n]).
*/
static void verificar_switch(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    /* A expressão de controlo é o primeiro filho */
    ast_node_t *expr = (node->child_count > 0) ? node->children[0] : NULL;
    if (!expr) expr = node->data.stmt.condition;

    if (expr) {
        verificar_expressao(sem, expr);

        int tipo   = inferir_tipo(sem, expr);
        int is_ptr = 0; /* switch não aceita ponteiros */

        /* Switch exige tipo inteiro (int ou char) */
        if (tipo != -1 && tipo != KW_INT && tipo != KW_CHAR) {
            sem_error(sem, expr->line, expr->column,
                      "expressão do 'switch' deve ser de tipo inteiro, "
                      "mas é '%s'",
                      sem_tipo_para_string(tipo, is_ptr));
        }
    }

    /* Visitar os casos */
    for (int i = 1; i < node->child_count; i++) {
        verificar_no(sem, node->children[i]);
    }
}

/* ============================================================================
   VERIFICAÇÃO DE RETURN
   ============================================================================ */

/*
   Verificar instrução return:
     - Se a função tem tipo de retorno void, não deve retornar valor.
     - Se a função tem tipo de retorno não-void, o valor de retorno
       deve ser compatível com o tipo declarado.
*/
static void verificar_return(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    /* A expressão de retorno é o primeiro filho */
    ast_node_t *expr = (node->child_count > 0) ? node->children[0] : NULL;
    if (!expr) expr = node->data.stmt.condition; /* fallback */

    if (!sem->in_function) {
        /* return fora de função — erro sintático já apanhado pelo parser */
        return;
    }

    if (sem->current_return_type == KW_VOID) {
        /* Função void: não deve retornar valor */
        if (expr) {
            sem_error(sem, node->line, node->column,
                      "função '%s' é void mas retorna um valor",
                      sem->current_func_name[0] ? sem->current_func_name : "?");
        }
    } else {
        /* Função com tipo: deve retornar valor compatível */
        if (!expr) {
            sem_error(sem, node->line, node->column,
                      "função '%s' deve retornar '%s' mas não retorna valor",
                      sem->current_func_name[0] ? sem->current_func_name : "?",
                      sem_tipo_para_string(sem->current_return_type, 0));
        } else {
            verificar_expressao(sem, expr);

            int tipo_ret = inferir_tipo(sem, expr);
            int ptr_ret  = 0;

            if (tipo_ret != -1 &&
                !tipos_compativeis(tipo_ret, ptr_ret,
                                   sem->current_return_type, 0)) {
                sem_error(sem, expr->line, expr->column,
                          "tipo de retorno incompatível: retorna '%s', "
                          "função '%s' declarada como '%s'",
                          sem_tipo_para_string(tipo_ret, ptr_ret),
                          sem->current_func_name[0] ? sem->current_func_name : "?",
                          sem_tipo_para_string(sem->current_return_type, 0));
            }
        }
    }
}

/* ============================================================================
   DISPATCHER DE INSTRUÇÕES
   ============================================================================ */

/*
   Verificar uma instrução.
   Despacha para a função de verificação específica com base no tipo do nó.
*/
void verificar_instrucao(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    switch (node->type) {

        case NODE_BLOCK:
            verificar_bloco(sem, node);
            break;

        case NODE_IF_STMT:
            /* Alínea f): condição do if */
            verificar_if(sem, node);
            break;

        case NODE_WHILE_STMT:
            /* Alínea f): condição do while */
            verificar_while(sem, node);
            break;

        case NODE_DO_WHILE_STMT:
            /* Alínea f): condição do do-while */
            verificar_do_while(sem, node);
            break;

        case NODE_FOR_STMT:
            /* Alínea f): condição do for */
            verificar_for(sem, node);
            break;

        case NODE_SWITCH_STMT:
            /* Alínea f): expressão do switch deve ser inteira */
            verificar_switch(sem, node);
            break;

        case NODE_RETURN_STMT:
            verificar_return(sem, node);
            break;

        case NODE_EXPR_STMT:
            /* Instrução de expressão: verificar a expressão */
            if (node->child_count > 0) {
                verificar_expressao(sem, node->children[0]);
            }
            if (node->data.stmt.condition) {
                verificar_expressao(sem, node->data.stmt.condition);
            }
            break;

        case NODE_BREAK_STMT:
        case NODE_CONTINUE_STMT:
            /* Sem verificação semântica adicional */
            break;

        case NODE_GOTO_STMT:
        case NODE_LABEL_STMT:
            /* Sem verificação semântica adicional */
            break;

        default:
            /* Instrução desconhecida — visitar filhos */
            for (int i = 0; i < node->child_count; i++) {
                verificar_no(sem, node->children[i]);
            }
            break;
    }
}

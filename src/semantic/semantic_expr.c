#define _POSIX_C_SOURCE 200809L
#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
   ANALISADOR SEMÂNTICO — VERIFICAÇÃO DE EXPRESSÕES

   Implementa:
     a) Verificação de variável não declarada          (NODE_IDENTIFIER)
     c) Incompatibilidade de tipos em operações        (NODE_BINARY_OP)
     d) Tipos dos argumentos em chamadas de função     (NODE_CALL)
     e) Atribuição de tipo incompatível                (NODE_ASSIGN)
   ============================================================================ */

/* ============================================================================
   INFERÊNCIA DE TIPO
   ============================================================================ */

/*
   Inferir o tipo de retorno de uma expressão da AST.

   Esta função percorre o nó de expressão e devolve o tipo KW_* do resultado.
   Não emite erros — é chamada pelas funções de verificação.

   Retorna -1 se o tipo não puder ser determinado (ex.: erro anterior).
*/
int inferir_tipo(semantic_t *sem, ast_node_t *node) {
    if (!node) return -1;

    switch (node->type) {

        /* ------------------------------------------------------------------
           Literais: tipo determinado pelo subtipo do literal
        ------------------------------------------------------------------ */
        case NODE_LITERAL:
            switch (node->literal_type) {
                case AST_INT_LITERAL:    return KW_INT;
                case AST_FLOAT_LITERAL:  return KW_FLOAT;
                case AST_CHAR_LITERAL:   return KW_CHAR;
                case AST_STRING_LITERAL: return KW_CHAR; /* char* */
                default:                 return KW_INT;
            }

        /* ------------------------------------------------------------------
           Identificadores: tipo obtido da tabela de símbolos
        ------------------------------------------------------------------ */
        case NODE_IDENTIFIER: {
            const char *nome = node->data.expr.identifier
                               ? node->data.expr.identifier
                               : node->name;
            if (!nome) return -1;

            symbol_info_t *info = sem_lookup(sem, nome);
            if (!info) return -1; /* Erro já reportado em verificar_expressao */
            return info->data_type;
        }

        /* ------------------------------------------------------------------
           Operadores binários: promoção de tipo entre os dois operandos
        ------------------------------------------------------------------ */
        case NODE_BINARY_OP: {
            int op = node->data.expr.operator;

            /* Operadores relacionais e lógicos sempre devolvem int (bool em C) */
            if (op == OP_EQ || op == OP_NE || op == OP_LT || op == OP_LE ||
                op == OP_GT || op == OP_GE || op == OP_AND || op == OP_OR) {
                return KW_INT;
            }

            int tipo_esq = inferir_tipo(sem, node->data.expr.left);
            int tipo_dir = inferir_tipo(sem, node->data.expr.right);

            if (tipo_esq == -1) return tipo_dir;
            if (tipo_dir == -1) return tipo_esq;

            /* Promoção numérica: double > float > int > char */
            if (tipo_esq == KW_DOUBLE || tipo_dir == KW_DOUBLE) return KW_DOUBLE;
            if (tipo_esq == KW_FLOAT  || tipo_dir == KW_FLOAT)  return KW_FLOAT;
            return KW_INT;
        }

        /* ------------------------------------------------------------------
           Operadores unários: mantêm o tipo do operando
        ------------------------------------------------------------------ */
        case NODE_UNARY_OP: {
            int op = node->data.expr.operator;
            /* ! e == sempre devolvem int */
            if (op == OP_NOT) return KW_INT;
            return inferir_tipo(sem, node->data.expr.left);
        }

        /* ------------------------------------------------------------------
           Atribuição: tipo é o do lado esquerdo
        ------------------------------------------------------------------ */
        case NODE_ASSIGN:
            if (node->child_count >= 1) {
                return inferir_tipo(sem, node->children[0]);
            }
            return inferir_tipo(sem, node->data.expr.left);

        /* ------------------------------------------------------------------
           Chamada de função: tipo de retorno da função
        ------------------------------------------------------------------ */
        case NODE_CALL: {
            /* O primeiro filho é o identificador da função */
            if (node->child_count < 1) return -1;
            ast_node_t *fn_node = node->children[0];
            const char *fn_nome = fn_node ? (fn_node->data.expr.identifier
                                             ? fn_node->data.expr.identifier
                                             : fn_node->name) : NULL;
            if (!fn_nome) return -1;

            symbol_info_t *info = sem_lookup(sem, fn_nome);
            if (!info) return -1;
            return info->is_function ? info->function_return_type : info->data_type;
        }

        /* ------------------------------------------------------------------
           Acesso a array: tipo do elemento base do array
        ------------------------------------------------------------------ */
        case NODE_ARRAY_ACCESS: {
            if (node->child_count < 1) return -1;
            return inferir_tipo(sem, node->children[0]);
        }

        /* ------------------------------------------------------------------
           Acesso a membro: tipo do campo (simplificado — retorna int se
           não conseguir resolver; o parser já validou a estrutura)
        ------------------------------------------------------------------ */
        case NODE_MEMBER_ACCESS:
            return KW_INT; /* Simplificação: parser já verificou o campo */

        /* ------------------------------------------------------------------
           Ternário: tipo do ramo then (ambos devem ser compatíveis)
        ------------------------------------------------------------------ */
        case NODE_TERNARY:
            if (node->child_count >= 2) {
                return inferir_tipo(sem, node->children[1]);
            }
            return -1;

        default:
            return -1;
    }
}

/*
   Inferir o nível de indireção (is_pointer) de uma expressão.
   Retorna 0 se não for ponteiro, > 0 para ponteiro(s).
*/
int inferir_is_pointer(semantic_t *sem, ast_node_t *node) {
    if (!node) return 0;

    switch (node->type) {
        case NODE_IDENTIFIER: {
            const char *nome = node->data.expr.identifier
                               ? node->data.expr.identifier : node->name;
            if (!nome) return 0;
            symbol_info_t *info = sem_lookup(sem, nome);
            return info ? info->is_pointer : 0;
        }
        case NODE_LITERAL:
            /* Literal de string é char* */
            return (node->literal_type == AST_STRING_LITERAL) ? 1 : 0;
        case NODE_UNARY_OP:
            /* &x → ponteiro; *x → desreferência */
            if (node->data.expr.operator == OP_BITAND) {
                return inferir_is_pointer(sem, node->data.expr.left) + 1;
            }
            if (node->data.expr.operator == OP_MULT) {
                int p = inferir_is_pointer(sem, node->data.expr.left);
                return (p > 0) ? p - 1 : 0;
            }
            return 0;
        case NODE_ASSIGN:
            if (node->child_count >= 1) {
                return inferir_is_pointer(sem, node->children[0]);
            }
            return 0;
        case NODE_CALL: {
            if (node->child_count < 1) return 0;
            ast_node_t *fn = node->children[0];
            const char *nome = fn ? (fn->data.expr.identifier
                                     ? fn->data.expr.identifier : fn->name) : NULL;
            if (!nome) return 0;
            symbol_info_t *info = sem_lookup(sem, nome);
            return info ? info->is_pointer : 0;
        }
        default:
            return 0;
    }
}

/* ============================================================================
   VERIFICAÇÃO DE CHAMADA DE FUNÇÃO  (alínea d)
   ============================================================================ */

/*
   Verificar uma chamada de função:
     - Verificar que o identificador existe e é uma função.
     - Verificar o número de argumentos vs. parâmetros declarados.
     - Verificar o tipo de cada argumento vs. tipo do parâmetro correspondente.
*/
static void verificar_chamada_funcao(semantic_t *sem, ast_node_t *node) {
    if (!node || node->child_count < 1) return;

    ast_node_t *fn_node = node->children[0];
    const char *fn_nome = fn_node
                          ? (fn_node->data.expr.identifier
                             ? fn_node->data.expr.identifier
                             : fn_node->name)
                          : NULL;

    if (!fn_nome) return;

    /* Verificar se a função está declarada */
    symbol_info_t *info = sem_lookup(sem, fn_nome);
    if (!info) {
        /* Erro de variável não declarada já emitido por verificar_expressao */
        return;
    }

    /* Verificar que é de facto uma função */
    if (!info->is_function) {
        sem_error(sem, node->line, node->column,
                  "'%s' não é uma função — não pode ser chamada", fn_nome);
        return;
    }

    /* Número de argumentos fornecidos (filhos[0] é o nome da função) */
    int n_args = node->child_count - 1;
    int n_params = info->function_param_count;

    if (n_args != n_params) {
        sem_error(sem, node->line, node->column,
                  "chamada a '%s': %d argumento(s) fornecido(s), "
                  "mas a função espera %d",
                  fn_nome, n_args, n_params);
        /* Continua para verificar os argumentos disponíveis */
    }

    /* Verificar o tipo de cada argumento */
    int min_args = (n_args < n_params) ? n_args : n_params;
    for (int i = 0; i < min_args; i++) {
        ast_node_t *arg = node->children[i + 1]; /* filhos[0] = nome da fn */
        if (!arg) continue;

        int tipo_arg    = inferir_tipo(sem, arg);
        int ptr_arg     = inferir_is_pointer(sem, arg);
        int tipo_param  = info->function_param_types[i];
        int ptr_param   = 0; /* o parser não armazena is_pointer dos params */

        if (tipo_arg == -1) continue; /* tipo desconhecido — erro já reportado */

        if (!tipos_compativeis(tipo_arg, ptr_arg, tipo_param, ptr_param)) {
            sem_error(sem, arg->line, arg->column,
                      "argumento %d de '%s': tipo incompatível — "
                      "fornecido '%s', esperado '%s'",
                      i + 1, fn_nome,
                      sem_tipo_para_string(tipo_arg, ptr_arg),
                      sem_tipo_para_string(tipo_param, ptr_param));
        }

        /* Verificar o argumento recursivamente */
        verificar_expressao(sem, arg);
    }

    /* Verificar os argumentos extra (em caso de excesso) */
    for (int i = n_params; i < n_args; i++) {
        ast_node_t *arg = node->children[i + 1];
        if (arg) verificar_expressao(sem, arg);
    }
}

/* ============================================================================
   VERIFICAÇÃO DE ATRIBUIÇÃO  (alínea e)
   ============================================================================ */

/*
   Verificar uma atribuição (=, +=, -=, *=, /=):
     - Inferir o tipo do lado esquerdo (lvalue).
     - Inferir o tipo do lado direito (rvalue).
     - Verificar compatibilidade de tipos.
*/
static void verificar_atribuicao(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    ast_node_t *esq = (node->child_count > 0) ? node->children[0] : NULL;
    ast_node_t *dir = (node->child_count > 1) ? node->children[1] : NULL;

    if (!esq) esq = node->data.expr.left;
    if (!dir) dir = node->data.expr.right;

    /* Verificar recursivamente os dois lados primeiro */
    verificar_expressao(sem, esq);
    verificar_expressao(sem, dir);

    if (!esq || !dir) return;

    int tipo_esq = inferir_tipo(sem, esq);
    int ptr_esq  = inferir_is_pointer(sem, esq);
    int tipo_dir = inferir_tipo(sem, dir);
    int ptr_dir  = inferir_is_pointer(sem, dir);

    if (tipo_esq == -1 || tipo_dir == -1) return; /* erro já reportado */

    /* Verificar compatibilidade */
    int is_zero_literal = 0;
    if (dir->type == NODE_LITERAL && dir->literal_type == AST_INT_LITERAL) {
        if (dir->data.expr.value.string_value && strcmp(dir->data.expr.value.string_value, "0") == 0) {
            is_zero_literal = 1;
        }
    }

    if (!is_zero_literal && !tipos_compativeis(tipo_esq, ptr_esq, tipo_dir, ptr_dir)) {
        sem_error(sem, node->line, node->column,
                  "atribuição incompatível: não é possível atribuir '%s' a '%s'",
                  sem_tipo_para_string(tipo_dir, ptr_dir),
                  sem_tipo_para_string(tipo_esq, ptr_esq));
    }
}

/* ============================================================================
   VERIFICAÇÃO DE OPERADOR BINÁRIO  (alínea c)
   ============================================================================ */

/*
   Verificar uma operação binária:
     - Inferir os tipos dos dois operandos.
     - Verificar compatibilidade para a operação.
     - Verificar recursivamente cada operando.
*/
static void verificar_binario(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    ast_node_t *esq = node->data.expr.left;
    ast_node_t *dir = node->data.expr.right;

    /* Verificar cada lado recursivamente primeiro */
    verificar_expressao(sem, esq);
    verificar_expressao(sem, dir);

    if (!esq || !dir) return;

    int tipo_esq = inferir_tipo(sem, esq);
    int ptr_esq  = inferir_is_pointer(sem, esq);
    int tipo_dir = inferir_tipo(sem, dir);
    int ptr_dir  = inferir_is_pointer(sem, dir);

    if (tipo_esq == -1 || tipo_dir == -1) return;

    int op = node->data.expr.operator;

    /* Operadores aritméticos (+, -, *, /, %) exigem tipos numéricos */
    if (op == OP_PLUS || op == OP_MINUS || op == OP_MULT ||
        op == OP_DIV  || op == OP_MOD) {
        if (!tipos_compativeis(tipo_esq, ptr_esq, tipo_dir, ptr_dir)) {
            sem_error(sem, node->line, node->column,
                      "tipos incompatíveis na operação aritmética: "
                      "'%s' e '%s'",
                      sem_tipo_para_string(tipo_esq, ptr_esq),
                      sem_tipo_para_string(tipo_dir, ptr_dir));
        }
        /* Não se pode somar dois ponteiros */
        if (ptr_esq > 0 && ptr_dir > 0 && op == OP_PLUS) {
            sem_error(sem, node->line, node->column,
                      "não é permitido somar dois ponteiros");
        }
    }

    /* Operadores de comparação: os tipos devem ser compatíveis */
    if (op == OP_EQ || op == OP_NE || op == OP_LT ||
        op == OP_LE || op == OP_GT || op == OP_GE) {
        if (!tipos_compativeis(tipo_esq, ptr_esq, tipo_dir, ptr_dir)) {
            sem_error(sem, node->line, node->column,
                      "comparação entre tipos incompatíveis: "
                      "'%s' e '%s'",
                      sem_tipo_para_string(tipo_esq, ptr_esq),
                      sem_tipo_para_string(tipo_dir, ptr_dir));
        }
    }
}

/* ============================================================================
   VERIFICAÇÃO DE IDENTIFICADOR  (alínea a)
   ============================================================================ */

/*
   Verificar que um identificador foi declarado antes de ser utilizado.
   Esta é a verificação da alínea a).
*/
static void verificar_identificador(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    const char *nome = node->data.expr.identifier
                       ? node->data.expr.identifier
                       : node->name;
    if (!nome) return;

    symbol_info_t *info = sem_lookup(sem, nome);
    if (!info) {
        sem_error(sem, node->line, node->column,
                  "variável '%s' não declarada", nome);
    }
}

/* ============================================================================
   DISPATCHER DE EXPRESSÕES
   ============================================================================ */

/*
   Verificar um nó de expressão.
   Despacha para a função de verificação específica com base no tipo do nó.
*/
void verificar_expressao(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    switch (node->type) {

        case NODE_IDENTIFIER:
            verificar_identificador(sem, node);
            break;

        case NODE_LITERAL:
            /* Literais são sempre válidos — sem verificação */
            break;

        case NODE_ASSIGN:
            /* Alínea e): verificação de atribuição com compatibilidade de tipos */
            verificar_atribuicao(sem, node);
            break;

        case NODE_BINARY_OP:
            /* Alínea c): verificação de incompatibilidade em operações */
            verificar_binario(sem, node);
            break;

        case NODE_CALL:
            /* Alínea d): verificação de argumentos de função */
            verificar_chamada_funcao(sem, node);
            break;

        case NODE_UNARY_OP:
            verificar_expressao(sem, node->data.expr.left);
            break;

        case NODE_ARRAY_ACCESS:
            /* Verificar o array e o índice */
            if (node->child_count >= 1) verificar_expressao(sem, node->children[0]);
            if (node->child_count >= 2) verificar_expressao(sem, node->children[1]);
            break;

        case NODE_MEMBER_ACCESS:
            /* Verificar o objecto (lado esquerdo) */
            if (node->child_count >= 1) verificar_expressao(sem, node->children[0]);
            break;

        case NODE_TERNARY:
            /* Verificar os três filhos: condição, then, else */
            for (int i = 0; i < node->child_count; i++) {
                verificar_expressao(sem, node->children[i]);
            }
            break;

        case NODE_INIT_LIST:
            /* Lista de inicialização: verificar cada elemento */
            for (int i = 0; i < node->child_count; i++) {
                verificar_expressao(sem, node->children[i]);
            }
            break;

        default:
            /* Nó de expressão desconhecido — visitar filhos */
            for (int i = 0; i < node->child_count; i++) {
                verificar_expressao(sem, node->children[i]);
            }
            break;
    }
}

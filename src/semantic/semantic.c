#define _POSIX_C_SOURCE 200809L
#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* ============================================================================
   ANALISADOR SEMÂNTICO — NÚCLEO
   Implementa: inicialização, execução, limpeza, sistema de erros e gestão
   da pilha de escopos semânticos.
   ============================================================================ */

/* Códigos ANSI (idênticos aos usados em parser_error.c) */
#define ANSI_RED    "\x1b[31m"
#define ANSI_BOLD   "\x1b[1m"
#define ANSI_RESET  "\x1b[0m"
#define ANSI_YELLOW "\x1b[33m"

/* ============================================================================
   INICIALIZAÇÃO E LIMPEZA
   ============================================================================ */

semantic_t* semantic_init(parser_t *parser) {
    if (!parser) return NULL;

    semantic_t *sem = (semantic_t*)malloc(sizeof(semantic_t));
    if (!sem) return NULL;

    sem->parser              = parser;
    sem->error_count         = 0;
    sem->source_path         = parser->source_path;
    sem->current_return_type = KW_VOID;
    sem->in_function         = 0;
    sem->current_func_name[0] = '\0';

    /* Pilha de escopos semânticos — começa com o escopo global */
    sem->scope_stack_cap  = 32;
    sem->scope_stack_size = 0;
    sem->scope_stack = (scope_t**)malloc(sizeof(scope_t*) * sem->scope_stack_cap);
    if (!sem->scope_stack) {
        free(sem);
        return NULL;
    }

    /* O escopo global é sempre o primeiro da pilha */
    sem_push_scope(sem, parser->global_scope);

    /* O índice 0 de all_scopes[] é o global (já na pilha); começar em 1 */
    sem->next_scope_idx = 1;

    return sem;
}

void semantic_free(semantic_t *sem) {
    if (!sem) return;
    free(sem->scope_stack);
    free(sem);
}

/* ============================================================================
   GESTÃO DA PILHA DE ESCOPOS
   ============================================================================ */

/*
   Empurrar um escopo na pilha semântica.
   Chamado antes de entrar num bloco, função ou qualquer escopo aninhado.
*/
void sem_push_scope(semantic_t *sem, scope_t *scope) {
    if (!sem || !scope) return;

    /* Redimensionar a pilha se necessário */
    if (sem->scope_stack_size >= sem->scope_stack_cap) {
        sem->scope_stack_cap *= 2;
        scope_t **new_stack = (scope_t**)realloc(sem->scope_stack,
                              sizeof(scope_t*) * sem->scope_stack_cap);
        if (!new_stack) return;
        sem->scope_stack = new_stack;
    }

    sem->scope_stack[sem->scope_stack_size++] = scope;
}

/*
   Retirar o escopo do topo da pilha semântica.
   Chamado ao sair de um bloco ou função.
*/
void sem_pop_scope(semantic_t *sem) {
    if (!sem || sem->scope_stack_size <= 1) return; /* nunca remover o global */
    sem->scope_stack_size--;
}

/* ============================================================================
   PESQUISA DE SÍMBOLOS
   ============================================================================ */

/*
   Procurar um símbolo na pilha de escopos semânticos, do mais interior
   para o global. Devolve symbol_info_t* ou NULL se não encontrado.

   Percorre a pilha semântica do topo para a base. Cada scope_t tem um
   parent_scope_id que permite subir na hierarquia até ao escopo global.
*/
symbol_info_t* sem_lookup(semantic_t *sem, const char *name) {
    if (!sem || !name) return NULL;

    /* Percorrer a pilha do topo (escopo mais interior) para a base */
    for (int i = sem->scope_stack_size - 1; i >= 0; i--) {
        if (!sem->scope_stack[i]) continue;
        void *info = scope_lookup_symbol(sem->scope_stack[i], name);
        if (info) return (symbol_info_t*)info;
    }

    /* Não encontrado na pilha — tentar todos os escopos registados
       (cobre o caso em que a pilha semântica não foi reconstruída
        completamente para o bloco actual) */
    parser_t *p = sem->parser;
    /* Pesquisa no escopo global como fallback final */
    void *info = scope_lookup_symbol(p->global_scope, name);
    if (info) return (symbol_info_t*)info;

    return NULL;
}

/*
   Verificar se um símbolo existe apenas no escopo actual (topo da pilha).
   Usado para detectar redeclarações no mesmo escopo.
*/
int sem_exists_current_scope(semantic_t *sem, const char *name) {
    if (!sem || !name || sem->scope_stack_size == 0) return 0;
    scope_t *current = sem->scope_stack[sem->scope_stack_size - 1];
    return scope_symbol_exists(current, name);
}

/* ============================================================================
   SISTEMA DE ERROS SEMÂNTICOS
   ============================================================================ */

/*
   Lê uma linha específica do ficheiro fonte para mostrar contexto no erro.
   (Replicado de parser_error.c — função local estática.)
*/
static char* ler_linha_fonte(const char *path, int target_line) {
    if (!path || target_line <= 0) return NULL;

    FILE *f = fopen(path, "r");
    if (!f) return NULL;

    char buffer[1024];
    int current = 1;

    while (fgets(buffer, sizeof(buffer), f)) {
        if (current == target_line) {
            size_t len = strlen(buffer);
            char *line = (char*)malloc(len + 1);
            if (line) memcpy(line, buffer, len + 1);
            fclose(f);
            return line;
        }
        current++;
    }
    fclose(f);
    return NULL;
}

/*
   Emitir mensagem de erro semântico com formatação GCC-style:

       ficheiro.c:12:5: semantic error: variável 'x' não declarada
          12 | int y = x + 1;
             |         ^

   A mensagem suporta argumentos variáveis (printf-style).
*/
void sem_error(semantic_t *sem, int line, int col, const char *fmt, ...) {
    if (!sem) return;

    sem->error_count++;

    const char *path = sem->source_path ? sem->source_path : "<desconhecido>";

    /* Cabeçalho do erro */
    fprintf(stderr, "%s%s:%d:%d:%s %ssemantic error:%s ",
            ANSI_BOLD, path, line, col, ANSI_RESET,
            ANSI_RED ANSI_BOLD, ANSI_RESET);

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");

    /* Contexto: linha do código-fonte com cursor */
    if (sem->source_path && line > 0) {
        char *src_line = ler_linha_fonte(sem->source_path, line);
        if (src_line) {
            /* Remover newline/CR final */
            size_t len = strlen(src_line);
            while (len > 0 && (src_line[len-1] == '\n' || src_line[len-1] == '\r')) {
                src_line[--len] = '\0';
            }

            /* Largura do gutter (número de linha) */
            int gutter = (line >= 1000) ? 5 : (line >= 100) ? 4 :
                         (line >= 10)   ? 3 : 2;
            if (gutter < 4) gutter = 4;

            fprintf(stderr, "%*d | %s\n", gutter, line, src_line);
            fprintf(stderr, "%*s | ", gutter, "");

            int c = (col > 0) ? col : 1;
            for (int i = 0; i < c - 1 && src_line[i] != '\0'; i++) {
                fputc(src_line[i] == '\t' ? '\t' : ' ', stderr);
            }
            fprintf(stderr, "%s^%s\n", ANSI_RED ANSI_BOLD, ANSI_RESET);
            free(src_line);
        }
    }
}

/* ============================================================================
   UTILITÁRIOS DE TIPO
   ============================================================================ */

/*
   Converter tipo KW_* para string legível (para mensagens de erro).
*/
const char* sem_tipo_para_string(int tipo, int is_pointer) {
    if (is_pointer > 0) {
        switch (tipo) {
            case KW_INT:    return "int*";
            case KW_CHAR:   return "char*";
            case KW_FLOAT:  return "float*";
            case KW_DOUBLE: return "double*";
            case KW_VOID:   return "void*";
            default:        return "ponteiro";
        }
    }
    switch (tipo) {
        case KW_INT:    return "int";
        case KW_CHAR:   return "char";
        case KW_FLOAT:  return "float";
        case KW_DOUBLE: return "double";
        case KW_VOID:   return "void";
        case KW_STRUCT: return "struct";
        case KW_UNION:  return "union";
        case TK_ID:     return "typedef";
        default:        return "desconhecido";
    }
}

/*
   Verificar se dois tipos são compatíveis para atribuição ou operação.

   Regras (subconjunto C):
   - Qualquer tipo numérico (int, char, float, double) é compatível com
     qualquer outro tipo numérico (promoção implícita).
   - void é incompatível com qualquer outro tipo.
   - Ponteiros são compatíveis entre si se o tipo base for igual, ou se um
     dos lados for void* (ponteiro genérico).
   - Strings (char*) são compatíveis com char* ou void*.
*/
int tipos_compativeis(int tipo_a, int is_ptr_a, int tipo_b, int is_ptr_b) {
    /* Void nunca é compatível com outro tipo em operações/atribuições */
    if ((tipo_a == KW_VOID && is_ptr_a == 0) ||
        (tipo_b == KW_VOID && is_ptr_b == 0)) {
        return 0;
    }

    /* Ambos são ponteiros */
    if (is_ptr_a > 0 && is_ptr_b > 0) {
        /* void* é compatível com qualquer ponteiro */
        if (tipo_a == KW_VOID || tipo_b == KW_VOID) return 1;
        return (tipo_a == tipo_b) ? 1 : 0;
    }

    /* Um ponteiro e um escalar — incompatível (excepto 0 literal,
       que não verificamos aqui porque precisaríamos do valor) */
    if (is_ptr_a != is_ptr_b) return 0;

    /* Ambos são tipos base escalares numéricos — compatíveis por promoção */
    int num_a = (tipo_a == KW_INT || tipo_a == KW_CHAR ||
                 tipo_a == KW_FLOAT || tipo_a == KW_DOUBLE);
    int num_b = (tipo_b == KW_INT || tipo_b == KW_CHAR ||
                 tipo_b == KW_FLOAT || tipo_b == KW_DOUBLE);
    if (num_a && num_b) return 1;

    /* Struct/union só é compatível consigo próprio (mesmo nome seria
       necessário, mas aqui simplificamos: se ambos forem struct/union,
       deixamos passar — o parser já valida a estrutura) */
    if (tipo_a == tipo_b) return 1;

    return 0;
}

/*
   Verificar se um tipo é válido como condição de estrutura de controlo.

   Em C, qualquer escalar (int, char, float, double, ponteiro) é válido.
   void (sem ponteiro) é o único tipo inválido.
*/
int tipo_valido_como_condicao(int tipo, int is_pointer) {
    if (is_pointer > 0) return 1; /* qualquer ponteiro é válido */
    return (tipo != KW_VOID && tipo != KW_STRUCT && tipo != KW_UNION);
}

/* ============================================================================
   VISITOR PRINCIPAL
   ============================================================================ */

/*
   Dispatcher principal: visitar um nó da AST e despachar para o verificador
   correcto com base no tipo do nó.

   Esta função é recursiva — chama verificar_declaracao, verificar_instrucao
   e verificar_expressao, que por sua vez a chamam de volta para os filhos.
*/
void verificar_no(semantic_t *sem, ast_node_t *node) {
    if (!node) return;

    switch (node->type) {
        /* Declarações */
        case NODE_VAR_DECL:
        case NODE_FUNC_DECL:
        case NODE_STRUCT_DECL:
        case NODE_TYPEDEF_DECL:
        case NODE_PARAM_DECL:
            verificar_declaracao(sem, node);
            break;

        /* Instruções */
        case NODE_BLOCK:
        case NODE_IF_STMT:
        case NODE_WHILE_STMT:
        case NODE_FOR_STMT:
        case NODE_DO_WHILE_STMT:
        case NODE_RETURN_STMT:
        case NODE_EXPR_STMT:
        case NODE_SWITCH_STMT:
        case NODE_BREAK_STMT:
        case NODE_CONTINUE_STMT:
        case NODE_GOTO_STMT:
        case NODE_LABEL_STMT:
            verificar_instrucao(sem, node);
            break;

        /* Expressões */
        case NODE_BINARY_OP:
        case NODE_UNARY_OP:
        case NODE_ASSIGN:
        case NODE_TERNARY:
        case NODE_CALL:
        case NODE_ARRAY_ACCESS:
        case NODE_MEMBER_ACCESS:
        case NODE_IDENTIFIER:
        case NODE_LITERAL:
        case NODE_INIT_LIST:
            verificar_expressao(sem, node);
            break;

        /* Nó raiz: visitar todos os filhos */
        case NODE_PROGRAM:
            for (int i = 0; i < node->child_count; i++) {
                verificar_no(sem, node->children[i]);
            }
            break;

        /* Directivas de pré-processador — sem verificação semântica */
        case NODE_PP_DIRECTIVE:
            break;

        case NODE_CASE:
        case NODE_DEFAULT:
            /* Visitar os filhos (instruções dentro do case) */
            for (int i = 0; i < node->child_count; i++) {
                verificar_no(sem, node->children[i]);
            }
            break;

        default:
            /* Nó desconhecido — visitar filhos por segurança */
            for (int i = 0; i < node->child_count; i++) {
                verificar_no(sem, node->children[i]);
            }
            break;
    }
}

/* ============================================================================
   PONTO DE ENTRADA
   ============================================================================ */

/*
   Executar a análise semântica completa.

   Percorre a AST a partir da raiz (NODE_PROGRAM) usando o visitor recursivo.
   Retorna o número de erros encontrados.
*/
int semantic_run(semantic_t *sem) {
    if (!sem || !sem->parser || !sem->parser->ast_root) return 0;

    verificar_no(sem, sem->parser->ast_root);
    return sem->error_count;
}

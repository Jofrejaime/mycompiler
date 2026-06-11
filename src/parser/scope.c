#define _POSIX_C_SOURCE 200809L
#include "scope.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ============================================================================
   SCOPE MANAGEMENT - IMPLEMENTAÇÃO
   ============================================================================ */

#define SCOPE_HASH_SIZE 64

/* ============================================================================
   FUNÇÕES AUXILIARES
   ============================================================================ */

static unsigned int hash_string(const char *str) {
    unsigned int hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    
    return hash % SCOPE_HASH_SIZE;
}

/* ============================================================================
   FUNÇÕES AUXILIARES - CÁLCULO DE TAMANHO
   ============================================================================ */

int get_base_type_size(int data_type, int is_pointer) {
    /* Ponteiros sempre ocupam 8 bytes */
    if (is_pointer > 0) {
        return 8;
    }
    
    /* Tamanhos de tipos base */
    switch (data_type) {
        case KW_CHAR:
            return 1;
        case KW_INT:
            return 4;
        case KW_FLOAT:
            return 4;
        case KW_DOUBLE:
            return 8;
        case KW_VOID:
            return 0;  /* void não tem tamanho */
        case KW_STRUCT:
        case KW_UNION:
            return 0;  /* Será calculado a partir dos campos */
        default:
            return 4;  /* Default: tamanho de int */
    }
}

int calculate_total_size(int data_type, int is_pointer, 
                        int *dimensions, int dim_count) {
    int base_size = get_base_type_size(data_type, is_pointer);
    
    if (base_size == 0) {
        return 0;  /* Tipo sem tamanho definido */
    }
    
    /* Multiplicar por todas as dimensões do array */
    int total_size = base_size;
    for (int i = 0; i < dim_count; i++) {
        if (dimensions[i] > 0) {
            total_size *= dimensions[i];
        } else {
            /* Array com dimensão indeterminada: usar apenas tamanho base */
            return base_size;
        }
    }
    
    return total_size;
}

/* ============================================================================
   FUNÇÕES DE GERENCIAMENTO DE ESCOPO
   ============================================================================ */

scope_t* scope_create(int scope_id, int parent_scope_id, int base_address) {
    scope_t *scope = (scope_t*)malloc(sizeof(scope_t));
    if (!scope) {
        return NULL;
    }
    
    scope->scope_id = scope_id;
    scope->parent_scope_id = parent_scope_id;
    scope->base_address = base_address;
    scope->current_offset = 0;
    
    /* Initialize symbol table */
    scope->symbol_capacity = SCOPE_HASH_SIZE;
    scope->symbols = (symbol_entry_t**)calloc(SCOPE_HASH_SIZE, sizeof(symbol_entry_t*));
    if (!scope->symbols) {
        free(scope);
        return NULL;
    }
    scope->symbol_count = 0;
    
    return scope;
}

void scope_free(scope_t *scope) {
    if (!scope) {
        return;
    }
    
    /* Free all symbol entries */
    for (int i = 0; i < SCOPE_HASH_SIZE; i++) {
        symbol_entry_t *entry = scope->symbols[i];
        while (entry) {
            symbol_entry_t *next = entry->next;
            free(entry->name);
            /* Note: entry->info is managed by parser, not freed here */
            free(entry);
            entry = next;
        }
    }
    
    free(scope->symbols);
    free(scope);
}

int scope_add_symbol(scope_t *scope, const char *name, void *info) {
    if (!scope || !name) {
        return 0;
    }
    
    /* Check if symbol already exists */
    if (scope_lookup_symbol(scope, name) != NULL) {
        return 0;  /* Symbol already exists */
    }
    
    /* Create new entry */
    symbol_entry_t *entry = (symbol_entry_t*)malloc(sizeof(symbol_entry_t));
    if (!entry) {
        return 0;
    }
    
    entry->name = strdup(name);
    entry->info = info;
    entry->next = NULL;
    
    /* Add to hash table */
    unsigned int hash = hash_string(name);
    entry->next = scope->symbols[hash];
    scope->symbols[hash] = entry;
    scope->symbol_count++;
    
    return 1;
}

int add_symbol_to_scope(scope_t *scope, const char *name) {
    if (!scope || !name) {
        return 0;
    }
    
    /* Check if symbol already exists by searching the hash table directly */
    unsigned int hash = hash_string(name);
    symbol_entry_t *existing = scope->symbols[hash];
    
    while (existing) {
        if (strcmp(existing->name, name) == 0) {
            return 0;  /* Symbol already exists */
        }
        existing = existing->next;
    }
    
    /* Create new entry with NULL info (to be enriched later) */
    symbol_entry_t *entry = (symbol_entry_t*)malloc(sizeof(symbol_entry_t));
    if (!entry) {
        return 0;
    }
    
    entry->name = strdup(name);
    if (!entry->name) {
        free(entry);
        return 0;
    }
    entry->info = NULL;  /* Will be set later via enrichment functions */
    entry->next = NULL;
    
    /* Add to hash table */
    entry->next = scope->symbols[hash];
    scope->symbols[hash] = entry;
    scope->symbol_count++;
    
    return 1;
}

void* scope_lookup_symbol(scope_t *scope, const char *name) {
    if (!scope || !name) {
        return NULL;
    }
    
    unsigned int hash = hash_string(name);
    symbol_entry_t *entry = scope->symbols[hash];
    
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            return entry->info;
        }
        entry = entry->next;
    }
    
    return NULL;
}

int scope_symbol_exists(scope_t *scope, const char *name) {
    if (!scope || !name) {
        return 0;
    }
    
    unsigned int hash = hash_string(name);
    symbol_entry_t *entry = scope->symbols[hash];
    
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            return 1;  /* Symbol exists */
        }
        entry = entry->next;
    }
    
    return 0;  /* Symbol not found */
}

int scope_allocate_memory(scope_t *scope, int size) {
    if (!scope) {
        return -1;
    }
    
    int address = scope->base_address + scope->current_offset;
    scope->current_offset += size;
    
    return address;
}

/* ============================================================================
   FUNÇÕES DE IMPRESSÃO (DEBUG)
   ============================================================================ */

const char* type_to_string(int type) {
    switch (type) {
        case KW_INT:
            return "int";
        case KW_CHAR:
            return "char";
        case KW_VOID:
            return "void";
        case KW_FLOAT:
            return "float";
        case KW_DOUBLE:
            return "double";
        case KW_STRUCT:
            return "struct";
        case KW_UNION:
            return "union";
        case TK_ID:
            return "typedef";
        default:
            return "unknown";
    }
}

static const char* kind_to_string(symbol_info_t *info) {
    if (!info) {
        return "unknown";
    }

    if (info->is_function || info->kind == SYMBOL_FUNCTION) {
        return "function";
    }
    if (info->is_typedef || info->kind == SYMBOL_TYPEDEF) {
        return "typedef";
    }

    switch (info->kind) {
        case SYMBOL_PARAMETER:
            return "parameter";
        case SYMBOL_STRUCT:
            return "struct";
        case SYMBOL_UNION:
            return "union";
        case SYMBOL_VARIABLE:
        default:
            return (info->variable_type == VAR_PARAMETER) ? "parameter" : "variable";
    }
}

static const char* scope_label(const scope_t *scope) {
    if (!scope) {
        return "unknown";
    }

    return (scope->scope_id == 0) ? "global" : "local";
}

void print_symbol_info(symbol_info_t *info) {
    if (!info) {
        printf("%-12s %-12s %-12s %-8s %-10s %-10s %-8s\n",
               "unknown", "unknown", "unknown", "-", "-", "-", "-");
        return;
    }

    char pointer_buf[16];
    if (info->is_pointer > 0) {
        snprintf(pointer_buf, sizeof(pointer_buf), "%d", info->is_pointer);
    } else {
        snprintf(pointer_buf, sizeof(pointer_buf), "-");
    }

    char array_buf[32];
    if (info->is_array && info->array_dim_count > 0) {
        snprintf(array_buf, sizeof(array_buf), "yes(%d)", info->array_dim_count);
    } else {
        snprintf(array_buf, sizeof(array_buf), "-");
    }

    char address_buf[16];
    if (info->memory_address >= 0) {
        snprintf(address_buf, sizeof(address_buf), "%d", info->memory_address);
    } else {
        snprintf(address_buf, sizeof(address_buf), "-");
    }

    char size_buf[16];
    if (info->size_bytes >= 0) {
        snprintf(size_buf, sizeof(size_buf), "%d", info->size_bytes);
    } else {
        snprintf(size_buf, sizeof(size_buf), "-");
    }

    char scope_buf[16];
    if (info->scope_id >= 0) {
        if (info->scope_id == 0) {
            snprintf(scope_buf, sizeof(scope_buf), "global");
        } else {
            snprintf(scope_buf, sizeof(scope_buf), "#%d", info->scope_id);
        }
    } else {
        snprintf(scope_buf, sizeof(scope_buf), "-");
    }

        printf("%-12s %-12s %-12s %-8s %-10s %-10s %-8s\n",
           type_to_string(info->data_type),
           kind_to_string(info),
           pointer_buf,
           array_buf,
           address_buf,
           size_buf,
            scope_buf);
}

void print_scope(scope_t *scope) {
    if (!scope) {
        return;
    }

    if (scope->scope_id == 0) {
        printf("================ GLOBAL SYMBOL TABLE ================\n\n");
    } else {
        printf("================ SCOPE #%d ===========================\n\n", scope->scope_id);
    }

        printf("%-16s %-12s %-12s %-8s %-10s %-10s %-8s %-8s\n",
            "Nome", "Tipo", "Categoria", "Ponteiro", "Array", "Endereco", "Tamanho", "Escopo");
        printf("--------------------------------------------------------------------------\n");

    for (int i = 0; i < SCOPE_HASH_SIZE; i++) {
        symbol_entry_t *entry = scope->symbols[i];
        while (entry) {
            printf("%-16s ", entry->name);
            print_symbol_info((symbol_info_t*)entry->info);
            entry = entry->next;
        }
    }

    printf("\nEscopo atual: %s #%d | Pai: %d | Base: %d | Offset: %d | Símbolos: %d\n\n",
           scope_label(scope),
           scope->scope_id,
           scope->parent_scope_id,
           scope->base_address,
           scope->current_offset,
           scope->symbol_count);
}

void print_all_scopes(parser_t *parser) {
    if (!parser) {
        return;
    }

    printf("\n================ SCOPE HIERARCHY =====================\n\n");
    for (int i = 0; i < parser->total_scopes; i++) {
        scope_t *scope = parser->all_scopes[i];
        if (!scope) {
            continue;
        }

        print_scope(scope);
    }
}

void print_scope_stack(scope_t **scope_stack, int stack_size) {
    printf("=== SCOPE STACK ===\n");
    printf("Stack size: %d\n\n", stack_size);
    
    for (int i = 0; i < stack_size; i++) {
        printf("Level %d:\n", i);
        print_scope(scope_stack[i]);
        printf("\n");
    }
}

#include "parser.h"
#include "../lexer/tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANSI_RED "\x1b[31m"
#define ANSI_RESET "\x1b[0m"

static int is_sync_token(int tipo) {
    return (tipo == SYM_SEMICOLON || tipo == SYM_RBRACE || tipo == TK_EOF);
}

static int is_stmt_start_token(int tipo) {
    switch (tipo) {
        case SYM_LBRACE:
        case SYM_SEMICOLON:
        case TK_ID:
        case KW_IF:
        case KW_WHILE:
        case KW_FOR:
        case KW_DO:
        case KW_SWITCH:
        case KW_RETURN:
        case KW_BREAK:
        case KW_CONTINUE:
        case KW_GOTO:
            return 1;
        default:
            return 0;
    }
}

static int is_decl_start_token(int tipo) {
    switch (tipo) {
        case KW_TYPEDEF:
        case KW_STRUCT:
        case KW_UNION:
        case KW_INT:
        case KW_CHAR:
        case KW_FLOAT:
        case KW_DOUBLE:
        case KW_VOID:
        case TK_ID: /* typedef-name (ou erro: ainda assim ajuda a re-sincronizar) */
            return 1;
        default:
            return 0;
    }
}

void synchronize(parser_t *parser) {
    if (!parser) {
        return;
    }

    int start_pos = parser->current_position;

    /* Fast paths for obvious boundaries */
    if (match(parser, TK_EOF) || match(parser, SYM_RBRACE)) {
        return;
    }
    if (match(parser, SYM_SEMICOLON)) {
        consume_token(parser);
        return;
    }

    /*
       Panic-mode: consume until we reach a boundary OR a plausible "restart point".

       Restart points are tokens that can begin a statement or declaration, so the
       caller can attempt to continue parsing from there.

       Important: guarantee progress (consume at least one token when possible),
       otherwise the parser can get stuck re-reporting the same error forever.
    */
    while (!is_sync_token(peek_token(parser).tipo)) {
        token_t t = peek_token(parser);
        if (is_stmt_start_token(t.tipo) || is_decl_start_token(t.tipo)) {
            break;
        }
        consume_token(parser);
    }

    /* If we didn't move at all and we are not at EOF, consume one token. */
    if (parser->current_position == start_pos && !match(parser, TK_EOF)) {
        consume_token(parser);
    }

    /* If we stopped at ';', consume it to move past the broken statement. */
    if (match(parser, SYM_SEMICOLON)) {
        consume_token(parser);
    }

    /* If we stopped at '}', we don't consume it here; the caller may expect it. */
}

static char *read_source_line(const char *path, int target_line) {
    if (!path || target_line <= 0) {
        return NULL;
    }

    FILE *file = fopen(path, "r");
    if (!file) {
        return NULL;
    }

    char buffer[1024];
    int current_line = 1;

    while (fgets(buffer, sizeof(buffer), file)) {
        if (current_line == target_line) {
            size_t len = strlen(buffer);
            char *line = (char*)malloc(len + 1);
            if (line) {
                memcpy(line, buffer, len + 1);
            }
            fclose(file);
            return line;
        }
        current_line++;
    }

    fclose(file);
    return NULL;
}

static void print_error_context(parser_t *parser, token_t found) {
    if (!parser || !parser->source_path) {
        return;
    }

    char *line = read_source_line(parser->source_path, found.linha);
    if (!line) {
        return;
    }

    int col = found.coluna;
    if (col < 1) {
        col = 1;
    }

    size_t line_len = strlen(line);
    while (line_len > 0 && (line[line_len - 1] == '\n' || line[line_len - 1] == '\r')) {
        line[--line_len] = '\0';
    }

    fprintf(stderr, "%4d | %s%s%s\n", found.linha, ANSI_RED, line, ANSI_RESET);
    fprintf(stderr, "     | %*s%s^%s\n", col - 1, "", ANSI_RED, ANSI_RESET);

    free(line);
}

void syntax_error(parser_t *parser, const char *message, int expected_type, token_t found) {
    if (!parser) {
        return;
    }

    parser->error_count++;
    parser->panic_mode = 1;

    const char *found_name = tipo_token_para_string(found.tipo);
    const char *expected_name = NULL;
    if (expected_type > 0) {
        expected_name = tipo_token_para_string(expected_type);
    }

    /* Format: file:line:col: error: message */
    fprintf(stderr, "\n");
    if (expected_name) {
        fprintf(stderr, "[%d:%d] erro sintatico: %s. Esperado '%s', encontrado '%s'\n",
                found.linha, found.coluna, message, expected_name, found_name);
    } else {
        fprintf(stderr, "[%d:%d] erro sintatico: %s. Encontrado '%s'\n",
                found.linha, found.coluna, message, found_name);
    }

    print_error_context(parser, found);
    fprintf(stderr, "\n");

    /* Basic panic-mode recovery: skip to statement boundary */
    panic_mode_recovery(parser);
}

void panic_mode_recovery(parser_t *parser) {
    if (!parser || !parser->panic_mode) {
        return;
    }

    synchronize(parser);

    parser->panic_mode = 0;
}

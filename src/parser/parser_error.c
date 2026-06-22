#include "parser.h"
#include "../lexer/tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANSI_RED     "\x1b[31m"
#define ANSI_BOLD    "\x1b[1m"
#define ANSI_RESET   "\x1b[0m"
#define ANSI_CYAN    "\x1b[36m"


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
            /* Note: TK_ID intentionally excluded — it is too broad and causes
               the synchronizer to stop on every identifier, generating cascades. */
            return 1;
        default:
            return 0;
    }
}

void synchronize(parser_t *parser) {
    if (!parser) {
        return;
    }

    /* Never consume past EOF or a closing brace — that belongs to the caller. */
    if (match(parser, TK_EOF) || match(parser, SYM_RBRACE)) {
        return;
    }

    /* Already sitting on ';': consume it and we're done. */
    if (match(parser, SYM_SEMICOLON)) {
        consume_token(parser);
        return;
    }

    /*
       Panic-mode recovery:
       Consume tokens until we hit a synchronisation point:
         - '}' or EOF  → stop WITHOUT consuming (caller needs it)
         - ';'         → consume it and stop
         - start of a new statement/declaration keyword → stop, let caller retry

       CRITICAL: always consume at least one token first so we cannot loop
       forever on the token that triggered the error.
    */
    consume_token(parser);  /* guaranteed progress */

    while (!match(parser, TK_EOF) && !match(parser, SYM_RBRACE)) {
        token_t t = peek_token(parser);

        if (match(parser, SYM_SEMICOLON)) {
            consume_token(parser);  /* consume ';' and land on next statement */
            return;
        }

        /* Stop before a keyword that starts a new statement or declaration */
        if (is_stmt_start_token(t.tipo) || is_decl_start_token(t.tipo)) {
            return;
        }

        consume_token(parser);
    }
    /* We stopped at '}' or EOF — leave it for the caller. */
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
    if (col < 1) col = 1;

    /* Strip trailing newline/carriage-return */
    size_t line_len = strlen(line);
    while (line_len > 0 && (line[line_len - 1] == '\n' || line[line_len - 1] == '\r')) {
        line[--line_len] = '\0';
    }

    /* Width of the line-number gutter (at least 4 chars wide) */
    int gutter = (found.linha >= 1000) ? 5 :
                 (found.linha >= 100)  ? 4 :
                 (found.linha >= 10)   ? 3 : 2;
    if (gutter < 4) gutter = 4;

    /* Print the source line: "  13 | <line>" */
    fprintf(stderr, "%*d | %s\n", gutter, found.linha, line);

    /* Print the caret line: "     |    ^"                          */
    /* Tabs in the source line must be replicated as spaces so that  */
    /* the caret lands on the right column.                          */
    fprintf(stderr, "%*s | ", gutter, "");
    for (int i = 0; i < col - 1 && line[i] != '\0'; i++) {
        fputc(line[i] == '\t' ? '\t' : ' ', stderr);
    }
    fprintf(stderr, "%s^%s\n", ANSI_RED ANSI_BOLD, ANSI_RESET);

    free(line);
}

void syntax_error(parser_t *parser, const char *message, int expected_type, token_t found) {
    if (!parser) {
        return;
    }

    parser->error_count++;

    /* Do not print cascading errors: if we are already in panic mode, just
       increment the counter and return — recovery is already in progress. */
    if (parser->panic_mode) {
        return;
    }
    parser->panic_mode = 1;

    const char *found_name    = tipo_token_para_string(found.tipo);
    const char *expected_name = (expected_type > 0) ? tipo_token_para_string(expected_type) : NULL;

    /* GCC-style header: file:line:col: error: message */
    const char *path = parser->source_path ? parser->source_path : "<unknown>";

    if (expected_name) {
        fprintf(stderr,
                "%s%s:%d:%d:%s %serror:%s %s; expected '%s' before '%s'\n",
                ANSI_BOLD, path, found.linha, found.coluna, ANSI_RESET,
                ANSI_RED ANSI_BOLD, ANSI_RESET,
                message, expected_name, found_name);
    } else {
        fprintf(stderr,
                "%s%s:%d:%d:%s %serror:%s %s; got '%s'\n",
                ANSI_BOLD, path, found.linha, found.coluna, ANSI_RESET,
                ANSI_RED ANSI_BOLD, ANSI_RESET,
                message, found_name);
    }

    print_error_context(parser, found);

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

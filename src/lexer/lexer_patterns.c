#include "lexer.h"
#include "keywords.h"
#include <string.h>

static int is_letter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static int is_digit(char c) {
    return c >= '0' && c <= '9';
}

static int is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}


token_t process_identifier(lexer_t *lexer, char first_char) {
    lexer->lexema[0] = first_char;
    int pos = 1;
    
    char c;
    while ((c = ler_caractere(lexer)) != '\0') {
        if (is_letter(c) || is_digit(c)) {
            if (pos < sizeof(lexer->lexema) - 1) {
                lexer->lexema[pos++] = c;
            }
        } else {
            volta_caractere(lexer, c);
            break;
        }
    }
    
    lexer->lexema[pos] = '\0';
    int type = procurar_palavra_chave(lexer->lexema);
    return gravar_token_lexema(lexer, type, lexer->lexema,
                               lexer->linha_inicio_lexema,
                               lexer->coluna_inicio_lexema);
}

token_t process_number(lexer_t *lexer, char first_char) {
    lexer->lexema[0] = first_char;
    int pos = 1;
    int has_dot = 0;
    int has_exp = 0;
    
    char c;
    while ((c = ler_caractere(lexer)) != '\0') {
        if (is_digit(c)) {
            if (pos < sizeof(lexer->lexema) - 1) {
                lexer->lexema[pos++] = c;
            }
        } else if (c == '.' && !has_dot && !has_exp) {
            has_dot = 1;
            if (pos < sizeof(lexer->lexema) - 1) {
                lexer->lexema[pos++] = c;
            }
        } else if ((c == 'e' || c == 'E') && !has_exp) {
            has_exp = 1;
            if (pos < sizeof(lexer->lexema) - 1) {
                lexer->lexema[pos++] = c;
            }
            
            c = ler_caractere(lexer);
            if (c == '+' || c == '-') {
                if (pos < sizeof(lexer->lexema) - 1) {
                    lexer->lexema[pos++] = c;
                }
            } else {
                volta_caractere(lexer, c);
            }
        } else {
            volta_caractere(lexer, c);
            break;
        }
    }
    
    lexer->lexema[pos] = '\0';
    int type = TK_NUM_INT;
    if (has_dot) type = TK_NUM_FLOAT;
    if (has_exp) type = TK_NUM_EXP;
    
    return gravar_token_lexema(lexer, type, lexer->lexema,
                               lexer->linha_inicio_lexema,
                               lexer->coluna_inicio_lexema);
}

token_t process_string(lexer_t *lexer) {
    int pos = 0;
    char c;
    
    while ((c = ler_caractere(lexer)) != '\0') {
        if (c == '"') {
            lexer->lexema[pos] = '\0';
            return gravar_token_lexema(lexer, TK_STRING, lexer->lexema,
                                       lexer->linha_inicio_lexema,
                                       lexer->coluna_inicio_lexema);
        }
        
        if (c == '\\') {
            if (pos < sizeof(lexer->lexema) - 1) {
                lexer->lexema[pos++] = c;
            }
            c = ler_caractere(lexer);
            if (c != '\0' && pos < sizeof(lexer->lexema) - 1) {
                lexer->lexema[pos++] = c;
            }
        } else {
            if (pos < sizeof(lexer->lexema) - 1) {
                lexer->lexema[pos++] = c;
            }
        }
    }
    
    lexer->lexema[pos] = '\0';
    token_t error;
    error.tipo = TK_ERROR;
    strcpy(error.lexeme, lexer->lexema);
    return error;
}

token_t process_char_literal(lexer_t *lexer) {
    int pos = 0;
    char c;
    
    while ((c = ler_caractere(lexer)) != '\0') {
        if (c == '\'') {
            lexer->lexema[pos] = '\0';
            return gravar_token_lexema(lexer, TK_CHAR, lexer->lexema,
                                       lexer->linha_inicio_lexema,
                                       lexer->coluna_inicio_lexema);
        }
        
        if (c == '\\') {
            if (pos < sizeof(lexer->lexema) - 1) {
                lexer->lexema[pos++] = c;
            }
            c = ler_caractere(lexer);
            if (c != '\0' && pos < sizeof(lexer->lexema) - 1) {
                lexer->lexema[pos++] = c;
            }
        } else {
            if (pos < sizeof(lexer->lexema) - 1) {
                lexer->lexema[pos++] = c;
            }
        }
    }
    
    lexer->lexema[pos] = '\0';
    token_t error;
    error.tipo = TK_ERROR;
    strcpy(error.lexeme, lexer->lexema);
    return error;
}

token_t process_operator(lexer_t *lexer, char first_char) {
    lexer->lexema[0] = first_char;
    char next = ler_caractere(lexer);
    
    if ((first_char == '+' && next == '+') || (first_char == '+' && next == '=') ||
        (first_char == '-' && next == '-') || (first_char == '-' && next == '=') ||
        (first_char == '-' && next == '>') ||
        (first_char == '*' && next == '=') || (first_char == '/' && next == '=') ||
        (first_char == '=' && next == '=') || (first_char == '!' && next == '=') ||
        (first_char == '<' && next == '=') || (first_char == '>' && next == '=') ||
        (first_char == '<' && next == '<') || (first_char == '>' && next == '>') ||
        (first_char == '&' && next == '&') || (first_char == '|' && next == '|')) {
        lexer->lexema[1] = next;
        lexer->lexema[2] = '\0';
    } else {
        lexer->lexema[1] = '\0';
        volta_caractere(lexer, next);
    }
    
    int type;
    if (strcmp(lexer->lexema, "+") == 0) type = OP_PLUS;
    else if (strcmp(lexer->lexema, "-") == 0) type = OP_MINUS;
    else if (strcmp(lexer->lexema, "*") == 0) type = OP_MULT;
    else if (strcmp(lexer->lexema, "/") == 0) type = OP_DIV;
    else if (strcmp(lexer->lexema, "%") == 0) type = OP_MOD;
    else if (strcmp(lexer->lexema, "=") == 0) type = OP_ASSIGN;
    else if (strcmp(lexer->lexema, "+=") == 0) type = OP_PLUS_ASSIGN;
    else if (strcmp(lexer->lexema, "-=") == 0) type = OP_MINUS_ASSIGN;
    else if (strcmp(lexer->lexema, "*=") == 0) type = OP_MULT_ASSIGN;
    else if (strcmp(lexer->lexema, "/=") == 0) type = OP_DIV_ASSIGN;
    else if (strcmp(lexer->lexema, "==") == 0) type = OP_EQ;
    else if (strcmp(lexer->lexema, "!=") == 0) type = OP_NE;
    else if (strcmp(lexer->lexema, "<") == 0) type = OP_LT;
    else if (strcmp(lexer->lexema, "<=") == 0) type = OP_LE;
    else if (strcmp(lexer->lexema, ">") == 0) type = OP_GT;
    else if (strcmp(lexer->lexema, ">=") == 0) type = OP_GE;
    else if (strcmp(lexer->lexema, "<<") == 0) type = OP_LSHIFT;
    else if (strcmp(lexer->lexema, ">>") == 0) type = OP_RSHIFT;
    else if (strcmp(lexer->lexema, "&&") == 0) type = OP_AND;
    else if (strcmp(lexer->lexema, "||") == 0) type = OP_OR;
    else if (strcmp(lexer->lexema, "!") == 0) type = OP_NOT;
    else if (strcmp(lexer->lexema, "&") == 0) type = OP_BITAND;
    else if (strcmp(lexer->lexema, "|") == 0) type = OP_BITOR;
    else if (strcmp(lexer->lexema, "^") == 0) type = OP_BITXOR;
    else if (strcmp(lexer->lexema, "~") == 0) type = OP_BITNOT;
    else if (strcmp(lexer->lexema, "++") == 0) type = OP_INC;
    else if (strcmp(lexer->lexema, "--") == 0) type = OP_DEC;
    else if (strcmp(lexer->lexema, "->") == 0) type = OP_ARROW;
    else type = TK_ERROR;
    
    return gravar_token_lexema(lexer, type, lexer->lexema,
                               lexer->linha_inicio_lexema,
                               lexer->coluna_inicio_lexema);
}

token_t process_symbol(lexer_t *lexer, char c) {
    lexer->lexema[0] = c;
    lexer->lexema[1] = '\0';
    
    int type;
    if (c == '(') type = SYM_LPAREN;
    else if (c == ')') type = SYM_RPAREN;
    else if (c == '{') type = SYM_LBRACE;
    else if (c == '}') type = SYM_RBRACE;
    else if (c == '[') type = SYM_LBRACKET;
    else if (c == ']') type = SYM_RBRACKET;
    else if (c == ';') type = SYM_SEMICOLON;
    else if (c == ',') type = SYM_COMMA;
    else if (c == '.') type = SYM_DOT;
    else if (c == ':') type = SYM_COLON;
    else if (c == '?') type = SYM_TERNARY;
    else type = TK_ERROR;
    
    return gravar_token_lexema(lexer, type, lexer->lexema,
                               lexer->linha_inicio_lexema,
                               lexer->coluna_inicio_lexema);
}

void skip_whitespace_and_comments(lexer_t *lexer, char *c) {
    while (1) {
        *c = ler_caractere(lexer);
        
        if (*c == '\0') return;
        
        if (is_whitespace(*c)) {
            continue;
        }
        
        if (*c == '/') {
            char next = ler_caractere(lexer);
            
            if (next == '/') {
                while ((*c = ler_caractere(lexer)) != '\n' && *c != '\0');
                continue;
            } else if (next == '*') {
                while ((*c = ler_caractere(lexer)) != '\0') {
                    if (*c == '*') {
                        char end = ler_caractere(lexer);
                        if (end == '/') break;
                        volta_caractere(lexer, end);
                    }
                }
                continue;
            } else {
                volta_caractere(lexer, next);
                return;
            }
        }
        
        return;
    }
}

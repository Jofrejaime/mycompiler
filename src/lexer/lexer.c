#include "lexer.h"
#include "keywords.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/* ============================================================================
   ANALISADOR LÉXICO - IMPLEMENTAÇÃO COMPLETA
   
   Implementa a máquina de estados (autômato finito determinístico)
   para reconhecer tokens conforme o arquivo CompiladorAutomatos.jff
   
   Funções Obrigatórias:
   1. analex()              - Analisar e tokenizar
   2. ler_caractere()       - Leitura com controle de linha/coluna
   3. volta_caractere()     - Retraçamento de caracteres
   4. gravar_token_lexema() - Salvar na tabela de símbolos
   
   ============================================================================ */

/* ============================================================================
   PREDICADOS (Funções de Teste)
   ============================================================================ */

static int eh_letra(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static int eh_digito(char c) {
    return c >= '0' && c <= '9';
}

static int eh_digito_hex(char c) {
    return eh_digito(c) || 
           (c >= 'a' && c <= 'f') || 
           (c >= 'A' && c <= 'F');
}

static int eh_espacobrancho(char c) {
    return c == ' ' || c == '\t' || c == '\r';
}

static int eh_operador_simples(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || 
           c == '%' || c == '=' || c == '<' || c == '>' || 
           c == '!' || c == '&' || c == '|' || c == '^' ||
           c == '~';
}

static int eh_simbolo_especial(char c) {
    return c == '(' || c == ')' || c == '{' || c == '}' ||
           c == '[' || c == ']' || c == ';' || c == ',' ||
           c == '.' || c == ':' || c == '?';
}

/* ============================================================================
   2. LER_CARACTERE() - LEITURA CARACTERE POR CARACTERE
   
   Função obrigatória conforme enunciado do professor
   Lê caracteres do buffer, mantendo linha/coluna
   ============================================================================ */
char ler_caractere(Lexer *lexer) {
    if (lexer == NULL) return '\0';
    
    /* Se houver retraçamento anterior, retornar caractere retratado */
    if (lexer->teve_retracao) {
        lexer->teve_retracao = 0;
        return lexer->caractere_retratado;
    }
    
    /* Verificar se precisa ler novo bloco do arquivo */
    if (lexer->pos_buffer >= lexer->tamanho_buffer) {
        lexer->tamanho_buffer = fread(lexer->buffer, 1, sizeof(lexer->buffer), 
                                       lexer->arquivo_entrada);
        if (lexer->tamanho_buffer == 0) {
            return '\0';  /* EOF */
        }
        lexer->pos_buffer = 0;
    }
    
    /* Ler próximo caractere do buffer */
    char c = lexer->buffer[lexer->pos_buffer++];
    
    /* Atualizar linha e coluna */
    if (c == '\n') {
        lexer->linha++;
        lexer->coluna = 1;
    } else if (c != '\r') {
        lexer->coluna++;
    }
    
    return c;
}

/* ============================================================================
   3. VOLTA_CARACTERE() - RETRAÇAMENTO (BACKTRACKING)
   
   Função obrigatória conforme enunciado do professor
   Usada quando a máquina de estados lê um caractere "outro"
   ============================================================================ */
void volta_caractere(Lexer *lexer, char c) {
    if (lexer == NULL) return;
    
    lexer->caractere_retratado = c;
    lexer->teve_retracao = 1;
    
    /* Ajustar linha/coluna */
    if (c == '\n') {
        lexer->linha--;
        /* Seria necessário calcular coluna anterior - simplificado aqui */
    } else {
        lexer->coluna--;
    }
}

/* ============================================================================
   4. GRAVAR_TOKEN_LEXEMA() - GUARDAR NA TABELA DE SÍMBOLOS
   
   Função obrigatória conforme enunciado do professor
   Cria Token e adiciona à TablaSimbolos
   ============================================================================ */
Token gravar_token_lexema(Lexer *lexer, int tipo, const char *lexema, 
                          int linha, int coluna) {
    Token t;
    t.tipo = tipo;
    t.linha = linha;
    t.coluna = coluna;
    
    strncpy(t.lexeme, lexema, sizeof(t.lexeme) - 1);
    t.lexeme[sizeof(t.lexeme) - 1] = '\0';
    
    /* Extrair valor numérico se aplicável */
    t.valor.valor_int = 0;
    t.valor.valor_float = 0.0;
    
    if (tipo == TK_NUM_INT) {
        t.valor.valor_int = atoi(lexema);
    } else if (tipo == TK_NUM_FLOAT || tipo == TK_NUM_EXP) {
        t.valor.valor_float = atof(lexema);
    }
    
    /* Adicionar à tabela de símbolos */
    if (lexer->tabla_simbolos != NULL) {
        adicionar_token(lexer->tabla_simbolos, t);
    }
    
    return t;
}

/* ============================================================================
   1. ANALEX() - ANALISADOR LÉXICO PRINCIPAL (MÁQUINA DE ESTADOS)
   
   Função obrigatória conforme enunciado do professor
   Implementa o autômato finito determinístico
   
   ALGORITMO DE MÁQUINA DE ESTADOS:
   1. q0 (inicial) → letra/_ → ID
   2. q0 → dígito → NUM
   3. q0 → " → STRING
   4. q0 → operador/símbolo → OP/SYM
   5. Quando atinge estado final, cria token
   6. Retrocede caractere "outro" com volta_caractere()
   ============================================================================ */
Token analex(Lexer *lexer) {
    if (lexer == NULL) {
        Token erro;
        erro.tipo = TK_ERROR;
        strcpy(erro.lexeme, "");
        return erro;
    }
    
    int estado = ESTADO_INICIAL;  /* q0 */
    char c;
    Token token_resultado;
    
    /* ========== PRIMEIRA PARTE: PULAR ESPAÇOS E COMENTÁRIOS ========== */
    
    while (1) {
        c = ler_caractere(lexer);
        
        if (c == '\0') {
            /* EOF */
            token_resultado.tipo = TK_EOF;
            token_resultado.lexeme[0] = '\0';
            token_resultado.linha = lexer->linha;
            token_resultado.coluna = lexer->coluna;
            return token_resultado;
        }
        
        /* Pular espaços e tabs */
        if (eh_espacobrancho(c)) {
            continue;
        }
        
        /* Pular newlines (mas não atualizar contadores - já feito em ler_caractere) */
        if (c == '\n') {
            continue;
        }
        
        /* Processar comentários C */
        if (c == '/') {
            char prox = ler_caractere(lexer);
            
            if (prox == '/') {
                /* Comentário de linha */
                while ((c = ler_caractere(lexer)) != '\n' && c != '\0') {
                    /* Pular até fim da linha */
                }
                continue;
            } else if (prox == '*') {
                /* Comentário em bloco */
                while ((c = ler_caractere(lexer)) != '\0') {
                    if (c == '*') {
                        char prox2 = ler_caractere(lexer);
                        if (prox2 == '/') {
                            break;
                        }
                        volta_caractere(lexer, prox2);
                    }
                }
                continue;
            } else {
                /* Não é comentário, retroceder e processar /  */
                volta_caractere(lexer, prox);
                break;
            }
        }
        
        /* Encontrou caractere não-espaço: processar */
        break;
    }
    
    /* ========== SEGUNDA PARTE: MÁQUINA DE ESTADOS ========== */
    
    lexer->linha_inicio_lexema = lexer->linha;
    lexer->coluna_inicio_lexema = lexer->coluna;
    lexer->pos_lexema = 0;
    memset(lexer->lexema, 0, sizeof(lexer->lexema));
    
    /* Adicionar primeiro caractere ao lexema */
    lexer->lexema[lexer->pos_lexema++] = c;
    
    /* RECONHECER IDENTIFICADORES OU PALAVRAS-CHAVE */
    if (eh_letra(c)) {
        /* Estado q1-qX: identificador */
        while ((c = ler_caractere(lexer)) != '\0') {
            if (eh_letra(c) || eh_digito(c)) {
                if (lexer->pos_lexema < sizeof(lexer->lexema) - 1) {
                    lexer->lexema[lexer->pos_lexema++] = c;
                }
            } else {
                /* Fim do identificador - caractere "outro" */
                volta_caractere(lexer, c);
                break;
            }
        }
        
        lexer->lexema[lexer->pos_lexema] = '\0';
        
        /* Consultar tabela de palavras-chave */
        int tipo = procurar_palavra_chave(lexer->lexema);
        return gravar_token_lexema(lexer, tipo, lexer->lexema, 
                                   lexer->linha_inicio_lexema, 
                                   lexer->coluna_inicio_lexema);
    }
    
    /* RECONHECER NÚMEROS */
    if (eh_digito(c)) {
        int tem_ponto = 0;
        int tem_expo = 0;
        
        while ((c = ler_caractere(lexer)) != '\0') {
            if (eh_digito(c)) {
                if (lexer->pos_lexema < sizeof(lexer->lexema) - 1) {
                    lexer->lexema[lexer->pos_lexema++] = c;
                }
            } else if (c == '.' && !tem_ponto && !tem_expo) {
                /* Número float */
                tem_ponto = 1;
                if (lexer->pos_lexema < sizeof(lexer->lexema) - 1) {
                    lexer->lexema[lexer->pos_lexema++] = c;
                }
            } else if ((c == 'e' || c == 'E') && !tem_expo) {
                /* Notação científica */
                tem_expo = 1;
                if (lexer->pos_lexema < sizeof(lexer->lexema) - 1) {
                    lexer->lexema[lexer->pos_lexema++] = c;
                }
                
                /* Próximo pode ser + ou - */
                c = ler_caractere(lexer);
                if (c == '+' || c == '-') {
                    if (lexer->pos_lexema < sizeof(lexer->lexema) - 1) {
                        lexer->lexema[lexer->pos_lexema++] = c;
                    }
                } else {
                    volta_caractere(lexer, c);
                }
            } else {
                /* Fim do número */
                volta_caractere(lexer, c);
                break;
            }
        }
        
        lexer->lexema[lexer->pos_lexema] = '\0';
        
        /* Determinar tipo de número */
        int tipo = TK_NUM_INT;
        if (tem_ponto) tipo = TK_NUM_FLOAT;
        if (tem_expo) tipo = TK_NUM_EXP;
        
        return gravar_token_lexema(lexer, tipo, lexer->lexema,
                                   lexer->linha_inicio_lexema,
                                   lexer->coluna_inicio_lexema);
    }
    
    /* RECONHECER STRINGS */
    if (c == '"') {
        while ((c = ler_caractere(lexer)) != '\0') {
            if (c == '"') {
                /* Fim da string - não incluir aspas no lexema */
                break;
            } else if (c == '\\') {
                /* Escape sequence */
                if (lexer->pos_lexema < sizeof(lexer->lexema) - 1) {
                    lexer->lexema[lexer->pos_lexema++] = c;
                }
                c = ler_caractere(lexer);
                if (c != '\0' && lexer->pos_lexema < sizeof(lexer->lexema) - 1) {
                    lexer->lexema[lexer->pos_lexema++] = c;
                }
            } else {
                if (lexer->pos_lexema < sizeof(lexer->lexema) - 1) {
                    lexer->lexema[lexer->pos_lexema++] = c;
                }
            }
        }
        
        lexer->lexema[lexer->pos_lexema] = '\0';
        return gravar_token_lexema(lexer, TK_STRING, lexer->lexema,
                                   lexer->linha_inicio_lexema,
                                   lexer->coluna_inicio_lexema);
    }
    
    /* RECONHECER CHARACTER LITERALS */
    if (c == '\'') {
        while ((c = ler_caractere(lexer)) != '\0') {
            if (c == '\'') {
                break;
            } else if (c == '\\') {
                if (lexer->pos_lexema < sizeof(lexer->lexema) - 1) {
                    lexer->lexema[lexer->pos_lexema++] = c;
                }
                c = ler_caractere(lexer);
                if (c != '\0' && lexer->pos_lexema < sizeof(lexer->lexema) - 1) {
                    lexer->lexema[lexer->pos_lexema++] = c;
                }
            } else {
                if (lexer->pos_lexema < sizeof(lexer->lexema) - 1) {
                    lexer->lexema[lexer->pos_lexema++] = c;
                }
            }
        }
        
        lexer->lexema[lexer->pos_lexema] = '\0';
        return gravar_token_lexema(lexer, TK_CHAR, lexer->lexema,
                                   lexer->linha_inicio_lexema,
                                   lexer->coluna_inicio_lexema);
    }
    
    /* RECONHECER OPERADORES (com lookahead para operadores compostos) */
    if (eh_operador_simples(c)) {
        char prox = ler_caractere(lexer);
        
        /* Operadores compostos */
        if ((c == '+' && prox == '+') || (c == '+' && prox == '=') ||
            (c == '-' && prox == '-') || (c == '-' && prox == '=') ||
            (c == '*' && prox == '=') || (c == '/' && prox == '=') ||
            (c == '=' && prox == '=') || (c == '!' && prox == '=') ||
            (c == '<' && prox == '=') || (c == '>' && prox == '=') ||
            (c == '<' && prox == '<') || (c == '>' && prox == '>') ||
            (c == '&' && prox == '&') || (c == '|' && prox == '|') ||
            (c == '-' && prox == '>')) {
            
            lexer->lexema[1] = prox;
            lexer->lexema[2] = '\0';
        } else {
            /* Operador simples - retroceder caractere extra */
            lexer->lexema[1] = '\0';
            volta_caractere(lexer, prox);
        }
        
        /* Determinar tipo de token para operador */
        int tipo;
        if (strcmp(lexer->lexema, "+") == 0) tipo = OP_PLUS;
        else if (strcmp(lexer->lexema, "-") == 0) tipo = OP_MINUS;
        else if (strcmp(lexer->lexema, "*") == 0) tipo = OP_MULT;
        else if (strcmp(lexer->lexema, "/") == 0) tipo = OP_DIV;
        else if (strcmp(lexer->lexema, "%") == 0) tipo = OP_MOD;
        else if (strcmp(lexer->lexema, "=") == 0) tipo = OP_ASSIGN;
        else if (strcmp(lexer->lexema, "+=") == 0) tipo = OP_PLUS_ASSIGN;
        else if (strcmp(lexer->lexema, "-=") == 0) tipo = OP_MINUS_ASSIGN;
        else if (strcmp(lexer->lexema, "*=") == 0) tipo = OP_MULT_ASSIGN;
        else if (strcmp(lexer->lexema, "/=") == 0) tipo = OP_DIV_ASSIGN;
        else if (strcmp(lexer->lexema, "==") == 0) tipo = OP_EQ;
        else if (strcmp(lexer->lexema, "!=") == 0) tipo = OP_NE;
        else if (strcmp(lexer->lexema, "<") == 0) tipo = OP_LT;
        else if (strcmp(lexer->lexema, "<=") == 0) tipo = OP_LE;
        else if (strcmp(lexer->lexema, ">") == 0) tipo = OP_GT;
        else if (strcmp(lexer->lexema, ">=") == 0) tipo = OP_GE;
        else if (strcmp(lexer->lexema, "<<") == 0) tipo = OP_LSHIFT;
        else if (strcmp(lexer->lexema, ">>") == 0) tipo = OP_RSHIFT;
        else if (strcmp(lexer->lexema, "&&") == 0) tipo = OP_AND;
        else if (strcmp(lexer->lexema, "||") == 0) tipo = OP_OR;
        else if (strcmp(lexer->lexema, "!") == 0) tipo = OP_NOT;
        else if (strcmp(lexer->lexema, "&") == 0) tipo = OP_BITAND;
        else if (strcmp(lexer->lexema, "|") == 0) tipo = OP_BITOR;
        else if (strcmp(lexer->lexema, "^") == 0) tipo = OP_BITXOR;
        else if (strcmp(lexer->lexema, "~") == 0) tipo = OP_BITNOT;
        else if (strcmp(lexer->lexema, "++") == 0) tipo = OP_INC;
        else if (strcmp(lexer->lexema, "--") == 0) tipo = OP_DEC;
        else if (strcmp(lexer->lexema, "->") == 0) tipo = OP_ARROW;
        else tipo = TK_ERROR;
        
        return gravar_token_lexema(lexer, tipo, lexer->lexema,
                                   lexer->linha_inicio_lexema,
                                   lexer->coluna_inicio_lexema);
    }
    
    /* RECONHECER SÍMBOLOS ESPECIAIS */
    if (eh_simbolo_especial(c)) {
        lexer->lexema[1] = '\0';
        
        int tipo;
        if (c == '(') tipo = SYM_LPAREN;
        else if (c == ')') tipo = SYM_RPAREN;
        else if (c == '{') tipo = SYM_LBRACE;
        else if (c == '}') tipo = SYM_RBRACE;
        else if (c == '[') tipo = SYM_LBRACKET;
        else if (c == ']') tipo = SYM_RBRACKET;
        else if (c == ';') tipo = SYM_SEMICOLON;
        else if (c == ',') tipo = SYM_COMMA;
        else if (c == '.') tipo = SYM_DOT;
        else if (c == ':') tipo = SYM_COLON;
        else if (c == '?') tipo = SYM_TERNARY;
        else tipo = TK_ERROR;
        
        return gravar_token_lexema(lexer, tipo, lexer->lexema,
                                   lexer->linha_inicio_lexema,
                                   lexer->coluna_inicio_lexema);
    }
    
    /* ERRO LÉXICO: Carácter não reconhecido */
    lexer->lexema[1] = '\0';
    Token t;
    t.tipo = TK_ERROR;
    strcpy(t.lexeme, lexer->lexema);
    t.linha = lexer->linha_inicio_lexema;
    t.coluna = lexer->coluna_inicio_lexema;
    
    return t;
}

/* ============================================================================
   FUNÇÕES DE INICIALIZAÇÃO E LIMPEZA
   ============================================================================ */

Lexer* criar_lexer(const char *nome_arquivo) {
    if (nome_arquivo == NULL) {
        return NULL;
    }
    
    Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
    if (lexer == NULL) {
        return NULL;
    }
    
    lexer->arquivo_entrada = fopen(nome_arquivo, "r");
    if (lexer->arquivo_entrada == NULL) {
        free(lexer);
        return NULL;
    }
    
    lexer->pos_buffer = 0;
    lexer->tamanho_buffer = 0;
    lexer->linha = 1;
    lexer->coluna = 1;
    lexer->linha_inicio_lexema = 1;
    lexer->coluna_inicio_lexema = 1;
    lexer->pos_lexema = 0;
    lexer->estado_atual = ESTADO_INICIAL;
    lexer->teve_retracao = 0;
    lexer->caractere_retratado = '\0';
    
    memset(lexer->lexema, 0, sizeof(lexer->lexema));
    memset(lexer->buffer, 0, sizeof(lexer->buffer));
    
    /* Criar tabela de símbolos com capacidade inicial de 100 tokens */
    lexer->tabla_simbolos = criar_tabla_simbolos(100);
    
    return lexer;
}

void liberar_lexer(Lexer *lexer) {
    if (lexer == NULL) {
        return;
    }
    
    if (lexer->arquivo_entrada != NULL) {
        fclose(lexer->arquivo_entrada);
    }
    
    if (lexer->tabla_simbolos != NULL) {
        liberar_tabla_simbolos(lexer->tabla_simbolos);
    }
    
    free(lexer);
}

TablaSimbolos* processar_arquivo_completo(const char *nome_arquivo) {
    Lexer *lexer = criar_lexer(nome_arquivo);
    if (lexer == NULL) {
        return NULL;
    }
    
    Token token;
    do {
        token = analex(lexer);
    } while (token.tipo != TK_EOF);
    
    TablaSimbolos *resultado = lexer->tabla_simbolos;
    
    /* Liberar lexer mas preservar tabela de símbolos */
    if (lexer->arquivo_entrada != NULL) {
        fclose(lexer->arquivo_entrada);
        lexer->arquivo_entrada = NULL;
    }
    lexer->tabla_simbolos = NULL;
    
    liberar_lexer(lexer);
    return resultado;
}

void debug_lexer(Lexer *lexer) {
    if (lexer == NULL) return;
    
    printf("=== DEBUG LEXER ===\n");
    printf("Linha: %d, Coluna: %d\n", lexer->linha, lexer->coluna);
    printf("Lexema: '%s'\n", lexer->lexema);
    printf("Estado: %d\n", lexer->estado_atual);
    printf("Teve retraçamento: %d\n", lexer->teve_retracao);
}

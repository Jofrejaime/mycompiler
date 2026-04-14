# 🏗️ ARQUITETURA DO COMPILADOR C→C

## Estrutura Completa do Projeto

```
mycompiler/
│
├── 📄 README.md                           ← Documentação do projeto
│
├── 📁 automato/
│   ├── CompiladorAutomatos.jff            ← Autômatos (seu arquivo)
│   └── README_AUTOMATO.md
│
├── 📁 src/
│   │
│   ├── 📁 lexer/                          ← FASE 1: Análise Léxica
│   │   ├── lexer.c                        ← Implementação
│   │   ├── lexer.h                        ← Interface pública
│   │   ├── tokens.h                       ← Definição de tipos
│   │   ├── keywords.c                     ← Tabela de palavras-chave
│   │   ├── keywords.h
│   │   └── error.c, error.h               ← Tratamento de erros
│   │
│   ├── 📁 parser/                         ← FASE 2: Análise Sintática
│   │   ├── parser.c
│   │   ├── parser.h
│   │   ├── ast.c, ast.h                   ← Árvore Sintática
│   │   └── grammar.txt                    ← Gramática da língua
│   │
│   ├── 📁 semantic/                       ← FASE 3: Análise Semântica
│   │   ├── semantic.c
│   │   ├── semantic.h
│   │   ├── symtable.c, symtable.h         ← Tabela de símbolos
│   │   ├── type_checker.c, type_checker.h ← Verificação de tipos
│   │   └── scope.c, scope.h               ← Gerenciamento de escopo
│   │
│   ├── 📁 codegen/                        ← FASE 4: Geração de Código
│   │   ├── codegen_tac.c                  ← Gera TAC
│   │   ├── codegen_tac.h
│   │   ├── tac.c, tac.h                   ← Representação TAC
│   │   └── optimizer.c, optimizer.h       ← FASE 5: Otimização (opcional)
│   │
│   ├── 📁 backend/                        ← FASE 6: Geração C
│   │   ├── codegen_c.c                    ← TAC → C
│   │   ├── codegen_c.h
│   │   └── code_writer.c, code_writer.h   ← Escrever arquivo .c
│   │
│   ├── 📁 main/
│   │   ├── main.c                         ← Ponto de entrada
│   │   ├── compiler.c, compiler.c                ← Orquestra as fases
│   │   └── compiler.h
│   │
│   └── 📁 utils/                          ← Utilitários
│       ├── memory.c, memory.h             ← Alocação dinâmica
│       ├── string_utils.c, string_utils.h
│       ├── list.c, list.h                 ← Estruturas de dados
│       ├── queue.c, queue.h
│       ├── hash_table.c, hash_table.h
│       └── debug.c, debug.h               ← Debug/logging
│
├── 📁 tests/
│   ├── test_lexer.c                       ← Testes unitários
│   ├── test_parser.c
│   ├── test_semantic.c
│   │
│   ├── 📁 input_samples/                  ← Programas C para testar
│   │   ├── simple.c
│   │   ├── loop.c
│   │   ├── function.c
│   │   └── complex.c
│   │
│   └── 📁 expected_output/                ← Saída esperada dos testes
│       ├── simple_output.c
│       └── ...
│
├── 📁 output/                             ← Código gerado
│   ├── generated_code.c                   ← Código C gerado
│   ├── generated_tac.txt                  ← TAC intermediária
│   └── error_log.txt
│
├── 📁 docs/                               ← Documentação técnica
│   ├── PLANO_COMPILADOR_COMPLETO.md      ← (já criado)
│   ├── AS_7_FASES_COMPILACAO.md          ← (já criado)
│   ├── ANALISE_LEXICA.md                 ← Fase 1 detalhado
│   ├── ANALISE_SINTATICA.md              ← Fase 2 detalhado
│   ├── ANALISE_SEMANTICA.md              ← Fase 3 detalhado
│   └── ...
│
├── Makefile                               ← Build system
├── CMakeLists.txt                         ← Build system alternativo
├── .gitignore
└── LICENSE
```

---

## 📋 ARQUIVOS PRINCIPAIS E SUAS RESPONSABILIDADES

### **1. lexer/lexer.h** (Interface Pública do Lexer)

```c
#ifndef LEXER_H
#define LEXER_H

#include "tokens.h"

typedef struct {
    Token *tokens;
    int tamanho;
    int capacidade;
    int posicao_atual;  // Para o parser iterar
} TokenStream;

// Funções públicas
TokenStream* lexer_analisar(const char *codigo_fonte);
Token* lexer_proximo_token(TokenStream *ts);
Token* lexer_token_atual(TokenStream *ts);
void lexer_resetar(TokenStream *ts);
void lexer_liberar(TokenStream *ts);

// Erro
void lexer_reportar_erro(const char *msg, int linha, int coluna);

#endif
```

### **2. tokens.h** (Definição de Tipos de Token)

```c
#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
    // Palavras-chave
    KW_INT, KW_CHAR, KW_VOID, KW_FLOAT, KW_DOUBLE,
    KW_RETURN, KW_IF, KW_ELSE, KW_WHILE, KW_FOR,
    KW_DO, KW_BREAK, KW_CONTINUE, KW_SWITCH, KW_CASE,
    KW_DEFAULT, KW_STRUCT, KW_UNION, KW_TYPEDEF,
    KW_SIZEOF, KW_STATIC, KW_EXTERN, KW_CONST,
    KW_VOLATILE, KW_RESTRICT, KW_INLINE, KW_REGISTER,
    
    // Tipos de token
    ID,              // Identificador
    NUM_INT,         // 123
    NUM_FLOAT,       // 3.14
    NUM_EXP,         // 1e10
    STRING,          // "hello"
    CHAR_LIT,        // 'a'
    
    // Operadores
    OP_PLUS, OP_MINUS, OP_MULT, OP_DIV, OP_MOD,
    OP_ASSIGN, OP_PLUS_ASSIGN, OP_MINUS_ASSIGN,
    OP_MULT_ASSIGN, OP_DIV_ASSIGN, OP_MOD_ASSIGN,
    OP_EQ, OP_NE, OP_LT, OP_LE, OP_GT, OP_GE,
    OP_LSHIFT, OP_RSHIFT, OP_AND, OP_OR, OP_NOT,
    OP_BITAND, OP_BITOR, OP_BITXOR, OP_BITNOT,
    OP_INC, OP_DEC, OP_ARROW,
    
    // Símbolos
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    SEMICOLON, COMMA, DOT, COLON, TERNARY,
    
    // Especiais
    EOF_TOKEN,
    ERROR_TOKEN
} TokenType;

typedef struct {
    TokenType tipo;
    char *lexeme;
    int linha;
    int coluna;
    union {
        int valor_int;
        double valor_float;
        char *valor_string;
    } valor;
} Token;

#endif
```

### **3. keywords.h/keywords.c** (Tabela de Palavras-Chave)

```c
// keywords.h
#ifndef KEYWORDS_H
#define KEYWORDS_H

#include "tokens.h"

TokenType procurar_palavra_chave(const char *palavra);
void inicializar_tabela_palavras_chave(void);

#endif

// keywords.c (resumido)
#include "keywords.h"
#include <string.h>

struct {
    const char *palavra;
    TokenType tipo;
} tabela_palavras_chave[] = {
    {"int", KW_INT},
    {"char", KW_CHAR},
    {"void", KW_VOID},
    {"return", KW_RETURN},
    {"if", KW_IF},
    {"else", KW_ELSE},
    {"while", KW_WHILE},
    {"for", KW_FOR},
    // ... mais 30 palavras-chave
    {NULL, 0}
};

TokenType procurar_palavra_chave(const char *palavra) {
    for (int i = 0; tabela_palavras_chave[i].palavra != NULL; i++) {
        if (strcmp(tabela_palavras_chave[i].palavra, palavra) == 0) {
            return tabela_palavras_chave[i].tipo;
        }
    }
    return ID;  // Não é palavra-chave
}
```

### **4. lexer.c** (Implementação do Lexer - Esquemático)

```c
#include "lexer.h"
#include "keywords.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

struct Lexer {
    const char *entrada;
    int posicao;
    int linha;
    int coluna;
    int tamanho_entrada;
};

// Funções auxiliares
static char caractere_atual(Lexer *lex) {
    if (lex->posicao >= lex->tamanho_entrada) {
        return '\0';
    }
    return lex->entrada[lex->posicao];
}

static char proximo_caractere(Lexer *lex) {
    if (lex->posicao + 1 >= lex->tamanho_entrada) {
        return '\0';
    }
    return lex->entrada[lex->posicao + 1];
}

static void avancar(Lexer *lex) {
    if (lex->entrada[lex->posicao] == '\n') {
        lex->linha++;
        lex->coluna = 1;
    } else {
        lex->coluna++;
    }
    lex->posicao++;
}

static void pular_espacos_em_branco(Lexer *lex) {
    while (isspace(caractere_atual(lex))) {
        avancar(lex);
    }
}

static Token* reconhecer_numero(Lexer *lex) {
    Token *t = malloc(sizeof(Token));
    char buffer[256] = {0};
    int idx = 0;
    
    // Acumular dígitos
    while (isdigit(caractere_atual(lex))) {
        buffer[idx++] = caractere_atual(lex);
        avancar(lex);
    }
    
    // Verificar ponto decimal
    if (caractere_atual(lex) == '.' && isdigit(proximo_caractere(lex))) {
        buffer[idx++] = '.';
        avancar(lex);
        while (isdigit(caractere_atual(lex))) {
            buffer[idx++] = caractere_atual(lex);
            avancar(lex);
        }
        t->tipo = NUM_FLOAT;
        t->valor.valor_float = atof(buffer);
    } else {
        t->tipo = NUM_INT;
        t->valor.valor_int = atoi(buffer);
    }
    
    t->lexeme = strdup(buffer);
    return t;
}

static Token* reconhecer_id_ou_palavra_chave(Lexer *lex) {
    Token *t = malloc(sizeof(Token));
    char buffer[256] = {0};
    int idx = 0;
    
    // Acumular caracteres alfanuméricos
    while (isalnum(caractere_atual(lex)) || caractere_atual(lex) == '_') {
        buffer[idx++] = caractere_atual(lex);
        avancar(lex);
    }
    
    buffer[idx] = '\0';
    t->lexeme = strdup(buffer);
    t->tipo = procurar_palavra_chave(buffer);
    t->valor.valor_string = t->lexeme;
    
    return t;
}

// ... mais funções para reconhecer operadores, strings, etc.

TokenStream* lexer_analisar(const char *codigo_fonte) {
    Lexer lex = {
        .entrada = codigo_fonte,
        .posicao = 0,
        .linha = 1,
        .coluna = 1,
        .tamanho_entrada = strlen(codigo_fonte)
    };
    
    TokenStream *ts = malloc(sizeof(TokenStream));
    ts->tokens = malloc(1000 * sizeof(Token));  // Pré-alocar
    ts->tamanho = 0;
    ts->capacidade = 1000;
    ts->posicao_atual = 0;
    
    while (lex.posicao < lex.tamanho_entrada) {
        pular_espacos_em_branco(&lex);
        
        char ch = caractere_atual(&lex);
        
        // Reconhecer tokens...
        if (isdigit(ch)) {
            ts->tokens[ts->tamanho++] = *reconhecer_numero(&lex);
        }
        else if (isalpha(ch) || ch == '_') {
            ts->tokens[ts->tamanho++] = *reconhecer_id_ou_palavra_chave(&lex);
        }
        // ... etc
    }
    
    // Adicionar EOF
    Token eof_token = {.tipo = EOF_TOKEN, .lexeme = ""};
    ts->tokens[ts->tamanho++] = eof_token;
    
    return ts;
}
```

---

## 🔄 FLUXO DE COMPILAÇÃO (main.c)

```c
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen_tac.h"
#include "codegen_c.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: compilador <arquivo.c>\n");
        return 1;
    }
    
    const char *arquivo_entrada = argv[1];
    
    // Ler arquivo
    FILE *f = fopen(arquivo_entrada, "r");
    if (!f) {
        fprintf(stderr, "Erro: não pode abrir %s\n", arquivo_entrada);
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    int tamanho = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *codigo = malloc(tamanho + 1);
    fread(codigo, 1, tamanho, f);
    codigo[tamanho] = '\0';
    fclose(f);
    
    // FASE 1: Análise Léxica
    printf("=== FASE 1: Análise Léxica ===\n");
    TokenStream *tokens = lexer_analisar(codigo);
    if (!tokens) {
        fprintf(stderr, "Erro na análise léxica\n");
        return 1;
    }
    printf("✓ %d tokens gerados\n\n", tokens->tamanho);
    
    // FASE 2: Análise Sintática
    printf("=== FASE 2: Análise Sintática ===\n");
    AST *ast = parser_analisar(tokens);
    if (!ast) {
        fprintf(stderr, "Erro na análise sintática\n");
        return 1;
    }
    printf("✓ AST gerada\n\n");
    
    // FASE 3: Análise Semântica
    printf("=== FASE 3: Análise Semântica ===\n");
    if (!semantic_analisar(ast)) {
        fprintf(stderr, "Erro na análise semântica\n");
        return 1;
    }
    printf("✓ Análise semântica concluída\n\n");
    
    // FASE 4: Geração de Código Intermediário
    printf("=== FASE 4: Geração de Código Intermediário ===\n");
    TAC *tac = codegen_tac_gerar(ast);
    if (!tac) {
        fprintf(stderr, "Erro na geração TAC\n");
        return 1;
    }
    printf("✓ TAC gerada com %d instruções\n\n", tac->num_instrucoes);
    
    // FASE 5: Otimização (opcional)
    printf("=== FASE 5: Otimização (Opcional) ===\n");
    // optimizer_otimizar(tac);
    printf("✓ Otimização pulada (primeira versão)\n\n");
    
    // FASE 6: Geração de Código C
    printf("=== FASE 6: Geração de Código C ===\n");
    char *codigo_c_gerado = codegen_c_gerar(tac);
    if (!codigo_c_gerado) {
        fprintf(stderr, "Erro na geração de código C\n");
        return 1;
    }
    printf("✓ Código C gerado\n\n");
    
    // Salvar arquivo gerado
    FILE *saida = fopen("output/generated.c", "w");
    if (saida) {
        fprintf(saida, "%s", codigo_c_gerado);
        fclose(saida);
        printf("✓ Salvo em output/generated.c\n");
    }
    
    // FASE 7: Compilação com gcc
    printf("=== FASE 7: Compilação ===\n");
    int ret = system("gcc output/generated.c -o output/executable");
    if (ret == 0) {
        printf("✓ Executável gerado: output/executable\n");
    } else {
        fprintf(stderr, "Erro ao compilar com gcc\n");
        return 1;
    }
    
    // Cleanup
    free(codigo);
    lexer_liberar(tokens);
    ast_liberar(ast);
    tac_liberar(tac);
    free(codigo_c_gerado);
    
    printf("\n✓ COMPILAÇÃO CONCLUÍDA COM SUCESSO!\n");
    return 0;
}
```

---

## 🛠️ BUILD SYSTEM (Makefile)

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -g -I./src
LDFLAGS = -lm

# Diretórios
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Fontes
SRCS = $(wildcard $(SRC_DIR)/*/*.c) $(SRC_DIR)/main/main.c
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Alvos
TARGET = $(BIN_DIR)/compiler

# Principal
all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✓ Compilador gerado: $@"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR):
	@mkdir -p $@

$(BIN_DIR):
	@mkdir -p $@

# Teste
test: $(TARGET)
	./$(TARGET) tests/input_samples/simple.c
	./output/executable

# Limpeza
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) output/*.c output/executable

.PHONY: all test clean
```

---

## 📊 PROGRESSO DO DESENVOLVIMENTO

| Fase | Status | Arquivo | Duração Estimada |
|------|--------|---------|-----------------|
| 1: Análise Léxica | 🔄 Atual | `src/lexer/` | 1-2 semanas |
| 2: Análise Sintática | ⏳ Próximo | `src/parser/` | 2-3 semanas |
| 3: Análise Semântica | ⏳ Próximo | `src/semantic/` | 1-2 semanas |
| 4: Geração TAC | ⏳ Próximo | `src/codegen/` | 1-2 semanas |
| 5: Otimização | ⏳ Opcional | `src/codegen/optimizer/` | 1 semana |
| 6: Geração C | ⏳ Próximo | `src/backend/` | 1 semana |
| 7: Compilação | ✓ Automático | Makefile | N/A |

**Total estimado**: 7-10 semanas (trabalhando tranquilo)

---

## 🎯 CHECKLIST DO PROJETO

- [ ] Estrutura de diretórios criada
- [ ] Makefile funcional
- [ ] `lexer.h/lexer.c` implementado
- [ ] `tokens.h` definido
- [ ] `keywords.c/keywords.h` implementado
- [ ] Testes básicos passando
- [ ] `parser.h/parser.c` implementado
- [ ] `ast.h/ast.c` implementado
- [ ] Testes do parser passando
- [ ] Análise semântica implementada
- [ ] Geração de TAC implementada
- [ ] Geração de código C implementada
- [ ] Compilação final funcionando
- [ ] Testes de integração passando

---

**Próxima etapa**: Criar algoritmo detalhado de **Análise Léxica Prática**. Quer começar a programar?

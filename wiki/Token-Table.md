# 📋 Tabela de Tokens e Lexemas

**Analisador Léxico - Fase 1 do Compilador**  
**Total de Tokens:** 82+

---

## 🔑 Palavras-Chave (27 tokens)

| # | Token | Constante | Lexema | Descrição |
|---|-------|-----------|--------|-----------|
| 1 | `KW_INT` | 1 | `int` | Tipo inteiro |
| 2 | `KW_CHAR` | 2 | `char` | Tipo caractere |
| 3 | `KW_VOID` | 3 | `void` | Tipo vazio |
| 4 | `KW_FLOAT` | 4 | `float` | Tipo ponto flutuante |
| 5 | `KW_DOUBLE` | 5 | `double` | Tipo dupla precisão |
| 6 | `KW_IF` | 6 | `if` | Condicional se |
| 7 | `KW_ELSE` | 7 | `else` | Condicional senão |
| 8 | `KW_WHILE` | 8 | `while` | Loop enquanto |
| 9 | `KW_FOR` | 9 | `for` | Loop para |
| 10 | `KW_DO` | 10 | `do` | Loop fazer |
| 11 | `KW_BREAK` | 11 | `break` | Interromper loop |
| 12 | `KW_CONTINUE` | 12 | `continue` | Continuar loop |
| 13 | `KW_RETURN` | 13 | `return` | Retornar função |
| 14 | `KW_STRUCT` | 14 | `struct` | Estrutura de dados |
| 15 | `KW_UNION` | 15 | `union` | União de dados |
| 16 | `KW_TYPEDEF` | 16 | `typedef` | Tipo definido |
| 17 | `KW_SIZEOF` | 17 | `sizeof` | Tamanho em bytes |
| 18 | `KW_STATIC` | 18 | `static` | Variável estática |
| 19 | `KW_EXTERN` | 19 | `extern` | Variável externa |
| 20 | `KW_CONST` | 20 | `const` | Constante |
| 21 | `KW_VOLATILE` | 21 | `volatile` | Volátil |
| 22 | `KW_SWITCH` | 22 | `switch` | Seleção múltipla |
| 23 | `KW_CASE` | 23 | `case` | Caso do switch |
| 24 | `KW_DEFAULT` | 24 | `default` | Padrão do switch |
| 25 | `KW_GOTO` | 25 | `goto` | Desvio condicional |

---

## 👤 Identificadores e Constantes (6 tokens)

| # | Token | Constante | Lexema Exemplo | Expressão Regular |
|---|-------|-----------|-----------------|-------------------|
| 1 | `TK_ID` | 50 | `x`, `main`, `fibonacci` | `[a-zA-Z_][a-zA-Z0-9_]*` |
| 2 | `TK_NUM_INT` | 51 | `123`, `0`, `999` | `[0-9]+` |
| 3 | `TK_NUM_FLOAT` | 52 | `3.14`, `2.5` | `[0-9]+\.[0-9]+` |
| 4 | `TK_NUM_EXP` | 53 | `1e5`, `2.5e-3` | `[0-9]+(e\|E)[+-]?[0-9]+` |
| 5 | `TK_STRING` | 54 | `"hello"`, `"mundo"` | `"[^"\\]*(\\"[^"\\]*)*"` |
| 6 | `TK_CHAR` | 55 | `'A'`, `'\n'` | `'[^'\\](\\'[^'\\])*'` |

---

## ➕ Operadores Aritméticos (5 tokens)

| # | Token | Constante | Lexema | Operação |
|---|-------|-----------|--------|----------|
| 1 | `OP_PLUS` | 100 | `+` | Adição |
| 2 | `OP_MINUS` | 101 | `-` | Subtração |
| 3 | `OP_MULT` | 102 | `*` | Multiplicação |
| 4 | `OP_DIV` | 103 | `/` | Divisão |
| 5 | `OP_MOD` | 104 | `%` | Módulo |

---

## 🔄 Operadores de Atribuição (8 tokens)

| # | Token | Constante | Lexema | Equivalente |
|---|-------|-----------|--------|-------------|
| 1 | `OP_ASSIGN` | 110 | `=` | `x = 10` |
| 2 | `OP_PLUS_ASSIGN` | 111 | `+=` | `x = x + 5` |
| 3 | `OP_MINUS_ASSIGN` | 112 | `-=` | `x = x - 3` |
| 4 | `OP_MULT_ASSIGN` | 113 | `*=` | `x = x * 2` |
| 5 | `OP_DIV_ASSIGN` | 114 | `/=` | `x = x / 4` |
| 6 | `OP_AND_ASSIGN` | 115 | `&=` | `x = x & 0xFF` |
| 7 | `OP_OR_ASSIGN` | 116 | `\|=` | `x = x \| 0x01` |
| 8 | `OP_XOR_ASSIGN` | 117 | `^=` | `x = x ^ 0xAA` |

---

## 🔗 Operadores Relacionais (6 tokens)

| # | Token | Constante | Lexema | Comparação |
|---|-------|-----------|--------|------------|
| 1 | `OP_EQ` | 120 | `==` | Igual |
| 2 | `OP_NE` | 121 | `!=` | Não igual |
| 3 | `OP_LT` | 122 | `<` | Menor que |
| 4 | `OP_LE` | 123 | `<=` | Menor ou igual |
| 5 | `OP_GT` | 124 | `>` | Maior que |
| 6 | `OP_GE` | 125 | `>=` | Maior ou igual |

---

## 🧠 Operadores Lógicos (3 tokens)

| # | Token | Constante | Lexema | Operação |
|---|-------|-----------|--------|----------|
| 1 | `OP_AND` | 130 | `&&` | E lógico |
| 2 | `OP_OR` | 131 | `\|\|` | OU lógico |
| 3 | `OP_NOT` | 132 | `!` | NÃO lógico |

---

## 🔲 Operadores Bitwise (6 tokens)

| # | Token | Constante | Lexema | Operação |
|---|-------|-----------|--------|----------|
| 1 | `OP_BITAND` | 140 | `&` | E bit-a-bit |
| 2 | `OP_BITOR` | 141 | `\|` | OU bit-a-bit |
| 3 | `OP_BITXOR` | 142 | `^` | XOR bit-a-bit |
| 4 | `OP_BITNOT` | 143 | `~` | NÃO bit-a-bit |
| 5 | `OP_LSHIFT` | 144 | `<<` | Deslocamento esquerda |
| 6 | `OP_RSHIFT` | 145 | `>>` | Deslocamento direita |

---

## ➖ Operadores Unários (2 tokens)

| # | Token | Constante | Lexema | Operação |
|---|-------|-----------|--------|----------|
| 1 | `OP_INC` | 150 | `++` | Incremento |
| 2 | `OP_DEC` | 151 | `--` | Decremento |

---

## 🎯 Símbolos Especiais (11 tokens)

| # | Token | Constante | Lexema | Descrição |
|---|-------|-----------|--------|-----------|
| 1 | `SYM_LPAREN` | 200 | `(` | Parêntese esquerdo |
| 2 | `SYM_RPAREN` | 201 | `)` | Parêntese direito |
| 3 | `SYM_LBRACE` | 202 | `{` | Chave esquerda |
| 4 | `SYM_RBRACE` | 203 | `}` | Chave direita |
| 5 | `SYM_LBRACKET` | 204 | `[` | Colchete esquerdo |
| 6 | `SYM_RBRACKET` | 205 | `]` | Colchete direito |
| 7 | `SYM_SEMICOLON` | 210 | `;` | Ponto-e-vírgula |
| 8 | `SYM_COMMA` | 211 | `,` | Vírgula |
| 9 | `SYM_DOT` | 212 | `.` | Ponto |
| 10 | `SYM_COLON` | 213 | `:` | Dois-pontos |
| 11 | `SYM_TERNARY` | 214 | `?` | Interrogação |

---

## 🔧 Pré-processador (7 tokens)

| # | Token | Constante | Lexema Exemplo | Descrição |
|---|-------|-----------|-----------------|-----------|
| 1 | `TK_PP_INCLUDE` | 250 | `#include <stdio.h>` | Incluir arquivo |
| 2 | `TK_PP_DEFINE` | 251 | `#define MAX 100` | Definir constante |
| 3 | `TK_PP_IFDEF` | 252 | `#ifdef DEBUG` | Se definido |
| 4 | `TK_PP_IFNDEF` | 253 | `#ifndef RELEASE` | Se não definido |
| 5 | `TK_PP_ENDIF` | 254 | `#endif` | Fim condicional |
| 6 | `TK_PP_PRAGMA` | 255 | `#pragma pack(1)` | Pragmas |
| 7 | `TK_PP_OTHER` | 256 | `#error "msg"` | Outras diretivas |

---

## 🆘 Especiais (2 tokens)

| # | Token | Constante | Lexema | Descrição |
|---|-------|-----------|--------|-----------|
| 1 | `TK_EOF` | 300 | (vazio) | Fim de arquivo |
| 2 | `TK_ERROR` | 301 | (caractere) | Erro léxico |

---

## 📊 Resumo Estatístico

| Categoria | Quantidade | Range |
|-----------|-----------|-------|
| Palavras-chave | 27 | 1-25 |
| Identificadores | 1 | 50 |
| Constantes | 6 | 51-55 |
| Operadores Aritméticos | 5 | 100-104 |
| Operadores Atribuição | 8 | 110-117 |
| Operadores Relacionais | 6 | 120-125 |
| Operadores Lógicos | 3 | 130-132 |
| Operadores Bitwise | 6 | 140-145 |
| Operadores Unários | 2 | 150-151 |
| Símbolos | 11 | 200-214 |
| Pré-processador | 7 | 250-256 |
| Especiais | 2 | 300-301 |
| **TOTAL** | **82+** | **1-301** |

---

**Referências:**
- [Gramática Sintática](Syntactic-Grammar.md)
- [Manual do Programador](Manual-Programador.md)
- [Lexer](Lexer.md)

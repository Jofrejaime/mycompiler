# 🔤 Analisador Léxico (Lexer)

**Fase 1 do Compilador**  
**Status:** ✅ Completo

---

## 📋 Visão Geral

O **Analisador Léxico** (Lexer) é a primeira fase do compilador. Sua função é ler o código-fonte caractere por caractere e agrupá-los em **tokens** (unidades léxicas significativas).

### Responsabilidades

- Reconhecer **82+ tipos de tokens** diferentes
- Identificar **27 palavras-chave** da linguagem C
- Detectar **identificadores**, **constantes numéricas**, **strings** e **caracteres**
- Reconhecer **operadores** (aritméticos, lógicos, relacionais, bitwise)
- Identificar **símbolos especiais** (parênteses, chaves, ponto-e-vírgula, etc.)
- Processar **diretivas de pré-processador** (`#include`, `#define`, etc.)
- Reportar **erros léxicos** com linha e coluna

---

## 🏗️ Arquitetura

### Estrutura de Dados Principal

```c
typedef struct {
    int type;           // Tipo do token (constante)
    char *lexeme;       // Lexema (texto do token)
    int line;           // Linha no código-fonte
    int column;         // Coluna no código-fonte
} Token;
```

### Módulos

| Módulo | Arquivo | Descrição |
|--------|---------|-----------|
| **Lexer Principal** | `lexer.c`, `lexer.h` | Função principal `getNextToken()` |
| **Tokens** | `tokens.c`, `tokens.h` | Definições de tipos de tokens |
| **Keywords** | `keywords.c`, `keywords.h` | Tabela de palavras-chave |
| **FSM** | `lexer_fsm.c` | Máquina de estados finitos |
| **I/O** | `lexer_io.c` | Leitura de arquivo |
| **Utilitários** | `lexer_utils.c` | Funções auxiliares |

---

## 🤖 Autômato Finito {#automato}

O lexer utiliza um **Autômato Finito Determinístico (AFD)** para reconhecer tokens.

### Estados Principais

| Estado | Descrição |
|--------|-----------|
| `S0` | Estado inicial |
| `S_ID` | Reconhecendo identificador |
| `S_NUM` | Reconhecendo número inteiro |
| `S_FLOAT` | Reconhecendo número float |
| `S_STRING` | Reconhecendo string |
| `S_CHAR` | Reconhecendo caractere |
| `S_OP` | Reconhecendo operador |
| `S_COMMENT` | Dentro de comentário |

### Transições

O autômato completo está disponível em:
- **Arquivo JFLAP:** `automato/automatofinal.jff`
- **Visualização:** Abrir com [JFLAP](http://www.jflap.org/)

---

## 📊 Tokens Reconhecidos

O lexer reconhece **82+ tipos de tokens** organizados em categorias:

| Categoria | Quantidade | Exemplos |
|-----------|-----------|----------|
| **Palavras-chave** | 27 | `int`, `if`, `while`, `return` |
| **Identificadores** | 1 tipo | `x`, `main`, `fibonacci` |
| **Constantes** | 5 tipos | `123`, `3.14`, `"hello"`, `'A'` |
| **Operadores** | 30+ | `+`, `-`, `*`, `/`, `==`, `&&` |
| **Símbolos** | 11 | `(`, `)`, `{`, `}`, `;`, `,` |
| **Pré-processador** | 7 | `#include`, `#define` |

**Documentação completa:** [Tabela de Tokens](Token-Table.md)

---

## 🔍 Algoritmo Principal

### Função `getNextToken()`

```c
Token* getNextToken(Lexer *lexer) {
    // 1. Pular espaços em branco e comentários
    skipWhitespaceAndComments(lexer);
    
    // 2. Verificar fim de arquivo
    if (isEOF(lexer)) {
        return createToken(TK_EOF, "", lexer->line, lexer->column);
    }
    
    // 3. Obter caractere atual
    char c = currentChar(lexer);
    
    // 4. Reconhecer token baseado no primeiro caractere
    if (isalpha(c) || c == '_') {
        return recognizeIdentifierOrKeyword(lexer);
    }
    else if (isdigit(c)) {
        return recognizeNumber(lexer);
    }
    else if (c == '"') {
        return recognizeString(lexer);
    }
    else if (c == '\'') {
        return recognizeChar(lexer);
    }
    else if (c == '#') {
        return recognizePreprocessor(lexer);
    }
    else {
        return recognizeOperatorOrSymbol(lexer);
    }
}
```

---

## 🎯 Reconhecimento de Padrões

### Identificadores e Palavras-Chave

**Expressão Regular:** `[a-zA-Z_][a-zA-Z0-9_]*`

**Algoritmo:**
1. Ler primeiro caractere (letra ou `_`)
2. Continuar lendo enquanto for letra, dígito ou `_`
3. Verificar se é palavra-chave na tabela hash
4. Se sim, retornar token de keyword
5. Se não, retornar `TK_ID`

### Números

**Inteiros:** `[0-9]+`  
**Float:** `[0-9]+\.[0-9]+`  
**Exponencial:** `[0-9]+(e|E)[+-]?[0-9]+`

**Algoritmo:**
1. Ler dígitos
2. Se encontrar `.`, continuar lendo dígitos (float)
3. Se encontrar `e` ou `E`, ler expoente (exponencial)
4. Retornar token apropriado

### Strings

**Expressão Regular:** `"[^"\\]*(\\.[^"\\]*)*"`

**Algoritmo:**
1. Consumir `"`
2. Ler caracteres até encontrar `"` de fechamento
3. Processar sequências de escape (`\n`, `\t`, `\"`, etc.)
4. Retornar `TK_STRING`

### Operadores

**Algoritmo de Máximo Munch:**
1. Tentar reconhecer operador de 2 caracteres (`==`, `!=`, `<=`, `>=`, `&&`, `||`, etc.)
2. Se não reconhecer, tentar operador de 1 caractere (`+`, `-`, `*`, `/`, etc.)
3. Retornar token apropriado

---

## 🛠️ Tratamento de Erros

### Erros Léxicos Detectados

| Erro | Descrição | Exemplo |
|------|-----------|---------|
| **Caractere inválido** | Caractere não reconhecido | `@`, `$` |
| **String não fechada** | Falta `"` de fechamento | `"hello` |
| **Char inválido** | Mais de um caractere em `'...'` | `'abc'` |
| **Número mal formado** | Float sem dígitos após `.` | `3.` |

### Formato de Erro

```
ERRO LÉXICO [linha:coluna]: mensagem
Exemplo: ERRO LÉXICO [10:15]: Caractere inválido '@'
```

---

## 📁 Arquivos do Lexer

```
src/lexer/
├── lexer.c              # Função principal getNextToken()
├── lexer.h              # Interface pública
├── lexer_fsm.c          # Máquina de estados
├── lexer_io.c           # Leitura de arquivo
├── lexer_token.c        # Criação de tokens
├── lexer_utils.c        # Funções auxiliares
├── tokens.c             # Definições de tokens
├── tokens.h             # Constantes de tokens
├── keywords.c           # Tabela de palavras-chave
└── keywords.h           # Interface de keywords
```

---

## 🧪 Testes

### Casos de Teste

1. **Identificadores:** `x`, `main`, `_temp`, `var123`
2. **Números:** `0`, `123`, `3.14`, `2.5e-3`
3. **Strings:** `"hello"`, `"world\n"`, `""`
4. **Operadores:** `+`, `-`, `==`, `!=`, `&&`, `||`
5. **Comentários:** `// linha`, `/* bloco */`
6. **Pré-processador:** `#include <stdio.h>`, `#define MAX 100`

### Executar Testes

```bash
make test-lexer
```

---

## 📚 Referências

- [Tabela de Tokens](Token-Table.md)
- [Manual do Programador](Manual-Programador.md)
- [Gramática Sintática](Syntactic-Grammar.md)

---

**Implementação:** C99  
**Compilador:** GCC / Clang  
**Status:** ✅ Completo e Testado

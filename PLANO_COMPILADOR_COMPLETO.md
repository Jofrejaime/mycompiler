# 📚 Plano Completo: Compilador C → C

## Baseado no Livro: "Compiladores - Princípios, Técnicas e Ferramentas"

---

## 1️⃣ AS FASES DA COMPILAÇÃO

O processo de compilação é dividido em **2 partes principais**:

### **PARTE I: ANÁLISE (Frontend)**

```
Código Fonte (C)
       ↓
    ┌─────────────────────────────────┐
    │   1. ANÁLISE LÉXICA             │ ← Você está aqui
    │      (Lexer/Tokenizador)        │
    └────────────┬────────────────────┘
                 ↓
         Stream de Tokens
                 ↓
    ┌─────────────────────────────────┐
    │   2. ANÁLISE SINTÁTICA          │
    │      (Parser)                   │
    └────────────┬────────────────────┘
                 ↓
         Árvore Sintática (AST)
                 ↓
    ┌─────────────────────────────────┐
    │   3. ANÁLISE SEMÂNTICA          │
    │      (Semantic Analysis)        │
    └────────────┬────────────────────┘
                 ↓
        AST Anotada + Tabela de Símbolos
```

### **PARTE II: SÍNTESE (Backend)**

```
        AST Anotada + Símbolos
                 ↓
    ┌─────────────────────────────────┐
    │   4. GERAÇÃO DE CÓDIGO          │
    │      INTERMEDIÁRIO (IR)         │
    └────────────┬────────────────────┘
                 ↓
    Código Intermediário (Three-Address Code)
                 ↓
    ┌─────────────────────────────────┐
    │   5. OTIMIZAÇÃO                 │
    │      (Optimization)             │
    └────────────┬────────────────────┘
                 ↓
    Código Intermediário Otimizado
                 ↓
    ┌─────────────────────────────────┐
    │   6. GERAÇÃO DE CÓDIGO DESTINO  │
    │      (Code Generation)          │
    └────────────┬────────────────────┘
                 ↓
    Código C (Destino)
                 ↓
    ┌─────────────────────────────────┐
    │   7. MONTAGEM E LIGAÇÃO         │
    │      (Compiler + Linker)        │
    └────────────┬────────────────────┘
                 ↓
    Executável
```

---

## 2️⃣ FOCO: ANÁLISE LÉXICA (Analisador Léxico)

### **O que é?**

O **analisador léxico** (lexer) é a **primeira fase** do compilador. Ele:
- Lê o código-fonte **carácter por carácter**
- Agrupa caracteres em **tokens** (unidades significativas)
- Remove **espaços em branco** e **comentários**
- Detecta **erros léxicos**

### **Entrada → Saída**

```
ENTRADA (Código C):
─────────────────────
int main() { 
    int x = 10;
    return 0;
}

↓ (LEXER)

SAÍDA (Stream de Tokens):
──────────────────────────
TOKEN(ID, "int")          linha 1
TOKEN(ID, "main")         linha 1
TOKEN(LPAREN, "(")        linha 1
TOKEN(RPAREN, ")")        linha 1
TOKEN(LBRACE, "{")        linha 1
TOKEN(ID, "int")          linha 2
TOKEN(ID, "x")            linha 2
TOKEN(ASSIGN, "=")        linha 2
TOKEN(NUM, "10")          linha 2
TOKEN(SEMICOLON, ";")     linha 2
TOKEN(RETURN, "return")   linha 3
TOKEN(NUM, "0")           linha 3
TOKEN(SEMICOLON, ";")     linha 3
TOKEN(RBRACE, "}")        linha 4
TOKEN(EOF)
```

### **Componentes do Analisador Léxico**

```
┌─────────────────────────────────────────────────────┐
│              ANALISADOR LÉXICO                      │
├─────────────────────────────────────────────────────┤
│                                                     │
│  1. Buffer de Entrada                              │
│     ↓                                               │
│  2. Autômato (seu .jff!)                           │
│     ↓                                               │
│  3. Processador de Tokens                          │
│     ↓                                               │
│  4. Gerenciador de Erros                           │
│     ↓                                               │
│  5. Tabela de Símbolos (inicializada)              │
│                                                     │
└─────────────────────────────────────────────────────┘
```

---

## 3️⃣ CONCEITOS-CHAVE DO ANALISADOR LÉXICO

### **A. Tokens**

Um **token** é uma **tupla** contendo:

```
TOKEN = (tipo, valor, linha, coluna)

Exemplos:
─────────────────────────────────────
(KW_INT,      "int",         1, 1)     ← Palavra-chave
(ID,          "main",        1, 5)     ← Identificador
(NUM_INT,     "10",          2, 13)    ← Número
(STRING,      "Hello",       3, 10)    ← String
(OPERATOR_OP, "==",          4, 5)     ← Operador
(SEMICOLON,   ";",           5, 1)     ← Símbolo
```

### **B. Padrões Léxicos (do seu autômato)**

```
Identificador:  [a-zA-Z_][a-zA-Z0-9_]*
Número:         [0-9]+ | [0-9]+\.[0-9]+ | [0-9]+[eE][+-]?[0-9]+
String:         \"([^\"\\]|\\.)*\"
Comentário:     //.*  ou  /* ... */
Operador:       +, -, *, /, ==, !=, +=, etc.
Símbolo:        ;, {, }, (, ), [, ], ,, :, etc.
```

### **C. Autômatos Finitos**

Você já criou isto! Seu `.jff` define:

```
Estado Inicial: q0
         ↓
Transições: q0 →[a-zA-Z_]→ q1 →[a-zA-Z0-9_]*→ q1 (loop)
            q1 →[outro]→ FINAL (encontrou ID)

Cada token é reconhecido por um caminho no autômato
```

---

## 4️⃣ ESTRUTURA DO ANALISADOR LÉXICO (Pseudocódigo)

```c
struct Token {
    int tipo;          // ID, NUM, STRING, etc.
    char *valor;       // "main", "10", "hello"
    int linha;         // linha do código
    int coluna;        // coluna do código
};

TokenStream *lexer(char *codigo_fonte) {
    TokenStream *ts = criar_stream();
    int i = 0;
    
    while (i < len(codigo_fonte)) {
        
        // 1. Pular espaços em branco (não são tokens)
        if (eh_espacobrancho(codigo_fonte[i])) {
            i++;
            continue;
        }
        
        // 2. Reconhecer comentários (ignorar)
        if (eh_comentario(codigo_fonte[i..i+1])) {
            pular_comentario(codigo_fonte, &i);
            continue;
        }
        
        // 3. Reconhecer strings
        if (codigo_fonte[i] == '"') {
            Token t = reconhecer_string(codigo_fonte, &i);
            adicionar_token(ts, t);
            continue;
        }
        
        // 4. Reconhecer números
        if (eh_digito(codigo_fonte[i])) {
            Token t = reconhecer_numero(codigo_fonte, &i);
            adicionar_token(ts, t);
            continue;
        }
        
        // 5. Reconhecer identificadores ou palavras-chave
        if (eh_letra(codigo_fonte[i]) || codigo_fonte[i] == '_') {
            Token t = reconhecer_id_ou_palavra_chave(codigo_fonte, &i);
            adicionar_token(ts, t);
            continue;
        }
        
        // 6. Reconhecer operadores e símbolos
        if (eh_operador_ou_simbolo(codigo_fonte[i])) {
            Token t = reconhecer_op_simb(codigo_fonte, &i);
            adicionar_token(ts, t);
            continue;
        }
        
        // 7. Erro léxico
        erro_lexico("Carácter inesperado", i, linha);
        i++;
    }
    
    adicionar_token(ts, TOKEN_EOF);
    return ts;
}
```

---

## 5️⃣ FASES DETALHADAS DA ANÁLISE LÉXICA

### **Fase 1: Leitura do Código**

```
char codigo[] = "int x = 10;";
                 ↓
Buffer:  i n t   x   =   1 0 ;
Índice:  0 1 2 3 4 5 6 7 8 9 10
```

### **Fase 2: Varredura (Scanning)**

```
Posição 0: 'i' → é letra
           'i' 'n' 't' → TOKEN(ID, "int")
           
Posição 3: ' ' → pular espaço
           
Posição 4: 'x' → é letra
           'x' → TOKEN(ID, "x")
           
Posição 6: '=' → é operador
           '=' (próximo não é '=') → TOKEN(ASSIGN, "=")
           
Posição 8: '1' → é dígito
           '1' '0' → TOKEN(NUM, "10")
           
Posição 10: ';' → TOKEN(SEMICOLON, ";")
```

### **Fase 3: Criação de Tokens**

```
Cada token vai para uma fila/lista:

[TOKEN(ID, "int", 1, 1)] 
    ↓
[TOKEN(ID, "x", 1, 5)]
    ↓
[TOKEN(ASSIGN, "=", 1, 7)]
    ↓
[TOKEN(NUM, "10", 1, 9)]
    ↓
[TOKEN(SEMICOLON, ";", 1, 11)]
    ↓
[TOKEN(EOF)]
```

### **Fase 4: Processamento Especial**

#### **A. Palavras-Chave vs Identificadores**

```c
// Reconhece "int" como TOKEN(ID, "int")
// Depois consulta Tabela de Palavras-Chave

if (valor == "int")    → TOKEN(KW_INT)
if (valor == "return") → TOKEN(KW_RETURN)
if (valor == "if")     → TOKEN(KW_IF)
else                   → TOKEN(ID) ← continua ID
```

#### **B. Tabela de Palavras-Chave (Lookup Table)**

```
"auto"       → KW_AUTO
"break"      → KW_BREAK
"case"       → KW_CASE
"char"       → KW_CHAR
"const"      → KW_CONST
"continue"   → KW_CONTINUE
"default"    → KW_DEFAULT
"do"         → KW_DO
"double"     → KW_DOUBLE
"else"       → KW_ELSE
"enum"       → KW_ENUM
"extern"     → KW_EXTERN
"float"      → KW_FLOAT
"for"        → KW_FOR
"goto"       → KW_GOTO
"if"         → KW_IF
"inline"     → KW_INLINE
"int"        → KW_INT
"long"       → KW_LONG
"register"   → KW_REGISTER
"restrict"   → KW_RESTRICT
"return"     → KW_RETURN
"short"      → KW_SHORT
"signed"     → KW_SIGNED
"sizeof"     → KW_SIZEOF
"static"     → KW_STATIC
"struct"     → KW_STRUCT
"switch"     → KW_SWITCH
"typedef"    → KW_TYPEDEF
"union"      → KW_UNION
"unsigned"   → KW_UNSIGNED
"void"       → KW_VOID
"volatile"   → KW_VOLATILE
"while"      → KW_WHILE
```

---

## 6️⃣ ESTRUTURA DE DADOS

### **Tipo Token (C)**

```c
#define MAX_LEXEME 256

typedef enum {
    // Palavras-chave
    KW_INT, KW_CHAR, KW_VOID, KW_RETURN, KW_IF, KW_ELSE,
    KW_WHILE, KW_FOR, KW_DO, KW_BREAK, KW_CONTINUE,
    KW_STRUCT, KW_UNION, KW_TYPEDEF, KW_SIZEOF,
    
    // Tipos de tokens
    ID,           // Identificador
    NUM_INT,      // Número inteiro
    NUM_FLOAT,    // Número float
    NUM_EXP,      // Notação científica
    STRING,       // String literal
    CHAR,         // Char literal
    
    // Operadores
    OP_PLUS,      // +
    OP_MINUS,     // -
    OP_MULT,      // *
    OP_DIV,       // /
    OP_MOD,       // %
    OP_ASSIGN,    // =
    OP_PLUS_ASSIGN,   // +=
    OP_MINUS_ASSIGN,  // -=
    OP_MULT_ASSIGN,   // *=
    OP_DIV_ASSIGN,    // /=
    OP_EQ,        // ==
    OP_NE,        // !=
    OP_LT,        // <
    OP_LE,        // <=
    OP_GT,        // >
    OP_GE,        // >=
    OP_LSHIFT,    // <<
    OP_RSHIFT,    // >>
    OP_AND,       // &&
    OP_OR,        // ||
    OP_NOT,       // !
    OP_BITAND,    // &
    OP_BITOR,     // |
    OP_BITXOR,    // ^
    OP_BITNOT,    // ~
    OP_INC,       // ++
    OP_DEC,       // --
    OP_ARROW,     // ->
    
    // Símbolos
    LPAREN,       // (
    RPAREN,       // )
    LBRACE,       // {
    RBRACE,       // }
    LBRACKET,     // [
    RBRACKET,     // ]
    SEMICOLON,    // ;
    COMMA,        // ,
    DOT,          // .
    COLON,        // :
    TERNARY,      // ?
    
    EOF_TOKEN,
    ERROR
} TokenType;

typedef struct {
    TokenType tipo;
    char lexeme[MAX_LEXEME];  // "int", "x", "10", etc.
    int linha;
    int coluna;
    union {
        int valor_int;
        double valor_float;
    } valor;
} Token;

typedef struct {
    Token *tokens;
    int tamanho;
    int capacidade;
    int posicao_atual;  // Para o parser iterar
} TokenStream;
```

---

## 7️⃣ FLUXO COMPLETO: DO CÓDIGO AO TOKEN

### **Exemplo Prático**: Compilar `int x = 10;`

```
┌─ Entrada ──────────────────────────────────────────────┐
│ "int x = 10;"                                           │
└────────────────────────────────────────────────────────┘
                        ↓
    ┌─ Lexer começa ──────────────┐
    │ i=0, linha=1, coluna=1      │
    └─────────────────────────────┘
                        ↓
    ┌─ Posição 0: 'i' ────────────────────────────┐
    │ É letra? SIM                                 │
    │ Acumular: 'i', 'n', 't'                     │
    │ Próximo é espaço? SIM → parar               │
    │ lexeme = "int"                              │
    │ Consultar tabela de palavras-chave          │
    │ "int" é palavra-chave? SIM                  │
    │ TOKEN(KW_INT, "int", 1, 1)                  │
    └─────────────────────────────────────────────┘
                        ↓
    ┌─ Posição 3: ' ' ────────────────────────────┐
    │ É espaço? SIM → ignorar, i++                │
    └─────────────────────────────────────────────┘
                        ↓
    ┌─ Posição 4: 'x' ────────────────────────────┐
    │ É letra? SIM                                 │
    │ Acumular: 'x'                               │
    │ Próximo é espaço? SIM → parar               │
    │ lexeme = "x"                                │
    │ Consultar tabela de palavras-chave          │
    │ "x" é palavra-chave? NÃO                    │
    │ TOKEN(ID, "x", 1, 5)                        │
    └─────────────────────────────────────────────┘
                        ↓
    ┌─ Posição 5: ' ' ────────────────────────────┐
    │ É espaço? SIM → ignorar, i++                │
    └─────────────────────────────────────────────┘
                        ↓
    ┌─ Posição 6: '=' ────────────────────────────┐
    │ É operador? SIM                              │
    │ Próximo é '='? NÃO (é ' ')                  │
    │ lexeme = "="                                │
    │ TOKEN(OP_ASSIGN, "=", 1, 7)                 │
    └─────────────────────────────────────────────┘
                        ↓
    ┌─ Posição 7: ' ' ────────────────────────────┐
    │ É espaço? SIM → ignorar, i++                │
    └─────────────────────────────────────────────┘
                        ↓
    ┌─ Posição 8: '1' ────────────────────────────┐
    │ É dígito? SIM                                │
    │ Acumular: '1', '0'                          │
    │ Próximo é ';'? SIM → parar                  │
    │ lexeme = "10"                               │
    │ valor_int = 10                              │
    │ TOKEN(NUM_INT, "10", 1, 9) {valor: 10}     │
    └─────────────────────────────────────────────┘
                        ↓
    ┌─ Posição 10: ';' ──────────────────────────┐
    │ É símbolo? SIM                               │
    │ lexeme = ";"                                │
    │ TOKEN(SEMICOLON, ";", 1, 11)                │
    └─────────────────────────────────────────────┘
                        ↓
    ┌─ Fim do arquivo ────────────────────────────┐
    │ TOKEN(EOF_TOKEN, "", 1, 12)                 │
    └─────────────────────────────────────────────┘
                        ↓
┌─ Saída (TokenStream) ──────────────────────────────┐
│ [KW_INT] [ID] [OP_ASSIGN] [NUM_INT] [SEMICOLON] [EOF]│
│ "int"    "x"  "="         "10"      ";"         ""   │
└────────────────────────────────────────────────────┘
```

---

## 8️⃣ DETECÇÃO E TRATAMENTO DE ERROS

### **Tipos de Erros Léxicos**

```
1. Carácter inválido
   ❌ "int x€ = 10;"  // € não é válido
   Erro: "Carácter € não reconhecido na linha 1, coluna 7"

2. String não terminada
   ❌ "Hello   // Falta "
   Erro: "String não terminada na linha 1"

3. Número mal formado
   ❌ 0x123G5  // G não é dígito hex
   Erro: "Número hexadecimal inválido na linha 1"

4. Comentário não fechado
   ❌ /* comentário inicia mas...
   Erro: "Comentário não fechado no final do arquivo"
```

### **Estratégias de Recuperação de Erros**

```c
// 1. Panic Mode (mais comum)
//    Pular até encontrar um token sincronizador
if (erro_lexico) {
    reportar_erro();
    while (!eh_token_sincronizador(codigo[i])) {
        i++;  // Pular caracteres
    }
}

// 2. Modo Ortográfico
//    Tentar corrigir pequenos erros
if (codigo[i] == '€') {
    erro_lexico("Carácter € inválido, assumindo espaço");
    i++;  // ignora
}
```

---

## 9️⃣ RELAÇÃO COM SEU ARQUIVO .JFF

Seu arquivo `CompiladorAutomatos.jff` define os **autômatos finitos** que reconhecem cada padrão:

```
JFF (seu arquivo): Define COMO reconhecer
                   ├─ Identificadores
                   ├─ Números
                   ├─ Strings
                   ├─ Comentários
                   ├─ Operadores
                   └─ Símbolos

C (seu lexer.c):   Implementa ONDE usar
                   ├─ Simular autômato
                   ├─ Acumular caracteres
                   ├─ Criar tokens
                   ├─ Consultar palavras-chave
                   └─ Reportar erros
```

---

## 🔟 EXERCÍCIO: TRACE MANUAL

Vamos traçar `int main() {` passo a passo:

```
Código: "int main() {"
Índice:  012345678910

┌─ Passo 1 ─────────────────────┐
│ i=0: 'i'                      │
│ Reconhecer: i-n-t             │
│ Token: (KW_INT, "int", 1, 1)  │
│ Novo i: 3                     │
└───────────────────────────────┘

┌─ Passo 2 ─────────────────────┐
│ i=3: ' '                      │
│ É espaço: SIM                 │
│ Ignorar, novo i: 4            │
└───────────────────────────────┘

┌─ Passo 3 ─────────────────────┐
│ i=4: 'm'                      │
│ Reconhecer: m-a-i-n           │
│ Token: (ID, "main", 1, 5)     │
│ Novo i: 8                     │
└───────────────────────────────┘

┌─ Passo 4 ─────────────────────┐
│ i=8: '('                      │
│ Reconhecer: (                 │
│ Token: (LPAREN, "(", 1, 9)    │
│ Novo i: 9                     │
└───────────────────────────────┘

┌─ Passo 5 ─────────────────────┐
│ i=9: ')'                      │
│ Reconhecer: )                 │
│ Token: (RPAREN, ")", 1, 10)   │
│ Novo i: 10                    │
└───────────────────────────────┘

┌─ Passo 6 ─────────────────────┐
│ i=10: ' '                     │
│ É espaço: SIM                 │
│ Ignorar, novo i: 11           │
└───────────────────────────────┘

┌─ Passo 7 ─────────────────────┐
│ i=11: '{'                     │
│ Reconhecer: {                 │
│ Token: (LBRACE, "{", 1, 12)   │
│ Novo i: 12                    │
└───────────────────────────────┘

RESULTADO FINAL:
─────────────────────────────────
[ (KW_INT, "int", 1, 1),
  (ID, "main", 1, 5),
  (LPAREN, "(", 1, 9),
  (RPAREN, ")", 1, 10),
  (LBRACE, "{", 1, 12),
  (EOF_TOKEN) ]
```

---

## 1️⃣1️⃣ TABELA COMPARATIVA: LIVRO vs PRÁTICA

| Conceito | No Livro | Na Prática (Você) |
|----------|----------|------------------|
| **Autômato** | Teórico, estados/transições | Seu .jff com q0-q79 |
| **Padrão Léxico** | Regex `[a-zA-Z_][a-zA-Z0-9_]*` | Seu autômato para ID |
| **Token** | (tipo, valor) | `struct Token` em C |
| **Buffer** | Conceitual | Array `char []` |
| **Tabela Símbolos** | Para análise semântica | Para guarda IDs |
| **Lookahead** | Até 1 caractere | `codigo[i+1]` |

---

## Resumo Visual: Fluxo Completo

```
┌──────────────────────────────────────────┐
│     CÓDIGO FONTE (arquivo .c)             │
│     int x = 10; return 0;                 │
└──────────┬───────────────────────────────┘
           │
           ↓
┌──────────────────────────────────────────┐
│  ANÁLISE LÉXICA (seu lexer.c)            │
│  ├─ Ler carácter por carácter            │
│  ├─ Consultar autômato (seu .jff)        │
│  ├─ Acumular em lexeme                   │
│  ├─ Criar token                          │
│  └─ Devolver TokenStream                 │
└──────────┬───────────────────────────────┘
           │
           ↓
┌──────────────────────────────────────────┐
│  STREAM DE TOKENS                        │
│  KW_INT ID OP_ASSIGN NUM_INT SEMICOLON   │
│  "int" "x" "="       "10"   ";"          │
│  KW_RETURN NUM_INT SEMICOLON EOF         │
│  "return" "0"      ";"       ""          │
└──────────────────────────────────────────┘
           │
           ↓ (vai para Parser)
┌──────────────────────────────────────────┐
│  ANÁLISE SINTÁTICA (próxima fase)        │
│  (Não abordamos agora)                   │
└──────────────────────────────────────────┘
```

---

**PRÓXIMO DOCUMENTO**: Vou criar o detalhamento de **TODAS AS 7 FASES** com exemplos práticos. Quer que eu continue?

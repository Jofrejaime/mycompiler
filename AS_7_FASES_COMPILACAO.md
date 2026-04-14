# 📖 AS 7 FASES DE UM COMPILADOR

## Baseado no Livro: "Compiladores - Princípios, Técnicas e Ferramentas" (Livro do Dragão)

---

## FASE 1️⃣: ANÁLISE LÉXICA (Lexical Analysis)

### O que faz?
Transforma uma sequência de caracteres em uma sequência de **tokens** (unidades léxicas).

### Por quê?
Facilitamos a análise sintática subsequente. Em vez de trabalhar com caracteres soltos, o parser trabalha com tokens bem definidos.

### Exemplo

```
ENTRADA:
────────
if (x >= 10) {
    y = x + 5;
}

↓ (Análise Léxica)

SAÍDA (tokens):
───────────────
KW_IF          "if"
LPAREN         "("
ID             "x"
OP_GE          ">="
NUM_INT        "10"
RPAREN         ")"
LBRACE         "{"
ID             "y"
OP_ASSIGN      "="
ID             "x"
OP_PLUS        "+"
NUM_INT        "5"
SEMICOLON      ";"
RBRACE         "}"
EOF_TOKEN      ""
```

### Responsabilidades

```
✓ Reconhecer padrões léxicos (tokens)
✓ Ignorar espaços em branco
✓ Ignorar comentários
✓ Rastrear números de linha e coluna
✓ Reportar erros léxicos
✓ Manter tabela de palavras-chave
✓ Manter tabela de símbolos (inicial)
✓ Gerar atributos de token (valor do número, etc.)
```

### Arquivos Envolvidos (seu projeto)

```
automato/CompiladorAutomatos.jff  ← Define autômatos
src/lexer/lexer.c                 ← Implementa fase 1
src/lexer/lexer.h                 ← Interface
```

---

## FASE 2️⃣: ANÁLISE SINTÁTICA (Syntax Analysis)

### O que faz?
Verifica se a sequência de tokens segue as regras gramaticais da linguagem.

### Por quê?
Estrutura os tokens em uma **Árvore Sintática Abstrata (AST)** que representa o programa de forma hierárquica.

### Exemplo

```
ENTRADA (tokens):
──────────────────
IF LPAREN ID OP_GE NUM RPAREN LBRACE ID OP_ASSIGN ID OP_PLUS NUM SEMICOLON RBRACE

↓ (Análise Sintática)

SAÍDA (Árvore Sintática):
────────────────────────
                Statement
                    │
                IF_Statement
               /    |     \
              /     |      \
         Condition Body  (else?)
           |         |
        BinOp      Block
       /  |  \       |
      ID ">=" NUM   Statements
      x   10       /  \
                  /    \
            Assignment  ...
            /  |  \
           ID "=" BinOp
           y    /  |  \
              ID "+" NUM
              x     5
```

### Regra Gramatical (BNF - Backus-Naur Form)

```
Programa    → Declarações Função*
Função      → Tipo ID LPAREN Parâmetros RPAREN Bloco
Bloco       → LBRACE Declarações Declarações RBRACE
Declaração  → Tipo ID (ASSIGN Expressão)? SEMICOLON
             | Comando

Comando     → IF LPAREN Expressão RPAREN Comando
             | WHILE LPAREN Expressão RPAREN Comando
             | LBRACE Comandos RBRACE
             | ID ASSIGN Expressão SEMICOLON
             | RETURN Expressão SEMICOLON

Expressão   → Expressão PLUS Termo
             | Expressão MINUS Termo
             | Termo

Termo       → Termo MULT Fator
             | Termo DIV Fator
             | Fator

Fator       → LPAREN Expressão RPAREN
             | NUM
             | ID
```

### Tipos de Analisadores Sintáticos

| Tipo | Método | Velocidade | Complexidade |
|------|--------|-----------|--------------|
| **Top-Down** | LL(1) | Rápido | Fácil |
|             | Recursive Descent | Rápido | Fácil |
| **Bottom-Up** | LR(0) | Lento | Difícil |
|              | SLR(1) | Médio | Médio |
|              | LR(1) | Lento | Muito Difícil |
|              | LALR(1) | Médio | Médio |

**Recomendação para seu primeiro compilador**: `LL(1)` com **Recursive Descent**

### Saída desta fase

```c
typedef struct ASTNode {
    int tipo;  // DECLARACAO, IF_STATEMENT, etc.
    struct ASTNode **filhos;
    int num_filhos;
    char *valor;
    // ... mais campos
} ASTNode;
```

---

## FASE 3️⃣: ANÁLISE SEMÂNTICA (Semantic Analysis)

### O que faz?
Verifica se o programa é **semanticamente correto**, mesmo que sintaticamente correto.

### Por quê?
Um programa pode ser sintaticamente válido mas semanticamente inválido. Exemplo:

```c
// ✓ Sintaticamente correto
// ✗ Semanticamente inválido: não pode fazer operação com tipos diferentes
int x = "hello" + 5;  

// ✓ Sintaticamente correto
// ✗ Semanticamente inválido: variável não declarada
y = 10;
```

### Verificações Realizadas

```
✓ Verificação de tipos
  - Operação entre tipos compatíveis?
  - Atribuição entre tipos compatíveis?

✓ Verificação de escopo
  - Variável foi declarada?
  - Está no escopo correto?

✓ Verificação de argumentos de função
  - Número de argumentos correto?
  - Tipos de argumentos corretos?

✓ Verificação de breakpoints (break/continue)
  - break/continue só podem estar em loops?

✓ Verificação de return
  - Função retorna o tipo correto?

✓ Verificação de arrays
  - Índice é inteiro?
  - Acesso dentro dos limites? (dinâmico/estático)

✓ Verificação de ponteiros
  - Dereference de ponteiro válido?
```

### Estrutura de Dados: Tabela de Símbolos

```c
typedef struct {
    char *nome;
    int tipo;      // INT, CHAR, FLOAT, etc.
    int escopo;    // 0=global, 1+=local
    int linha;
    int coluna;
    // Para função:
    int num_parametros;
    int *tipo_parametros;
    // Para variável:
    int endereco;  // No código intermediário
} Simbolo;

typedef struct {
    Simbolo *tabela;
    int tamanho;
    int escopo_atual;
} TabelaSimbolos;
```

### Exemplo de Análise Semântica

```c
int main() {
    int x = 5;      // ✓ OK: 5 é int, x é int
    x = "hello";    // ✗ ERRO: "hello" é string, x é int
    y = 10;         // ✗ ERRO: y não está declarada
    return 0;       // ✓ OK: return int em função int
}
```

### Saída desta fase

```
AST Anotada com info. de tipos e símbolos:

                IF_Statement
               /    |     \
          Expressão  Corps
          /  |  \
        ID ">=" NUM     (TIPOS: int, int, int)
        (tipo:int)
```

---

## FASE 4️⃣: GERAÇÃO DE CÓDIGO INTERMEDIÁRIO (Intermediate Code Generation)

### O que faz?
Transforma a AST em uma **representação intermediária** independente da máquina alvo.

### Por quê?
Facilita otimizações e compilação para múltiplas máquinas-alvo. Exemplo: transpilação para C, JavaScript, etc.

### Representação Intermediária Comum: TAC (Three-Address Code)

Cada instrução tem **no máximo 3 endereços**:

```
op x, y, z    // z = op(x, y)

Exemplos:
─────────
x = y + z          // ADD y, z, x
a = b * c + d      // MUL b, c, t1
                   // ADD t1, d, a

if (x > 10)        // GT x, 10, t1
                   // IFNOT t1, label_else
                   // ... THEN block ...
                   // GOTO label_end
                   // label_else:
                   // ... ELSE block ...
                   // label_end:
```

### Exemplo Prático

```c
CÓDIGO C:
─────────
int main() {
    int x = 10;
    int y = x + 5;
    int z = y * 2;
    return z;
}

↓ (Gera TAC)

CÓDIGO INTERMEDIÁRIO (TAC):
───────────────────────────
MAIN:
  x = 10              // ASSIGN 10, x
  t1 = x + 5          // ADD x, 5, t1
  y = t1              // ASSIGN t1, y
  t2 = y * 2          // MUL y, 2, t2
  z = t2              // ASSIGN t2, z
  return z            // RETURN z
```

### Operações Intermediárias Comuns

```
Aritmética:  ADD, SUB, MUL, DIV, MOD
Comparação:  LT, LE, GT, GE, EQ, NE
Lógica:      AND, OR, NOT
Controle:    GOTO, IFNOT, IFTRUE
Funções:     CALL, RETURN, PARAM
Memória:     LOAD, STORE
```

---

## FASE 5️⃣: OTIMIZAÇÃO DE CÓDIGO (Code Optimization)

### O que faz?
Melhora o código intermediário para ser mais **rápido** ou usar **menos memória**.

### Por quê?
Um compilador é responsável por gerar código eficiente.

### Tipos de Otimizações (do simples ao complexo)

#### **Nível 1: Otimizações Locais**

```c
// Dobragem de constantes
int x = 5 + 3;          → int x = 8;
int y = 2 * x;

// Propagação de cópia
x = 5;
y = x;                  → y = 5;

// Código morto
if (0) {
    código aqui;        → remover este bloco
}
```

#### **Nível 2: Otimizações Globais**

```c
// Eliminação de sub-expressões comuns
a = x + y;
b = x + y;              → t = x + y;
c = x + y;                 a = t;
                           b = t;
                           c = t;

// Loop strength reduction
for (i = 0; i < 1000; i++) {
    x = i * 4;          → for (i = 0; i < 1000; i++) {
}                             x += 4;
                           }
```

#### **Nível 3: Otimizações de Função**

```c
// Inlining (inserir código da função no local de chamada)
int quadrado(int x) {
    return x * x;
}

int y = quadrado(5);    → int y = 5 * 5;

// Tail recursion (recursão de cauda)
// Transformar em iteração
```

### Saída desta fase

```
TAC Otimizado:
──────────────
MAIN:
  x = 10
  t1 = x + 5            ← Pode ser removido se x não mudar
  y = t1
  t2 = y MULT 2
  z = t2
  RETURN z
```

**Nota**: Para seu primeiro compilador, **pule otimizações**. Foque em corretude primeiro.

---

## FASE 6️⃣: GERAÇÃO DE CÓDIGO (Code Generation)

### O que faz?
Transforma a TAC (código intermediário) em **código da máquina alvo** ou **linguagem alvo**.

### Por quê?
É onde finalmente o programa é executável.

### Opções de Código Alvo

```
Opção A: Código assembly  → Ligador → Executável
         (x86, ARM, etc.)

Opção B: Código C         → Compilador C → Executável
         (seu caso!)

Opção C: Bytecode         → VM → Executa
         (Java, C#)

Opção D: Código máquina   → CPU → Executa
         (muito complexo)
```

### Seu Projeto: TAC → C

```c
TAC:
────
MAIN:
  x = 10
  t1 = x + 5
  y = t1
  t2 = y * 2
  z = t2
  RETURN z

↓ (Geração)

CÓDIGO C (ALVO):
────────────────
int main() {
    int x;
    int t1;
    int y;
    int t2;
    int z;
    
    x = 10;
    t1 = (x + 5);
    y = t1;
    t2 = (y * 2);
    z = t2;
    return z;
}
```

### Mapeamento de Operações

```
TAC             → C
ADD x, y, z     → z = x + y;
SUB x, y, z     → z = x - y;
MUL x, y, z     → z = x * y;
DIV x, y, z     → z = x / y;
MOD x, y, z     → z = x % y;

LT x, y, z      → z = (x < y);
LE x, y, z      → z = (x <= y);
GT x, y, z      → z = (x > y);
GE x, y, z      → z = (x >= y);
EQ x, y, z      → z = (x == y);
NE x, y, z      → z = (x != y);

AND x, y, z     → z = (x && y);
OR x, y, z      → z = (x || y);
NOT x, z        → z = !(x);

GOTO L          → goto L;
IFNOT x, L      → if (!(x)) goto L;
IFTRUE x, L     → if (x) goto L;

CALL func, args → func(args);
RETURN x        → return x;

ASSIGN x, y     → y = x;

LOAD ptr, off, x    → x = *(ptr + off);
STORE x, ptr, off   → *(ptr + off) = x;
```

---

## FASE 7️⃣: MONTAGEM E LIGAÇÃO (Assembly and Linking)

### O que faz?
Após gerar código C, você precisa compilar e linkar esse código.

### Por quê?
O código C intermediário precisa ser convertido em executável.

### Fluxo Simplificado

```
Seu TAC
   ↓↓↓
Seu Compilador (escrito em C)
   ↓↓↓
código.c (gerado)
   ↓↓↓
Compilador C do sistema (gcc, clang)
   ↓↓↓
código.o (object file)
   ↓↓↓
Ligador (linker)
   ↓↓↓
executável (seu programa C compilado)
```

### Exemplo

```bash
# Seu compilador gera:
gcc mycompiler input.c -o input.c

# Você compila o código gerado:
gcc input.c -o output

# Executa:
./output
```

---

## 📊 VISÃO GERAL: AS 7 FASES

```
┌─────────────────────────────────────────────────────────┐
│                  PROGRAMA FONTE (C)                     │
│              "int x = 10; ... return 0;"                │
└──────────────────────┬──────────────────────────────────┘
                       │
        ╔══════════════╩═══════════════╗
        │    ANÁLISE   (Frontend)      │
        ╚═══╤══════════╤══════════════╗│
            │          │               ││
    ┌───────▼──┐ ┌────▼────┐ ┌───────▼┐│
    │  LÉXICA  │ │SINTÁTICA │ │SEMÂNTICA│
    │(Tokens)  │ │(AST)     │ │(AST+sim)│
    └───────┬──┘ └────┬────┘ └───────┬─┘
            │         │              │
        ╔═══▼══════════▼══════════════╩╗
        │  CÓDIGO INTERMEDIÁRIO (TAC) │
        │      x=10; t1=x+5; etc      │
        ╚═══╤══════════════════════════╝
            │
        ╔═══▼══════════════╗
        │ OTIMIZAÇÃO       │ ← Opcional para primeiro compilador
        │ (Código + Rápido)│
        ╚═══╤══════════════╝
            │
    ┌───────▼──────────┐
    │  GERAÇÃO (C)     │
    │ "int x = 10;..." │
    └───────┬──────────┘
            │
    ┌───────▼──────────┐
    │  COMPILAÇÃO      │
    │  (gcc compiler)  │
    └───────┬──────────┘
            │
    ┌───────▼──────────┐
    │  LIGAÇÃO         │
    │  (linking)       │
    └───────┬──────────┘
            │
    └───────▼──────────┐
        EXECUTÁVEL
```

---

## 🎯 MAPA DO LIVRO DO DRAGÃO

Este documento corresponde aos **Capítulos 1-8** do livro:

| Capítulo | Tópico | Você Cobre |
|----------|--------|-----------|
| 1 | Introdução | ✓ Este plano |
| 2 | Análise Léxica | ✓ Fase 1 + seu .jff |
| 3 | Análise Sintática | ⚠️ Fase 2 (próximo) |
| 4 | Análise Semântica | ⚠️ Fase 3 (próximo) |
| 5 | Geração de Código Intermediário | ⚠️ Fase 4 (próximo) |
| 6 | Geração de Código | ⚠️ Fase 6 (próximo) |
| 7 | Otimização | ⚠️ Fase 5 (opcional) |
| 8 | Tabelas de Símbolos | ⚠️ Fases 2-3 |

---

## 🔄 CICLO PRÁTICO: RESUMO

1. **Fase 1** (Hoje): Analisador Léxico
   - Entrada: `"int x = 10;"`
   - Saída: `[KW_INT, ID, OP_ASSIGN, NUM, SEMICOLON]`

2. **Fase 2** (Próxima semana): Analisador Sintático
   - Entrada: Tokens acima
   - Saída: AST estruturada

3. **Fase 3** (Próxima semana): Análise Semântica
   - Entrada: AST
   - Saída: AST anotada + Tabela de Símbolos

4. **Fase 4** (Semana 3): Geração Intermediária
   - Entrada: AST + Símbolos
   - Saída: TAC

5. **Fase 6** (Semana 3): Geração C
   - Entrada: TAC
   - Saída: `código.c`

6. **Fase 7** (Semana 3): Compilação
   - Input: `código.c`
   - Output: Executável

---

**PRÓXIMA ETAPA**: Vou criar um documento específico para **Análise Léxica Prática**, com código C que você pode começar a implementar. Quer?

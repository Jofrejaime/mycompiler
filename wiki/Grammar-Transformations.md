# 🔄 Transformações Gramaticais

**Eliminação de Recursividade à Esquerda e Fatoração**

---

## 📋 Visão Geral

Para construir um parser **LL(1)** usando **Recursive Descent**, a gramática deve ser transformada para eliminar:

1. **Recursividade à esquerda** (left recursion)
2. **Ambiguidade**
3. **Conflitos FIRST-FIRST**

Este documento descreve as transformações aplicadas à gramática original.

---

## 🚫 Problema: Recursividade à Esquerda

### O que é?

Uma produção é **recursiva à esquerda** se:

```
A → A α | β
```

O não-terminal `A` aparece como **primeiro símbolo** do lado direito.

### Por que é um problema?

Parsers **Recursive Descent** entram em **loop infinito**:

```c
ASTNode* parseA(Parser *parser) {
    // A → A α
    ASTNode *left = parseA(parser);  // ❌ Chamada recursiva infinita!
    // ...
}
```

---

## ✅ Solução: Eliminação de Recursividade à Esquerda

### Algoritmo Geral

**Transformação:**

```
A → A α | β
```

**Torna-se:**

```
A → β A'
A' → α A' | ε
```

### Exemplo 1: Expressões Aditivas

**Antes (recursiva à esquerda):**

```
AdditiveExpr → AdditiveExpr OP_PLUS MultiplicativeExpr
             | AdditiveExpr OP_MINUS MultiplicativeExpr
             | MultiplicativeExpr
```

**Depois (recursividade eliminada):**

```
AdditiveExpr → MultiplicativeExpr AdditiveTail

AdditiveTail → OP_PLUS MultiplicativeExpr AdditiveTail
             | OP_MINUS MultiplicativeExpr AdditiveTail
             | ε
```

### Exemplo 2: Lista de Declarações

**Antes:**

```
DeclList → DeclList Decl
         | Decl
```

**Depois:**

```
DeclList → Decl DeclList'

DeclList' → Decl DeclList'
          | ε
```

---

## 🎯 Transformações Aplicadas

### 1. Expressões (15 níveis)

Todas as expressões binárias foram transformadas:

#### Nível 2: Logical OR

**Antes:**
```
LogicalOrExpr → LogicalOrExpr OP_OR LogicalAndExpr
              | LogicalAndExpr
```

**Depois:**
```
LogicalOrExpr → LogicalAndExpr LogicalOrTail

LogicalOrTail → OP_OR LogicalAndExpr LogicalOrTail
              | ε
```

#### Nível 3: Logical AND

**Antes:**
```
LogicalAndExpr → LogicalAndExpr OP_AND BitwiseOrExpr
               | BitwiseOrExpr
```

**Depois:**
```
LogicalAndExpr → BitwiseOrExpr LogicalAndTail

LogicalAndTail → OP_AND BitwiseOrExpr LogicalAndTail
               | ε
```

#### Nível 10: Aditivas

**Antes:**
```
AdditiveExpr → AdditiveExpr OP_PLUS MultiplicativeExpr
             | AdditiveExpr OP_MINUS MultiplicativeExpr
             | MultiplicativeExpr
```

**Depois:**
```
AdditiveExpr → MultiplicativeExpr AdditiveTail

AdditiveTail → OP_PLUS MultiplicativeExpr AdditiveTail
             | OP_MINUS MultiplicativeExpr AdditiveTail
             | ε
```

#### Nível 11: Multiplicativas

**Antes:**
```
MultiplicativeExpr → MultiplicativeExpr OP_MULT UnaryExpr
                   | MultiplicativeExpr OP_DIV UnaryExpr
                   | MultiplicativeExpr OP_MOD UnaryExpr
                   | UnaryExpr
```

**Depois:**
```
MultiplicativeExpr → UnaryExpr MultiplicativeTail

MultiplicativeTail → OP_MULT UnaryExpr MultiplicativeTail
                   | OP_DIV UnaryExpr MultiplicativeTail
                   | OP_MOD UnaryExpr MultiplicativeTail
                   | ε
```

### 2. Listas

#### Lista de Declarações

**Antes:**
```
DeclList → DeclList Decl | Decl
```

**Depois:**
```
DeclList → Decl DeclList'
DeclList' → Decl DeclList' | ε
```

#### Lista de Parâmetros

**Antes:**
```
ParamList → ParamList SYM_COMMA Param | Param
```

**Depois:**
```
ParamList → Param ParamList'
ParamList' → SYM_COMMA Param ParamList' | ε
```

#### Lista de Argumentos

**Antes:**
```
ArgList → ArgList SYM_COMMA Expr | Expr
```

**Depois:**
```
ArgList → Expr ArgList'
ArgList' → SYM_COMMA Expr ArgList' | ε
```

---

## 🔀 Fatoração à Esquerda

### O que é?

Quando duas produções começam com o mesmo prefixo:

```
A → α β | α γ
```

Isso causa **conflito FIRST-FIRST** em parsers LL(1).

### Solução

**Fatorar o prefixo comum:**

```
A → α A'
A' → β | γ
```

### Exemplo: Declarações

**Antes (prefixo comum):**

```
Decl → Type TK_ID SYM_SEMICOLON
     | Type TK_ID SYM_LPAREN ParamList SYM_RPAREN Block
```

**Depois (fatorado):**

```
Decl → Type TK_ID DeclTail

DeclTail → SYM_SEMICOLON
         | SYM_LPAREN ParamList SYM_RPAREN Block
```

---

## 📊 Resumo das Transformações

| Categoria | Transformações | Resultado |
|-----------|----------------|-----------|
| **Expressões Binárias** | 10 níveis | Recursividade eliminada |
| **Listas** | 5 listas | Recursividade eliminada |
| **Declarações** | 3 tipos | Fatoração aplicada |
| **Statements** | 8 tipos | Sem transformação necessária |

---

## ✅ Verificação LL(1)

Após as transformações, a gramática satisfaz as condições LL(1):

### ✅ Condição 1: Sem Recursividade à Esquerda

Todas as produções recursivas à esquerda foram eliminadas.

### ✅ Condição 2: Sem Conflitos FIRST-FIRST

Para produções alternativas `A → α | β`:
- `FIRST(α) ∩ FIRST(β) = ∅`

**Exemplo:**
```
Stmt → IfStmt | WhileStmt | ForStmt | ...

FIRST(IfStmt) = { KW_IF }
FIRST(WhileStmt) = { KW_WHILE }
FIRST(ForStmt) = { KW_FOR }

✅ Conjuntos disjuntos
```

### ✅ Condição 3: Sem Conflitos FIRST-FOLLOW

Se `ε ∈ FIRST(α)`, então:
- `FIRST(β) ∩ FOLLOW(A) = ∅`

**Exemplo:**
```
AdditiveTail → OP_PLUS MultiplicativeExpr AdditiveTail | ε

FIRST(OP_PLUS ...) = { OP_PLUS }
FOLLOW(AdditiveTail) = { SYM_RPAREN, SYM_SEMICOLON, ... }

✅ Conjuntos disjuntos
```

---

## 🎯 Impacto no Parser

### Antes (com recursividade à esquerda)

```c
// ❌ Loop infinito!
ASTNode* parseAdditiveExpr(Parser *parser) {
    ASTNode *left = parseAdditiveExpr(parser);  // Recursão infinita
    if (match(parser, OP_PLUS)) {
        advance(parser);
        ASTNode *right = parseMultiplicativeExpr(parser);
        return createBinaryNode(OP_PLUS, left, right);
    }
    return left;
}
```

### Depois (recursividade eliminada)

```c
// ✅ Funciona corretamente!
ASTNode* parseAdditiveExpr(Parser *parser) {
    ASTNode *left = parseMultiplicativeExpr(parser);
    return parseAdditiveTail(parser, left);
}

ASTNode* parseAdditiveTail(Parser *parser, ASTNode *left) {
    if (match(parser, OP_PLUS)) {
        advance(parser);
        ASTNode *right = parseMultiplicativeExpr(parser);
        ASTNode *node = createBinaryNode(OP_PLUS, left, right);
        return parseAdditiveTail(parser, node);  // Recursão à direita OK
    }
    else if (match(parser, OP_MINUS)) {
        advance(parser);
        ASTNode *right = parseMultiplicativeExpr(parser);
        ASTNode *node = createBinaryNode(OP_MINUS, left, right);
        return parseAdditiveTail(parser, node);
    }
    return left;  // ε
}
```

---

## 📚 Gramática Transformada Completa

A gramática completa após todas as transformações está disponível em:

**[Gramática Sintática](Syntactic-Grammar.md)**

---

## 🔗 Referências

- **[Gramática Sintática](Syntactic-Grammar.md)** - Gramática LL(1) completa
- **[FIRST Sets](FIRST-Sets.md)** - Conjuntos FIRST
- **[Parser Design](Parser-Design.md)** - Análise detalhada
- **[Parser](Parser.md)** - Visão geral do parser

---

## 📖 Leitura Adicional

### Algoritmos de Transformação

1. **Eliminação de Recursividade à Esquerda Direta**
   - Algoritmo simples para `A → A α | β`

2. **Eliminação de Recursividade à Esquerda Indireta**
   - Algoritmo geral para `A → B α`, `B → A β`

3. **Fatoração à Esquerda**
   - Algoritmo para eliminar prefixos comuns

### Livros Recomendados

1. **"Compilers: Principles, Techniques, and Tools"** (Dragon Book)
   - Seção 4.3: Writing a Grammar
   - Seção 4.4: Top-Down Parsing

2. **"Engineering a Compiler"** (Cooper & Torczon)
   - Capítulo 3: Parsers

---

**Última Atualização:** 24-05-2026  
**Status:** ✅ Completo  
**Versão:** 1.0

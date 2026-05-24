# 🎯 Conjuntos FIRST

**Análise LL(1) - Parsing Preditivo**

---

## 📋 Visão Geral

Os **conjuntos FIRST** são fundamentais para a construção de parsers LL(1) preditivos. Eles determinam qual produção escolher baseado no próximo token de entrada.

### Definição

**FIRST(α)** é o conjunto de terminais que podem aparecer como primeiro símbolo em alguma string derivada de α.

---

## 🎯 Conjuntos FIRST dos Não-Terminais

### Programa e Declarações

```
FIRST(Program) = { KW_INT, KW_CHAR, KW_VOID, KW_FLOAT, KW_DOUBLE, KW_STRUCT }

FIRST(DeclList) = { KW_INT, KW_CHAR, KW_VOID, KW_FLOAT, KW_DOUBLE, KW_STRUCT }

FIRST(Decl) = { KW_INT, KW_CHAR, KW_VOID, KW_FLOAT, KW_DOUBLE, KW_STRUCT }

FIRST(Type) = { KW_INT, KW_CHAR, KW_VOID, KW_FLOAT, KW_DOUBLE }

FIRST(VarDecl) = { KW_INT, KW_CHAR, KW_FLOAT, KW_DOUBLE }

FIRST(FuncDecl) = { KW_INT, KW_CHAR, KW_VOID, KW_FLOAT, KW_DOUBLE }

FIRST(StructDecl) = { KW_STRUCT }
```

### Statements

```
FIRST(Stmt) = { KW_IF, KW_WHILE, KW_FOR, KW_DO, KW_RETURN, 
                KW_BREAK, KW_CONTINUE, KW_SWITCH, SYM_LBRACE, 
                TK_ID, SYM_SEMICOLON }

FIRST(IfStmt) = { KW_IF }

FIRST(WhileStmt) = { KW_WHILE }

FIRST(ForStmt) = { KW_FOR }

FIRST(DoWhileStmt) = { KW_DO }

FIRST(SwitchStmt) = { KW_SWITCH }

FIRST(ReturnStmt) = { KW_RETURN }

FIRST(BreakStmt) = { KW_BREAK }

FIRST(ContinueStmt) = { KW_CONTINUE }

FIRST(Block) = { SYM_LBRACE }

FIRST(ExprStmt) = { TK_ID, TK_NUM_INT, TK_NUM_FLOAT, TK_STRING, 
                    TK_CHAR, SYM_LPAREN, OP_NOT, OP_BITNOT, 
                    OP_INC, OP_DEC, KW_SIZEOF }
```

### Expressões (15 níveis)

```
FIRST(Expr) = { TK_ID, TK_NUM_INT, TK_NUM_FLOAT, TK_STRING, TK_CHAR, 
                SYM_LPAREN, OP_NOT, OP_BITNOT, OP_INC, OP_DEC, 
                KW_SIZEOF, OP_PLUS, OP_MINUS, OP_MULT, OP_BITAND }

FIRST(TernaryExpr) = FIRST(Expr)

FIRST(LogicalOrExpr) = FIRST(Expr)

FIRST(LogicalAndExpr) = FIRST(Expr)

FIRST(BitwiseOrExpr) = FIRST(Expr)

FIRST(BitwiseXorExpr) = FIRST(Expr)

FIRST(BitwiseAndExpr) = FIRST(Expr)

FIRST(EqualityExpr) = FIRST(Expr)

FIRST(RelationalExpr) = FIRST(Expr)

FIRST(ShiftExpr) = FIRST(Expr)

FIRST(AdditiveExpr) = FIRST(Expr)

FIRST(MultiplicativeExpr) = FIRST(Expr)

FIRST(UnaryExpr) = { OP_NOT, OP_BITNOT, OP_INC, OP_DEC, KW_SIZEOF, 
                     OP_PLUS, OP_MINUS, OP_MULT, OP_BITAND, 
                     TK_ID, TK_NUM_INT, TK_NUM_FLOAT, TK_STRING, 
                     TK_CHAR, SYM_LPAREN }

FIRST(PostfixExpr) = { TK_ID, TK_NUM_INT, TK_NUM_FLOAT, TK_STRING, 
                       TK_CHAR, SYM_LPAREN }

FIRST(PrimaryExpr) = { TK_ID, TK_NUM_INT, TK_NUM_FLOAT, TK_STRING, 
                       TK_CHAR, SYM_LPAREN }
```

---

## 🔍 Algoritmo de Cálculo

### Regras para Calcular FIRST

1. **Se X é terminal:** `FIRST(X) = {X}`

2. **Se X → ε:** `ε ∈ FIRST(X)`

3. **Se X → Y₁Y₂...Yₖ:**
   - Adicione `FIRST(Y₁) - {ε}` a `FIRST(X)`
   - Se `ε ∈ FIRST(Y₁)`, adicione `FIRST(Y₂) - {ε}` a `FIRST(X)`
   - Continue até encontrar Yᵢ onde `ε ∉ FIRST(Yᵢ)`
   - Se `ε ∈ FIRST(Yᵢ)` para todo i, adicione `ε` a `FIRST(X)`

### Exemplo de Cálculo

```
Produção: Expr → TernaryExpr

FIRST(Expr) = FIRST(TernaryExpr)

Produção: TernaryExpr → LogicalOrExpr TernaryTail

FIRST(TernaryExpr) = FIRST(LogicalOrExpr)

Produção: LogicalOrExpr → LogicalAndExpr LogicalOrTail

FIRST(LogicalOrExpr) = FIRST(LogicalAndExpr)

...

Produção: PrimaryExpr → TK_ID | TK_NUM_INT | TK_NUM_FLOAT | 
                        TK_STRING | TK_CHAR | SYM_LPAREN Expr SYM_RPAREN

FIRST(PrimaryExpr) = { TK_ID, TK_NUM_INT, TK_NUM_FLOAT, 
                       TK_STRING, TK_CHAR, SYM_LPAREN }
```

---

## 📊 Tabela de Decisão LL(1)

A tabela de decisão usa os conjuntos FIRST para determinar qual produção aplicar:

| Não-Terminal | Token | Produção |
|--------------|-------|----------|
| `Stmt` | `KW_IF` | `Stmt → IfStmt` |
| `Stmt` | `KW_WHILE` | `Stmt → WhileStmt` |
| `Stmt` | `KW_FOR` | `Stmt → ForStmt` |
| `Stmt` | `KW_RETURN` | `Stmt → ReturnStmt` |
| `Stmt` | `SYM_LBRACE` | `Stmt → Block` |
| `Stmt` | `TK_ID` | `Stmt → ExprStmt` |

---

## ✅ Verificação LL(1)

Para que a gramática seja LL(1), deve satisfazer:

### Condição 1: Sem Conflitos FIRST-FIRST

Para produções alternativas `A → α | β`:
- `FIRST(α) ∩ FIRST(β) = ∅`

**Exemplo:**
```
Stmt → IfStmt | WhileStmt | ForStmt | ...

FIRST(IfStmt) = { KW_IF }
FIRST(WhileStmt) = { KW_WHILE }
FIRST(ForStmt) = { KW_FOR }

✅ Sem conflitos: conjuntos disjuntos
```

### Condição 2: Sem Conflitos FIRST-FOLLOW

Se `ε ∈ FIRST(α)`, então:
- `FIRST(β) ∩ FOLLOW(A) = ∅`

**Exemplo:**
```
DeclList' → Decl DeclList' | ε

FIRST(Decl DeclList') = { KW_INT, KW_CHAR, ... }
FOLLOW(DeclList') = { TK_EOF }

✅ Sem conflitos: conjuntos disjuntos
```

---

## 🎯 Uso no Parser

### Função de Decisão

```c
ASTNode* parseStmt(Parser *parser) {
    Token *current = parser->currentToken;
    
    // Usar FIRST para decidir qual produção aplicar
    switch (current->type) {
        case KW_IF:
            return parseIfStmt(parser);
        
        case KW_WHILE:
            return parseWhileStmt(parser);
        
        case KW_FOR:
            return parseForStmt(parser);
        
        case KW_RETURN:
            return parseReturnStmt(parser);
        
        case SYM_LBRACE:
            return parseBlock(parser);
        
        case TK_ID:
        case TK_NUM_INT:
        case TK_NUM_FLOAT:
            return parseExprStmt(parser);
        
        default:
            reportError(parser, "Statement esperado");
            return NULL;
    }
}
```

---

## 📚 Conjuntos FIRST Completos

Para a lista completa de conjuntos FIRST de todos os não-terminais da gramática, consulte:

**Design do Parser:** [Parser-Design.md](Parser-Design.md)  
**Gramática Completa:** [Syntactic-Grammar.md](Syntactic-Grammar.md)

---

## 🔗 Referências

- **[Gramática Sintática](Syntactic-Grammar.md)** - Gramática LL(1) completa
- **[Parser Design](Parser-Design.md)** - Análise FIRST detalhada
- **[Parsing Table](Parsing-Table.md)** - Tabela LL(1) completa
- **[Transformações Gramaticais](Grammar-Transformations.md)** - Eliminação de recursividade

---

## 📖 Leitura Adicional

### Livros Recomendados

1. **"Compilers: Principles, Techniques, and Tools"** (Dragon Book)
   - Capítulo 4: Syntax Analysis
   - Seção 4.4: Top-Down Parsing

2. **"Engineering a Compiler"** (Cooper & Torczon)
   - Capítulo 3: Parsers

3. **"Modern Compiler Implementation in C"** (Appel)
   - Capítulo 3: Parsing

---

**Última Atualização:** 24-05-2026  
**Status:** ✅ Completo  
**Versão:** 1.0

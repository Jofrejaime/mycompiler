# 🌳 Analisador Sintático (Parser)

**Fase 2 do Compilador**  
**Status:** 🚧 Em Desenvolvimento

---

## 📋 Visão Geral

O **Analisador Sintático** (Parser) é a segunda fase do compilador. Sua função é verificar se a sequência de tokens produzida pelo lexer está de acordo com a **gramática da linguagem** e construir a **Árvore Sintática Abstrata (AST)**.

### Responsabilidades

- Verificar a **estrutura sintática** do programa
- Construir a **AST (Abstract Syntax Tree)**
- Enriquecer a **Tabela de Símbolos** com informações de tipo, escopo e endereço
- Gerenciar **escopos hierárquicos** (global, função, bloco)
- Reportar **erros sintáticos** com mensagens claras
- Implementar **recuperação de erros** em modo pânico

---

## 🎯 Método de Parsing

### Top-Down Recursive Descent (LL(1))

O parser utiliza o método **Recursive Descent** sem backtracking:

- **Top-Down:** Constrói a árvore da raiz para as folhas
- **Preditivo:** Usa apenas 1 token de lookahead (LL(1))
- **Sem Backtracking:** Decisões determinísticas baseadas no próximo token
- **Recursivo:** Cada não-terminal é uma função recursiva

### Vantagens

✅ Simples de implementar e entender  
✅ Mensagens de erro claras e específicas  
✅ Fácil de depurar  
✅ Eficiente (O(n) no tamanho da entrada)  
✅ Controle total sobre a construção da AST

---

## 🏗️ Arquitetura

### Estruturas de Dados Principais

#### 1. AST Node (Nó da Árvore Sintática)

```c
typedef struct ASTNode {
    NodeType type;              // Tipo do nó (EXPR, STMT, DECL, etc.)
    Token *token;               // Token associado
    struct ASTNode **children;  // Filhos do nó
    int childCount;             // Número de filhos
    SymbolInfo *symbol;         // Informação do símbolo (se aplicável)
} ASTNode;
```

#### 2. Symbol Table Entry (Entrada da Tabela de Símbolos)

```c
typedef struct SymbolInfo {
    char *name;                 // Nome do identificador
    DataType dataType;          // Tipo de dado (int, float, char, etc.)
    VarType varType;            // Tipo de variável (simples, array, ponteiro)
    int scope;                  // Nível de escopo
    int memoryAddress;          // Endereço de memória
    int sizeInBytes;            // Tamanho em bytes
    void *assignedValue;        // Valor atribuído (se constante)
    struct SymbolInfo *next;    // Próximo na lista (hash table)
} SymbolInfo;
```

#### 3. Scope Stack (Pilha de Escopos)

```c
typedef struct ScopeStack {
    int currentScope;           // Escopo atual (0=global, 1+=local)
    int scopeStack[MAX_SCOPE];  // Pilha de escopos
    int stackTop;               // Topo da pilha
} ScopeStack;
```

---

## 📊 Módulos do Parser

| Módulo | Arquivo | Descrição |
|--------|---------|-----------|
| **Parser Principal** | `parser.c`, `parser.h` | Inicialização e controle |
| **Expressões** | `parser_expr.c` | Parsing de expressões |
| **Statements** | `parser_stmt.c` | Parsing de comandos |
| **Declarações** | `parser_decl.c` | Parsing de declarações |
| **AST** | `ast.c`, `ast.h` | Construção da árvore |
| **Escopos** | `scope.c`, `scope.h` | Gerenciamento de escopos |
| **Tabela de Símbolos** | `symtab.c`, `symtab.h` | Tabela hash de símbolos |

---

## 🔄 Fluxo de Execução

```
┌─────────────┐
│   Lexer     │ → Tokens
└──────┬──────┘
       │
       ▼
┌─────────────┐
│   Parser    │ → AST + Symbol Table
└──────┬──────┘
       │
       ├─→ parser_decl.c  (Declarações)
       ├─→ parser_stmt.c  (Statements)
       └─→ parser_expr.c  (Expressões)
       │
       ▼
┌─────────────┐
│     AST     │ → Árvore Sintática
└─────────────┘
```

---

## 📝 Gramática LL(1)

A gramática utilizada é **LL(1)** com:
- ✅ Recursividade à esquerda eliminada
- ✅ Ambiguidade resolvida
- ✅ Precedência de operadores definida (15 níveis)
- ✅ Associatividade correta

**Documentação completa:** [Gramática Sintática](Syntactic-Grammar.md)

### Exemplo de Produção

```
Program → DeclList

DeclList → Decl DeclList'
DeclList' → Decl DeclList' | ε

Decl → Type TK_ID DeclTail

DeclTail → SYM_SEMICOLON              // Variável
         | SYM_LPAREN ParamList SYM_RPAREN FuncBody  // Função
```

---

## 🎯 Funções de Parsing

Cada não-terminal da gramática é implementado como uma função:

### Declarações

```c
ASTNode* parseProgram(Parser *parser);
ASTNode* parseDeclList(Parser *parser);
ASTNode* parseDecl(Parser *parser);
ASTNode* parseVarDecl(Parser *parser);
ASTNode* parseFuncDecl(Parser *parser);
```

### Statements

```c
ASTNode* parseStmt(Parser *parser);
ASTNode* parseIfStmt(Parser *parser);
ASTNode* parseWhileStmt(Parser *parser);
ASTNode* parseForStmt(Parser *parser);
ASTNode* parseReturnStmt(Parser *parser);
ASTNode* parseBlock(Parser *parser);
```

### Expressões (15 níveis de precedência)

```c
ASTNode* parseExpr(Parser *parser);           // Nível 1: Ternário
ASTNode* parseLogicalOr(Parser *parser);      // Nível 2: ||
ASTNode* parseLogicalAnd(Parser *parser);     // Nível 3: &&
ASTNode* parseBitwiseOr(Parser *parser);      // Nível 4: |
ASTNode* parseBitwiseXor(Parser *parser);     // Nível 5: ^
ASTNode* parseBitwiseAnd(Parser *parser);     // Nível 6: &
ASTNode* parseEquality(Parser *parser);       // Nível 7: ==, !=
ASTNode* parseRelational(Parser *parser);     // Nível 8: <, >, <=, >=
ASTNode* parseShift(Parser *parser);          // Nível 9: <<, >>
ASTNode* parseAdditive(Parser *parser);       // Nível 10: +, -
ASTNode* parseMultiplicative(Parser *parser); // Nível 11: *, /, %
ASTNode* parseUnary(Parser *parser);          // Nível 12: !, ~, ++, --, sizeof
ASTNode* parsePostfix(Parser *parser);        // Nível 13: [], (), ., ->
ASTNode* parsePrimary(Parser *parser);        // Nível 14: id, num, string, (expr)
```

---

## 🔍 Algoritmo de Parsing

### Exemplo: `parseIfStmt()`

```c
ASTNode* parseIfStmt(Parser *parser) {
    // 1. Consumir 'if'
    expect(parser, KW_IF);
    
    // 2. Consumir '('
    expect(parser, SYM_LPAREN);
    
    // 3. Parsear condição
    ASTNode *condition = parseExpr(parser);
    
    // 4. Consumir ')'
    expect(parser, SYM_RPAREN);
    
    // 5. Parsear bloco 'then'
    ASTNode *thenBlock = parseStmt(parser);
    
    // 6. Verificar 'else' opcional
    ASTNode *elseBlock = NULL;
    if (match(parser, KW_ELSE)) {
        advance(parser);
        elseBlock = parseStmt(parser);
    }
    
    // 7. Criar nó AST
    return createIfNode(condition, thenBlock, elseBlock);
}
```

---

## 🗂️ Gerenciamento de Escopos

### Hierarquia de Escopos

```
Escopo 0 (Global)
├── Escopo 1 (Função main)
│   ├── Escopo 2 (Bloco if)
│   └── Escopo 3 (Bloco while)
└── Escopo 4 (Função foo)
    └── Escopo 5 (Bloco for)
```

### Operações

```c
void enterScope(ScopeStack *stack);    // Entrar em novo escopo
void exitScope(ScopeStack *stack);     // Sair do escopo atual
int getCurrentScope(ScopeStack *stack); // Obter escopo atual
```

### Exemplo

```c
int x = 10;           // Escopo 0 (global)

void main() {         // Escopo 1
    int y = 20;       // Escopo 1
    
    if (x > 5) {      // Escopo 2
        int z = 30;   // Escopo 2
    }
    
    while (y > 0) {   // Escopo 3
        int w = 40;   // Escopo 3
    }
}
```

---

## 📋 Tabela de Símbolos

### Estrutura (Hash Table)

```c
#define SYMBOL_TABLE_SIZE 256

typedef struct SymbolTable {
    SymbolInfo *buckets[SYMBOL_TABLE_SIZE];
    int count;
} SymbolTable;
```

### Operações

```c
void insertSymbol(SymbolTable *table, SymbolInfo *symbol);
SymbolInfo* lookupSymbol(SymbolTable *table, char *name, int scope);
void deleteSymbolsInScope(SymbolTable *table, int scope);
```

### Informações Armazenadas

| Campo | Descrição | Exemplo |
|-------|-----------|---------|
| `name` | Nome do identificador | `"x"` |
| `dataType` | Tipo de dado | `TYPE_INT` |
| `varType` | Tipo de variável | `VAR_SIMPLE` |
| `scope` | Nível de escopo | `1` |
| `memoryAddress` | Endereço de memória | `0x1000` |
| `sizeInBytes` | Tamanho em bytes | `4` |
| `assignedValue` | Valor atribuído | `10` |

---

## 🛠️ Tratamento de Erros

### Tipos de Erros Sintáticos

| Erro | Descrição | Exemplo |
|------|-----------|---------|
| **Token inesperado** | Token não esperado | `if x > 5)` (falta `(`) |
| **Declaração inválida** | Sintaxe incorreta | `int ;` |
| **Expressão mal formada** | Operadores/operandos incorretos | `x + * y` |
| **Bloco não fechado** | Falta `}` | `if (x) { y = 1;` |

### Formato de Erro

```
ERRO SINTÁTICO [linha:coluna]: mensagem
Esperado: <token esperado>
Encontrado: <token atual>

Exemplo:
ERRO SINTÁTICO [15:10]: Token inesperado
Esperado: SYM_SEMICOLON (;)
Encontrado: SYM_RPAREN ())
```

### Recuperação de Erros (Modo Pânico)

1. Detectar erro
2. Reportar erro ao usuário
3. Sincronizar no próximo ponto seguro (`;`, `}`, etc.)
4. Continuar parsing

---

## 📁 Arquivos do Parser

```
src/parser/
├── parser.c             # Função principal parse()
├── parser.h             # Interface pública
├── parser_decl.c        # Parsing de declarações
├── parser_stmt.c        # Parsing de statements
├── parser_expr.c        # Parsing de expressões
├── ast.c                # Construção da AST
├── ast.h                # Estruturas da AST
├── scope.c              # Gerenciamento de escopos
├── scope.h              # Interface de escopos
├── symtab.c             # Tabela de símbolos
└── symtab.h             # Interface da tabela
```

---

## 🧪 Testes

### Casos de Teste

1. **Declarações:** `int x;`, `float y = 3.14;`, `int arr[10];`
2. **Funções:** `int main() { return 0; }`
3. **Expressões:** `x + y * z`, `a && b || c`, `x > 5 ? 10 : 20`
4. **Statements:** `if`, `while`, `for`, `return`, `break`, `continue`
5. **Escopos:** Variáveis locais e globais
6. **Erros:** Tokens inesperados, declarações inválidas

### Executar Testes

```bash
make test-parser
```

---

## 📚 Especificações Técnicas

- **[Requisitos do Parser](Parser-Requirements.md)** - 20 requisitos detalhados
- **[Design do Parser](Parser-Design.md)** - Arquitetura completa
- **[Plano de Implementação](Parser-Tasks.md)** - 24 tarefas organizadas

---

## 📖 Referências

- [Gramática Sintática](Syntactic-Grammar.md)
- [Tabela de Tokens](Token-Table.md)
- [Lexer](Lexer.md)
- [Manual do Programador](Manual-Programador.md)
- [FIRST Sets](FIRST-Sets.md)
- [Transformações Gramaticais](Grammar-Transformations.md)

---

**Implementação:** C99  
**Método:** Top-Down Recursive Descent (LL(1))  
**Status:** 🚧 Em Desenvolvimento

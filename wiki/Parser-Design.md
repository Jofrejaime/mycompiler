# 🏗️ Design do Parser

**Arquitetura Técnica Completa - Fase 2 do Compilador**

---

## 📍 Localização

Este documento faz referência ao design técnico completo do Parser localizado em:

```
.kiro/specs/syntactic-analyzer/design.md
```

---

## 📖 Visão Geral

O documento de design contém a **arquitetura técnica completa** do Analisador Sintático (Parser), incluindo:

### Conteúdo do Design

1. **Arquitetura Geral**
   - Diagrama de componentes
   - Fluxo de dados
   - Interação entre módulos

2. **Estruturas de Dados** (40+ definições)
   - `Parser` - Estrutura principal
   - `ASTNode` - Nós da árvore sintática
   - `SymbolInfo` - Entrada da tabela de símbolos
   - `ScopeStack` - Pilha de escopos
   - Enums: `NodeType`, `DataType`, `VarType`, `OperatorType`

3. **Análise da Gramática**
   - Verificação LL(1)
   - Eliminação de recursividade à esquerda
   - Resolução de ambiguidade
   - Precedência de operadores (15 níveis)

4. **Conjuntos FIRST**
   - FIRST sets para todos os não-terminais
   - Análise de conflitos
   - Validação LL(1)

5. **Assinaturas de Funções** (40+ funções)
   - Funções de parsing para cada não-terminal
   - Funções auxiliares
   - Funções de construção da AST
   - Funções de gerenciamento de escopos
   - Funções da tabela de símbolos

6. **Algoritmos Principais**
   - Algoritmo de parsing recursivo descendente
   - Construção da AST
   - Gerenciamento de escopos
   - Tratamento de erros

7. **Diagramas**
   - Diagrama de arquitetura
   - Fluxo de execução
   - Hierarquia de escopos
   - Estrutura da AST

---

## 🎯 Destaques do Design

### Estrutura Principal do Parser

```c
typedef struct Parser {
    Lexer *lexer;              // Analisador léxico
    Token *currentToken;       // Token atual
    Token *nextToken;          // Próximo token (lookahead)
    ASTNode *root;             // Raiz da AST
    SymbolTable *symbolTable;  // Tabela de símbolos
    ScopeStack *scopeStack;    // Pilha de escopos
    ErrorList *errors;         // Lista de erros
    int panicMode;             // Modo de recuperação de erros
} Parser;
```

### Hierarquia de Nós da AST

```
ASTNode (base)
├── ProgramNode
├── DeclNode
│   ├── VarDeclNode
│   ├── FuncDeclNode
│   └── StructDeclNode
├── StmtNode
│   ├── IfStmtNode
│   ├── WhileStmtNode
│   ├── ForStmtNode
│   ├── ReturnStmtNode
│   └── BlockNode
└── ExprNode
    ├── BinaryExprNode
    ├── UnaryExprNode
    ├── TernaryExprNode
    └── PrimaryExprNode
```

### Precedência de Operadores (15 níveis)

| Nível | Operadores | Associatividade | Função |
|-------|-----------|-----------------|---------|
| 1 | `? :` | Direita | `parseExpr()` |
| 2 | `\|\|` | Esquerda | `parseLogicalOr()` |
| 3 | `&&` | Esquerda | `parseLogicalAnd()` |
| 4 | `\|` | Esquerda | `parseBitwiseOr()` |
| 5 | `^` | Esquerda | `parseBitwiseXor()` |
| 6 | `&` | Esquerda | `parseBitwiseAnd()` |
| 7 | `==`, `!=` | Esquerda | `parseEquality()` |
| 8 | `<`, `>`, `<=`, `>=` | Esquerda | `parseRelational()` |
| 9 | `<<`, `>>` | Esquerda | `parseShift()` |
| 10 | `+`, `-` | Esquerda | `parseAdditive()` |
| 11 | `*`, `/`, `%` | Esquerda | `parseMultiplicative()` |
| 12 | `!`, `~`, `++`, `--`, `sizeof` | Direita | `parseUnary()` |
| 13 | `[]`, `()`, `.`, `->` | Esquerda | `parsePostfix()` |
| 14 | Primários | - | `parsePrimary()` |

---

## 📊 Estatísticas do Design

| Métrica | Valor |
|---------|-------|
| **Estruturas de Dados** | 10+ |
| **Enumerações** | 5+ |
| **Funções de Parsing** | 40+ |
| **Níveis de Precedência** | 15 |
| **Não-Terminais** | 60+ |
| **Produções Gramaticais** | 80+ |

---

## 🔗 Acesso ao Design Completo

Para visualizar o design técnico completo, acesse:

**Caminho relativo:** `../.kiro/specs/syntactic-analyzer/design.md`

**Caminho absoluto:** 
```
c:\Users\BeeFlex Studio\Documents\ISPTEC\3 º ano\2º Semestre\compiladores\mycompiler\.kiro\specs\syntactic-analyzer\design.md
```

---

## 📚 Documentos Relacionados

- **[Requisitos do Parser](Parser-Requirements.md)** - Especificação de requisitos
- **[Plano de Implementação](Parser-Tasks.md)** - Tarefas organizadas
- **[Parser](Parser.md)** - Visão geral do parser
- **[Gramática Sintática](Syntactic-Grammar.md)** - Gramática LL(1)
- **[FIRST Sets](FIRST-Sets.md)** - Conjuntos FIRST
- **[Transformações Gramaticais](Grammar-Transformations.md)** - Eliminação de recursividade

---

## 🛠️ Como Usar Este Design

1. **Estude a arquitetura geral** - Entenda os componentes e suas interações
2. **Revise as estruturas de dados** - Compreenda as definições de tipos
3. **Analise a gramática** - Verifique as produções e FIRST sets
4. **Consulte as assinaturas de funções** - Use como referência na implementação
5. **Siga os algoritmos** - Implemente conforme especificado

---

## 🎓 Conceitos Importantes

### LL(1) Parser

- **L**: Left-to-right (leitura da esquerda para direita)
- **L**: Leftmost derivation (derivação mais à esquerda)
- **(1)**: 1 token de lookahead

### Recursive Descent

- Cada não-terminal é uma função recursiva
- Decisões baseadas no próximo token (lookahead)
- Sem backtracking (determinístico)

### AST vs Parse Tree

- **Parse Tree**: Representa todas as derivações da gramática
- **AST**: Representa apenas a estrutura essencial do programa
- AST é mais compacta e útil para fases posteriores

---

**Última Atualização:** 24-05-2026  
**Status:** ✅ Completo  
**Versão:** 1.0

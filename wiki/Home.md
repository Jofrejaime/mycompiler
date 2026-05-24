# 📚 Wiki - Compilador C Simplificado

**Projeto Acadêmico - ISPTEC**  
**Disciplina:** Compiladores  
**Ano Letivo:** 2025/2026  
**Deadline:** 20-05-2026 | **Defesa:** 21-05-2026

---

## 🎯 Visão Geral do Projeto

Este projeto implementa um **compilador completo** para um subconjunto da linguagem C, desenvolvido em C, seguindo a metodologia de construção de compiladores em fases.

### Fases do Compilador

| Fase | Nome | Status | Documentação |
|------|------|--------|--------------|
| **Fase 1** | Análise Léxica (Lexer) | ✅ Completo | [Lexer](Lexer.md) |
| **Fase 2** | Análise Sintática (Parser) | 🚧 Em Desenvolvimento | [Parser](Parser.md) |
| **Fase 3** | Análise Semântica | ⏳ Pendente | - |
| **Fase 4** | Geração de Código Intermediário | ⏳ Pendente | - |
| **Fase 5** | Otimização | ⏳ Pendente | - |
| **Fase 6** | Geração de Código Final | ⏳ Pendente | - |

---

## 📖 Documentação Técnica

### 🔤 Análise Léxica
- **[Tabela de Tokens e Lexemas](Token-Table.md)** - 82+ tipos de tokens reconhecidos
- **[Autômato Finito](Lexer.md#automato)** - Máquina de estados do lexer
- **[Palavras-Chave](Token-Table.md#palavras-chave)** - 27 keywords do C

### 🌳 Análise Sintática
- **[Gramática Sintática](Syntactic-Grammar.md)** - Gramática EBNF LL(1) completa
- **[Transformações Gramaticais](Grammar-Transformations.md)** - Eliminação de recursividade
- **[FIRST Sets](FIRST-Sets.md)** - Conjuntos FIRST para parsing preditivo
- **[Tabela de Parsing](Parsing-Table.md)** - Tabela LL(1) completa

### 📋 Especificações
- **[Requisitos do Parser](Parser-Requirements.md)** - 20 requisitos detalhados
- **[Design do Parser](Parser-Design.md)** - Arquitetura e estruturas de dados
- **[Plano de Implementação](Parser-Tasks.md)** - 24 tarefas organizadas

---

## 📚 Manuais

### 👤 Manual do Usuário
- **[Manual do Usuário (MU)](Manual-Usuario.md)** - Como usar o compilador
- **[Exemplos de Uso](MU-Examples.md)** - Programas C de exemplo
- **[Mensagens de Erro](MU-Errors.md)** - Interpretação de erros

### 👨‍💻 Manual do Programador
- **[Manual do Programador (MP)](Manual-Programador.md)** - Arquitetura interna
- **[Estruturas de Dados](MP-Data-Structures.md)** - AST, Symbol Table, Scopes
- **[Algoritmos](MP-Algorithms.md)** - Pseudocódigo das funções principais
- **[Guia de Contribuição](MP-Contributing.md)** - Como contribuir para o projeto

---

## 🗂️ Estrutura do Projeto

```
mycompiler/
├── src/
│   ├── lexer/          # Fase 1: Análise Léxica
│   │   ├── lexer.c
│   │   ├── lexer.h
│   │   ├── tokens.c
│   │   ├── tokens.h
│   │   └── keywords.c
│   ├── parser/         # Fase 2: Análise Sintática
│   │   ├── parser.c
│   │   ├── parser.h
│   │   ├── parser_expr.c
│   │   ├── parser_stmt.c
│   │   ├── parser_decl.c
│   │   ├── ast.c
│   │   ├── ast.h
│   │   ├── scope.c
│   │   └── scope.h
│   └── utils/          # Utilitários
│       └── printer.c
├── wiki/               # Documentação (você está aqui!)
├── docs/               # Documentos auxiliares
├── automato/           # Autômato do lexer
├── Makefile            # Build system
└── README.md           # Introdução ao projeto
```

---

## 🚀 Quick Start

### Compilar o Projeto
```bash
make clean
make all
```

### Executar o Compilador
```bash
./compiler input.c -o output
```

### Executar Testes
```bash
make test
```

---

## 📊 Estatísticas do Projeto

| Métrica | Valor |
|---------|-------|
| **Tokens Reconhecidos** | 82+ tipos |
| **Palavras-Chave** | 27 |
| **Produções Gramaticais** | 60+ |
| **Requisitos** | 20 |
| **Tarefas de Implementação** | 24 |
| **Linhas de Código (estimado)** | ~5.000 |

---

## 🔗 Links Rápidos

### Documentação Essencial
- [Tabela de Tokens](Token-Table.md)
- [Gramática Sintática](Syntactic-Grammar.md)
- [Manual do Usuário](Manual-Usuario.md)
- [Manual do Programador](Manual-Programador.md)

### Especificações Técnicas
- [Requisitos do Parser](Parser-Requirements.md)
- [Design do Parser](Parser-Design.md)
- [Plano de Tarefas](Parser-Tasks.md)

### Guias de Referência
- [FIRST Sets](FIRST-Sets.md)
- [Transformações Gramaticais](Grammar-Transformations.md)
- [Estruturas de Dados](MP-Data-Structures.md)

---

## 👥 Equipe

**Desenvolvedor:** [Seu Nome]  
**Instituição:** ISPTEC  
**Curso:** Engenharia Informática  
**Disciplina:** Compiladores  
**Professor:** [Nome do Professor]

---

## 📅 Cronograma

| Data | Milestone |
|------|-----------|
| ✅ 15-03-2026 | Fase 1 (Lexer) Completa |
| 🚧 20-05-2026 | Fase 2 (Parser) - Entrega |
| 🎯 21-05-2026 | Defesa do Projeto |

---

## 📝 Notas

- Este projeto segue o método **Top-Down Recursive Descent** sem backtracking
- A gramática é **LL(1)** - parsing preditivo
- Implementação em **C puro** (C99)
- Compatível com **GCC** e **Clang**

---

**Última Atualização:** 24-05-2026  
**Versão da Wiki:** 1.0  
**Status:** 🚧 Em Construção

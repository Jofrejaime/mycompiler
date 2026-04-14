# 📖 MANUAL DO USUÁRIO - ANALISADOR LÉXICO

**Instituto Superior Politécnico de Tecnologias e Ciências (ISPTEC)**  
**Disciplina:** Compiladores - M3  
**Professor:** André Filemon  
**Aluno:** Jofre Jaime Fernando Muite  
**Data:** 14/04/2026

---

## 📑 Índice

1. [Visão Geral](#visão-geral)
2. [Requisitos do Sistema](#requisitos-do-sistema)
3. [Compilação](#compilação)
4. [Como Usar](#como-usar)
5. [Formatos de Entrada e Saída](#formatos-de-entrada-e-saida)
6. [Exemplos Prácticos](#exemplos-prácticos)
7. [Tratamento de Erros](#tratamento-de-erros)
8. [Perguntas Frequentes](#perguntas-frequentes)

---

## 🎯 Visão Geral

O **Analisador Léxico** é a primeira fase de um compilador. Este programa lê um arquivo-fonte em C e:

1. **Reconhece tokens** - Unidades sintáticas significativas (palavras-chave, identificadores, números, etc.)
2. **Cria lexemas** - String correspondente a cada token
3. **Gera tabela de símbolos** - Lista ordenada de todos os tokens encontrados

### O que o Programa Faz

```
┌─────────────────────┐
│  ARQUIVO FONTE (C)  │
│    programa.c       │
└──────────┬──────────┘
           │
      [ANALEX]
           │
    ┌──────┴──────┐
    │              │
    ▼              ▼
┌─────────┐   ┌──────────────────┐
│ TOKENS  │   │ TABELA SÍMBOLOS  │
│ & LEXEMAS   │ (arquivo .txt)   │
└─────────┘   └──────────────────┘
```

---

## 💻 Requisitos do Sistema

### Mínimo Recomendado
- **CPU:** Qualquer processador moderno
- **RAM:** 512 MB
- **Disco:** 10 MB (para código + dados de teste)
- **SO:** Linux, macOS, Windows (com Git Bash ou WSL)

### Compilador C
- **GCC** 5.0 ou superior (recomendado)
- Alternativas: Clang, MSVC

### Ferramentas Necessárias
- `gcc` - Compilador C
- `make` - Automação de compilação
- `bash` - Shell (para scripts)

### Verificar Instalação

```bash
$ gcc --version
gcc (GCC) 9.3.0

$ make --version
GNU Make 4.2.1
```

---

## 🔨 Compilação

### Pré-requisitos
1. Ter o código-fonte do analisador léxico
2. Ter GCC e Make instalados

### Passos de Compilação

#### Opção 1: Usando Make (Recomendado)

```bash
# Entrar no diretório do projeto
$ cd mycompiler

# Compilar
$ make

# Ou com mais detalhe
$ make all
```

**Saída esperada:**
```
📝 Compilando main.c...
📝 Compilando src/lexer/lexer.c...
📝 Compilando src/lexer/tokens.c...
📝 Compilando src/lexer/keywords.c...
🔗 Linkando analisador_lexico...
✓ Executável criado: analisador_lexico
```

#### Opção 2: Compilação Manual

```bash
$ gcc -Wall -Wextra -std=c99 -I. -O2 \
    main.c \
    src/lexer/lexer.c \
    src/lexer/tokens.c \
    src/lexer/keywords.c \
    -o analisador_lexico
```

#### Compilação com Debug

```bash
$ make debug
```

---

## 🚀 Como Usar

### Formato Básico

```bash
$ ./analisador_lexico <arquivo_entrada> [arquivo_saida]
```

### Parâmetros

| Parâmetro | Descrição | Obrigatório |
|-----------|-----------|---|
| `arquivo_entrada` | Caminho do arquivo C a analisar | Sim |
| `arquivo_saida` | Onde salvar resultado (default: `tabela_simbolos.txt`) | Não |

### Exemplos de Uso

#### Exemplo 1: Analisar arquivo simples

```bash
$ ./analisador_lexico programa.c
```

**Saída:**
- Exibe tabela de símbolos no console
- Salva em `tabela_simbolos.txt`

#### Exemplo 2: Especificar arquivo de saída

```bash
$ ./analisador_lexico programa.c resultado.txt
```

#### Exemplo 3: Processar arquivo de teste

```bash
$ make run
```

(Compila e processa `teste_entrada.c` automaticamente)

---

## 📊 Formatos de Entrada e Saída

### Formato de Entrada

O **arquivo de entrada** deve ser:
- Um arquivo de texto com extensão `.c`
- Contendo código válido em C (ou subconjunto)
- Codificado em UTF-8 (recomendado) ou ASCII

**Exemplo de entrada válida:**

```c
// teste.c
int main() {
    int x = 10;
    printf("Olá, mundo!\n");
    return 0;
}
```

### Formato de Saída

A **tabela de símbolos** contém:

```
╔════════════════════════════════╗
║   TABELA DE SÍMBOLOS (TOKENS)  ║
╠════════════════════════════════╣
║ Nº  │ Tipo  │ Lexema │ Linha │
║ 1   │ KW_INT│ int    │ 1     │
║ 2   │ ID    │ main   │ 1     │
║ 3   │ LPAREN│ (      │ 1     │
║ ... │ ...   │ ...    │ ...   │
╚════════════════════════════════╝
```

**Colunas da tabela:**
- **Nº** - Número sequencial do token
- **Tipo** - Tipo de token (KW_INT, ID, NUM_INT, STRING, etc.)
- **Lexema** - String original do código-fonte
- **Linha** - Linha onde o token foi encontrado
- **Coluna** - Coluna inicial do token
- **Valor** (opcional) - Valor numérico para constantes

---

## 💡 Exemplos Prácticos

### Exemplo 1: Programa Simples

**Arquivo: `simples.c`**
```c
int x = 5;
```

**Comando:**
```bash
$ ./analisador_lexico simples.c
```

**Saída esperada:**
```
Token 1: KW_INT      "int"      linha 1, coluna 1
Token 2: ID          "x"        linha 1, coluna 5
Token 3: OP_ASSIGN   "="        linha 1, coluna 7
Token 4: NUM_INT     "5"        linha 1, coluna 9
Token 5: SEMICOLON   ";"        linha 1, coluna 10
```

### Exemplo 2: Números em Notação Científica

**Arquivo: `numeros.c`**
```c
float a = 3.14;
double b = 1.5e-3;
```

**Comando:**
```bash
$ ./analisador_lexico numeros.c
```

**Saída esperada:**
```
Token 1: KW_FLOAT    "float"    linha 1, coluna 1
Token 2: ID          "a"        linha 1, coluna 7
Token 3: OP_ASSIGN   "="        linha 1, coluna 9
Token 4: NUM_FLOAT   "3.14"     linha 1, coluna 11
...
Token 8: NUM_EXP     "1.5e-3"   linha 2, coluna 13
```

### Exemplo 3: Comentários (Ignorados)

**Arquivo: `comentarios.c`**
```c
// Comentário de linha
int x; /* Comentário bloco */
```

**Comportamento:**
- Comentários são ignorados (não geram tokens)
- Apenas `int`, `x`, `;` são tokenizados

---

## ⚠️ Tratamento de Erros

### Erro: Arquivo não encontrado

```
❌ ERRO: Não foi possível abrir arquivo 'programa.c'
```

**Solução:** Verificar se o arquivo existe e o caminho está correto

```bash
$ ls -la programa.c  # Listar o arquivo
```

### Erro: Erro léxico

```
⚠️  ERRO LÉXICO na linha 5, coluna 12: '$'
```

**Solução:** Verificar caracteres inválidos no código-fonte. O símbolo `$` não é válido em C.

### Erro: Permissões

```
❌ ERRO: Não foi possível abrir arquivo
```

**Solução:** Verificar permissões do arquivo

```bash
$ chmod 644 programa.c  # Dar permissão de leitura
```

---

## ❓ Perguntas Frequentes

### P: Qual é a diferença entre "token" e "lexema"?

**R:** 
- **Token**: Tipo (ex: ID, NUM_INT, KW_IF)
- **Lexema**: String original (ex: "variavel", "123", "if")

### P: O analisador processa todas as fases do compilador?

**R:** Não. Processa apenas a **Fase 1 (Análise Léxica)**. As fases 2-7 (sintática, semântica, etc.) não são abordadas.

### P: Qual é o tamanho máximo de um arquivo?

**R:** Teórico: Ilimitado. Prático: Limitado pela RAM disponível (buffer interno de 1KB).

### P: Como adicionar mais palavras-chave?

**R:** Editar arquivo `src/lexer/keywords.c` e adicionar à tabela (veja Manual do Programador).

### P: Posso compilar no Windows?

**R:** Sim! Use:
- **Git Bash** + MinGW
- **Visual Studio Code** com extensão C/C++
- **WSL** (Windows Subsystem for Linux)

### P: O analisador suporta Unicode?

**R:** Parcialmente. Identificadores unicode não são suportados. Use apenas ASCII padrão.

### P: Como são tratadas as diretivas de pré-processador? ⭐ NOVO

**R:** Desde a v2.1, o analisador reconhece **tokens de pré-processador**:
- `#include <stdio.h>` → `TK_PP_INCLUDE`
- `#define MAX 100` → `TK_PP_DEFINE`
- `#ifdef DEBUG` → `TK_PP_IFDEF`
- `#ifndef RELEASE` → `TK_PP_IFNDEF`
- `#endif` → `TK_PP_ENDIF`
- `#pragma pack(1)` → `TK_PP_PRAGMA`
- `#...` (outras) → `TK_PP_OTHER`

**Funcionamento:** Um novo estado FSM (Q35) acumula caracteres da diretiva inteira (de `#` até `\n`) e a classifica conforme seu tipo.

### P: A diretiva `#` ainda é reportada como erro?

**R:** Não mais! Antes era `TK_ERROR`. Agora é tokenizado corretamente como um dos 7 tipos de pré-processador.

---

## 🔗 Referências

- **Livro:** "Compiladores - Princípios, Técnicas e Ferramentas" (Aho, Lam, Sethi, Ullman)
- **JFLAP:** Ferramenta visual para autômatos finitos
- **Padrão C:** ISO/IEC 9899:2018

---

## 📞 Suporte

Para dúvidas ou relatórios de bugs:
- **Email do Professor:** compiladores2023@gmail.com
- **Disciplina:** Compiladores - M3
- **Turno:** [Conforme seu horário]

---

**Data de Entrega:** 14/04/2026  
**Data de Defesa:** 16/04/2026

---

## ✅ Checklist de Uso

- [ ] Compilei o projeto com `make`
- [ ] Criei arquivo de entrada com código C
- [ ] Executei `./analisador_lexico arquivo.c`
- [ ] Consultei a tabela de símbolos
- [ ] Verifiquei erros léxicos (se houver)

---

**FIM DO MANUAL DO USUÁRIO**

# 📚 MANUAL DO PROGRAMADOR - ANALISADOR LÉXICO

**Instituto Superior Politécnico de Tecnologias e Ciências (ISPTEC)**  
**Disciplina:** Compiladores - M3  
**Professor:** André Filemon  
**Aluno:** Jofre Jaime Jamuinda Muto
**Data:** 14/04/2026

---

## 📑 Índice

1. [Introdução](#introdução)
2. [Arquitetura Geral](#arquitetura-geral)
3. [Tokens Definidos](#tokens-definidos)
4. [Expressões Regulares](#expressões-regulares)
5. [Autômato Finito Determinístico](#autômato-finito-determinístico)
6. [Estruturas de Dados](#estruturas-de-dados)
7. [Funções Obrigatórias](#funções-obrigatórias)
8. [Pseudocódigo](#pseudocódigo)
9. [Fluxo de Execução](#fluxo-de-execução)
10. [Como Estender](#como-estender)

---

## 🎓 Introdução

Este manual documenta a implementação do **Analisador Léxico** conforme o enunciado de "Análise Léxica (Primeira Fase de um Compilador)".

### Requisitos Implementados

✅ **Função `analex()`** - Implementa autômatos finitos determinísticos  
✅ **Função `ler_caractere()`** - Leitura caractere por caractere  
✅ **Função `volta_caractere()`** - Retraçamento (OUTRO no autômato)  
✅ **Função `gravar_token_lexema()`** - Guardar na tabela de símbolos  
✅ **Máquina de Estados** - Conceito obrigatório implementado  
✅ **Tabela de Símbolos** - Estrutura SQLite-like para tokens  

### Tecnologia

- **Linguagem:** C99
- **Compilador:** GCC
- **Build System:** Make
- **Padrão:** Conforme enunciado do Professor André Filemon

---

## 🏗️ Arquitetura Geral

### Estrutura de Diretórios

```
mycompiler/
├── main.c                      # Programa principal
├── Makefile                    # Build automation
├── teste_entrada.c             # Arquivo de teste
├── mu.md                       # Manual do Usuário
├── mp.md                       # Este arquivo
├── CompiladorAutomatos.jff     # Autômato visual (JFLAP)
└── src/lexer/
    ├── lexer.h                 # Interface do lexer
    ├── lexer.c                 # Implementação (máquina de estados)
    ├── tokens.h                # Definição de tipos de token
    ├── tokens.c                # Implementação de tabela de símbolos
    ├── keywords.h              # Interface de keywords
    └── keywords.c              # Tabela de palavras-chave
```

### Fluxo de Dados

```
┌──────────────────┐
│  main.c          │
│                  │
│  • Abre arquivo  │
│  • Cria lexer    │
│  • Chama analex  │
└────────┬─────────┘
         │
         ↓
    ┌────────────────┐
    │  lexer.c       │──→ keywords.c (procurar keyword)
    │                │
    │  • ler_caracte │
    │  • volta_carac │
    │  • máquina de  │
    │    estados     │
    └────────┬───────┘
             │
             ↓
    ┌────────────────┐
    │  tokens.c      │
    │                │
    │  • criar token │
    │  • tabla símb  │
    │  • imprimir    │
    └────────────────┘
```

---

## 🎫 Tokens Definidos

### Palavras-Chave (27 total)

```c
#define KW_INT          1    /* int */
#define KW_CHAR         2    /* char */
#define KW_VOID         3    /* void */
#define KW_FLOAT        4    /* float */
#define KW_DOUBLE       5    /* double */
#define KW_IF           6    /* if */
#define KW_ELSE         7    /* else */
#define KW_WHILE        8    /* while */
#define KW_FOR          9    /* for */
#define KW_DO           10   /* do */
#define KW_BREAK        11   /* break */
#define KW_CONTINUE     12   /* continue */
#define KW_RETURN       13   /* return */
#define KW_STRUCT       14   /* struct */
#define KW_UNION        15   /* union */
#define KW_TYPEDEF      16   /* typedef */
#define KW_SIZEOF       17   /* sizeof */
#define KW_STATIC       18   /* static */
#define KW_EXTERN       19   /* extern */
#define KW_CONST        20   /* const */
#define KW_VOLATILE     21   /* volatile */
#define KW_SWITCH       22   /* switch */
#define KW_CASE         23   /* case */
#define KW_DEFAULT      24   /* default */
#define KW_GOTO         25   /* goto */
```

### Identificadores e Constantes (6 total)

```c
#define TK_ID           50   /* Identificador: [a-zA-Z_][a-zA-Z0-9_]* */
#define TK_NUM_INT      51   /* Número inteiro: [0-9]+ */
#define TK_NUM_FLOAT    52   /* Número float: [0-9]+\.[0-9]+ */
#define TK_NUM_EXP      53   /* Notação científica */
#define TK_STRING       54   /* String literal: "..." */
#define TK_CHAR         55   /* Character literal: '.' */
```

### Operadores Aritméticos (5 total)

```c
#define OP_PLUS         100  /* + */
#define OP_MINUS        101  /* - */
#define OP_MULT         102  /* * */
#define OP_DIV          103  /* / */
#define OP_MOD          104  /* % */
```

### Operadores de Atribuição (5 total)

```c
#define OP_ASSIGN       110  /* = */
#define OP_PLUS_ASSIGN  111  /* += */
#define OP_MINUS_ASSIGN 112  /* -= */
#define OP_MULT_ASSIGN  113  /* *= */
#define OP_DIV_ASSIGN   114  /* /= */
```

### Operadores Relacionais (6 total)

```c
#define OP_EQ           120  /* == */
#define OP_NE           121  /* != */
#define OP_LT           122  /* < */
#define OP_LE           123  /* <= */
#define OP_GT           124  /* > */
#define OP_GE           125  /* >= */
```

### Operadores Lógicos (3 total)

```c
#define OP_AND          130  /* && */
#define OP_OR           131  /* || */
#define OP_NOT          132  /* ! */
```

### Operadores Bitwise (6 total)

```c
#define OP_BITAND       140  /* & */
#define OP_BITOR        141  /* | */
#define OP_BITXOR       142  /* ^ */
#define OP_BITNOT       143  /* ~ */
#define OP_LSHIFT       144  /* << */
#define OP_RSHIFT       145  /* >> */
```

### Operadores Unários (3 total)

```c
#define OP_INC          150  /* ++ */
#define OP_DEC          151  /* -- */
#define OP_ARROW        152  /* -> */
```

### Símbolos Especiais (6 total)

```c
#define SYM_LPAREN      200  /* ( */
#define SYM_RPAREN      201  /* ) */
#define SYM_LBRACE      202  /* { */
#define SYM_RBRACE      203  /* } */
#define SYM_LBRACKET    204  /* [ */
#define SYM_RBRACKET    205  /* ] */
```

### Pontuação (5 total)

```c
#define SYM_SEMICOLON   210  /* ; */
#define SYM_COMMA       211  /* , */
#define SYM_DOT         212  /* . */
#define SYM_COLON       213  /* : */
#define SYM_TERNARY     214  /* ? */
```

### Especiais (2 total)

```c
#define TK_EOF          300  /* Fim de arquivo */
#define TK_ERROR        301  /* Erro léxico */
```

**Total: ~75 tipos de tokens**

---

## 📝 Expressões Regulares

### Padrões Léxicos Reconhecidos

```regex
IDENTIFICADOR      := [a-zA-Z_][a-zA-Z0-9_]*
NUM_INT            := [0-9]+
NUM_FLOAT          := [0-9]+\.[0-9]+
NUM_EXP            := [0-9]+(e|E)[+-]?[0-9]+
STRING             := "[^"\\]*(\.[^"\\]*)*"
CHAR               := '[^'\\]|.'
OP_COMPOSTOS       := == | != | <= | >= | << | >> | && | \|\| |
                      ++ | -- | += | -= | *= | /= | ->
COMENTÁRIO_LINHA   := //[^\n]*
COMENTÁRIO_BLOCO   := /\*(.|\n)*?\*/
ESPAÇO             := [ \t\r\n]+
```

---

## 🤖 Autômato Finito Determinístico

### Diagrama Simplificado

```
q0 (INICIAL)
 ├─[a-zA-Z_]──→ q1 ──[a-zA-Z0-9_]*──→ q1 ──[OUTRO]──↷
 │                                                      │
 │                                             ┌────────┘
 │                                             ↓
 │                                    ESTADO FINAL (ID)
 │
 ├─[0-9]──→ q2 ──[0-9]*──→ q2 ──┬─[.]──→ q3 ──[0-9]+──→ q3 ──[OUTRO]──↷
 │                               │                            │
 │                               │                  ┌─────────┘
 │                               │                  ↓
 │                               └──→────[OUTRO]────→ FINAL (NUM_INT)
 │                                      │
 │                                      └─→ FINAL (NUM_FLOAT)
 │
 ├─["]──→ q4 ──[^"]──→ q4 ──["]──→ FINAL (STRING)
 │
 ├─[']──→ q5 ──[^']──→ q5 ──[']──→ FINAL (CHAR)
 │
 ├─[/]──→ q6 ──[/]──→ PULA_LINHA
 │       └─[*]──→ PULA_BLOCO
 │
 └─[+,-,*,/,=,<,>,!,&,|,^,~]──→ VERIFICA_COMPOSTO ──→ FINAL (OP)
 │
 └─[({[]──→ FINAL (SÍMBOLO)
 
OUTRO: qualquer caractere que encerre o padrão atual
```

### Estados Completos (q0-q79)

Conforme arquivo `CompiladorAutomatos.jff`:
- **q0:** Estado inicial
- **q1-q5:** Reconhecimento de identificadores
- **q10-q15:** Reconhecimento de números
- **q20-q25:** Reconhecimento de strings
- **q30-q35:** Reconhecimento de caracteres
- **q40-q50:** Transições de operadores
- **q60-q79:** Estados adicionais para símbolos especiais

---

## 🗂️ Estruturas de Dados

### Estrutura: Token

```c
typedef struct {
    int tipo;                   /* Tipo: KW_INT, ID, NUM_INT, etc. */
    char lexeme[256];           /* String: "int", "variavel", "123" */
    int linha;                  /* Linha no código-fonte */
    int coluna;                 /* Coluna no código-fonte */
    union {
        int valor_int;          /* Valor numérico para inteiros */
        double valor_float;     /* Valor numérico para floats */
    } valor;
} Token;
```

### Estrutura: Lexer (Máquina de Estados)

```c
typedef struct {
    FILE *arquivo_entrada;              /* Arquivo-fonte */
    char buffer[1024];                  /* Buffer de leitura */
    int pos_buffer;                     /* Posição no buffer */
    int tamanho_buffer;                 /* Dados efetivos no buffer */
    
    int linha, coluna;                  /* Posição atual */
    int linha_inicio_lexema;            /* Início do lexema */
    int coluna_inicio_lexema;           /* Coluna inicio */
    
    char lexema[256];                   /* Acumulador */
    int pos_lexema;                     /* Posição no lexema */
    
    TablaSimbolos *tabla_simbolos;      /* Tabela de símbolos */
    
    char caractere_retratado;           /* Backtracking */
    int teve_retracao;                  /* Flag retraçamento */
    
    int estado_atual;                   /* Estado autômato */
} Lexer;
```

### Estrutura: TablaSimbolos

```c
typedef struct {
    Token *tokens;              /* Array dinâmico de tokens */
    int quantidade;             /* Tokens adicionados */
    int capacidade;             /* Tamanho do array */
    int posicao_leitura;        /* Para iteração */
} TablaSimbolos;
```

---

## 📋 Funções Obrigatórias

### 1. analex() - ANALISADOR LÉXICO PRINCIPAL

**Assinatura:**
```c
Token analex(Lexer *lexer);
```

**Responsabilidade:**
- Implementa máquina de estados (autômatos finitos determinísticos)
- Lê caracteres através de `ler_caractere()`
- Retrocede quando necessário via `volta_caractere()`
- Cria tokens através de `gravar_token_lexema()`
- Pula espaços e comentários
- Retorna Token com tipo e lexema

**Entrada:** Lexer inicializado e aberto

**Saída:** Token encontrado

---

### 2. ler_caractere() - LEITURA CARACTERE POR CARACTERE

**Assinatura:**
```c
char ler_caractere(Lexer *lexer);
```

**Responsabilidade:**
- Lê um caractere do arquivo-fonte
- Mantém linha e coluna atualizados
- Usa buffer interno (1KB) para eficiência
- Retorna '\0' se EOF
- Consulta flag de retraçamento

**Entrada:** Lexer

**Saída:** Próximo caractere ou '\0'

---

### 3. volta_caractere() - RETRAÇAMENTO (BACKTRACKING)

**Assinatura:**
```c
void volta_caractere(Lexer *lexer, char c);
```

**Responsabilidade:**
- Armazena caractere lido demais
- Define flag de retraçamento
- Ajusta linha/coluna retroativamente
- Próxima `ler_caractere()` retorna este caractere

**Entrada:** Lexer e caractere a retroceder

**Saída:** Void

**Nota:** Usada quando o autômato lê "OUTRO" (caractere fora do padrão)

---

### 4. gravar_token_lexema() - GUARDAR TABELA DE SÍMBOLOS

**Assinatura:**
```c
Token gravar_token_lexema(Lexer *lexer, int tipo, const char *lexema, 
                          int linha, int coluna);
```

**Responsabilidade:**
- Cria novo Token com tipo e lexema
- Extrai valor numérico se aplicável
- Adiciona à `tabla_simbolos->tokens[]`
- Expande tabela se necessário
- Retorna Token criado

**Entrada:** Lexer, tipo, lexema, linha, coluna

**Saída:** Token criado

---

## 💻 Pseudocódigo

### Pseudocódigo: analex()

```
FUNÇÃO analex(lexer):
    
    // ETAPA 1: Pular espaços e comentários
    ENQUANTO verdade:
        c ← ler_caractere(lexer)
        
        SE c == EOF:
            RETORNAR Token(TK_EOF)
        FIM SE
        
        SE eh_espacobrancho(c) OU c == '\n':
            CONTINUAR  // ignorar
        FIM SE
        
        SE c == '/' E prox(lexer) == '/':
            PULAR_COMENTARIO_LINHA()
            CONTINUAR
        FIM SE
        
        SE c == '/' E prox(lexer) == '*':
            PULAR_COMENTARIO_BLOCO()
            CONTINUAR
        FIM SE
        
        // Encontrou caractere significativo
        QUEBRAR
    FIM ENQUANTO
    
    // ETAPA 2: Máquina de Estados
    lexer->linha_inicio ← lexer->linha
    lexer->coluna_inicio ← lexer->coluna
    lexer->lexema[0] ← c
    lexer->pos_lexema ← 1
    
    // Reconhecer Identificadores
    SE eh_letra(c) OU c == '_':
        ENQUANTO verdade:
            c ← ler_caractere(lexer)
            
            SE eh_letra(c) OU eh_digito(c):
                ADICIONAR_AO_LEXEMA(c)
            SENÃO:
                volta_caractere(lexer, c)
                QUEBRAR
            FIM SE
        FIM ENQUANTO
        
        tipo ← procurar_palavra_chave(lexer->lexema)
        RETORNAR gravar_token_lexema(lexer, tipo, ...)
    FIM SE
    
    // Reconhecer Números
    SE eh_digito(c):
        tem_ponto ← falso
        
        ENQUANTO verdade:
            c ← ler_caractere(lexer)
            
            SE eh_digito(c):
                ADICIONAR_AO_LEXEMA(c)
            SENÃO SE c == '.' E NÃO tem_ponto:
                tem_ponto ← verdade
                ADICIONAR_AO_LEXEMA(c)
            SENÃO:
                volta_caractere(lexer, c)
                QUEBRAR
            FIM SE
        FIM ENQUANTO
        
        tipo ← (tem_ponto ? TK_NUM_FLOAT : TK_NUM_INT)
        RETORNAR gravar_token_lexema(lexer, tipo, ...)
    FIM SE
    
    // Reconhecer Strings
    SE c == '"':
        ENQUANTO (c ← ler_caractere) ≠ '"' E c ≠ EOF:
            SE c == '\\':
                ADICIONAR_AO_LEXEMA(c)
                c ← ler_caractere()
                ADICIONAR_AO_LEXEMA(c)
            SENÃO:
                ADICIONAR_AO_LEXEMA(c)
            FIM SE
        FIM ENQUANTO
        
        RETORNAR gravar_token_lexema(lexer, TK_STRING, ...)
    FIM SE
    
    // ... similmente para operadores e símbolos
    
    // Erro léxico
    SE nenhum padrão conseguiu:
        RETORNAR gravar_token_lexema(lexer, TK_ERROR, ...)
    FIM SE
    
FIM FUNÇÃO
```

### Pseudocódigo: ler_caractere()

```
FUNÇÃO ler_caractere(lexer):
    
    // Se houver retraçamento anterior
    SE lexer->teve_retracao:
        lexer->teve_retracao ← falso
        RETORNAR lexer->caractere_retratado
    FIM SE
    
    // Se buffer vazio, carregar novo bloco
    SE lexer->pos_buffer >= lexer->tamanho_buffer:
        lexer->tamanho_buffer ← fread(buffer, 1, 1024, arquivo)
        
        SE tamanho == 0:
            RETORNAR '\0'  // EOF
        FIM SE
        
        lexer->pos_buffer ← 0
    FIM SE
    
    // Ler caractere do buffer
    c ← lexer->buffer[lexer->pos_buffer++]
    
    // Atualizar linha/coluna
    SE c == '\n':
        lexer->linha++
        lexer->coluna ← 1
    SENÃO SE c ≠ '\r':
        lexer->coluna++
    FIM SE
    
    RETORNAR c
    
FIM FUNÇÃO
```

### Pseudocódigo: volta_caractere()

```
FUNÇÃO volta_caractere(lexer, c):
    
    lexer->caractere_retratado ← c
    lexer->teve_retracao ← verdade
    
    // Ajustar posição
    SE c == '\n':
        lexer->linha--
    SENÃO:
        lexer->coluna--
    FIM SE
    
FIM FUNÇÃO
```

### Pseudocódigo: gravar_token_lexema()

```
FUNÇÃO gravar_token_lexema(lexer, tipo, lexema, linha, coluna):
    
    // Criar novo token
    token.tipo ← tipo
    token.lexeme ← CÓPIA(lexema)
    token.linha ← linha
    token.coluna ← coluna
    
    // Extrair valor se numérico
    SE tipo == TK_NUM_INT:
        token.valor.valor_int ← atoi(lexema)
    SENÃO SE tipo == TK_NUM_FLOAT OU tipo == TK_NUM_EXP:
        token.valor.valor_float ← atof(lexema)
    FIM SE
    
    // Adicionar à tabela de símbolos
    SE lexer->tabla_simbolos != NULL:
        adicionar_token(lexer->tabla_simbolos, token)
    FIM SE
    
    RETORNAR token
    
FIM FUNÇÃO
```

---

## 🔄 Fluxo de Execução

### Sequência Completa: main.c → analex()

```
1. main.c criado_lexer()
        │
        ├─→ Abre arquivo
        ├─→ Aloca Lexer struct
        └─→ Cria TablaSimbolos (100 tokens iniciais)

2. main.c chamada analex() EM LOOP
        │
        ├─→ analex() começa no q0 (estado inicial)
        │
        ├─→ ler_caractere() retorna 'i' (de "int")
        │
        ├─→ eh_letra('i') = verdade
        │
        ├─→ Loop: acumula 'n', 't'
        │
        ├─→ ler_caractere() retorna ' ' (espaço)
        │
        ├─→ volta_caractere(lexer, ' ')
        │
        ├─→ procurar_palavra_chave("int")
        │   └─→ Retorna KW_INT
        │
        └─→ gravar_token_lexema(lexer, KW_INT, "int", 1, 1)
                │
                ├─→ Cria Token struct
                ├─→ Adiciona à tabla_simbolos
                └─→ Retorna Token(KW_INT, "int", 1, 1)

3. main.c imprime tabela
        │
        ├─→ imprimir_tabla_simbolos()
        │   └─→ Loop sobre tokens[], exibe formatado
        │
        └─→ Salva em arquivo_saida

4. main.c libera recursos
        │
        ├─→ liberar_lexer()
        │   ├─→ fclose() arquivo
        │   ├─→ liberar_tabla_simbolos()
        │   └─→ free() Lexer
        │
        └─→ Fim do programa
```

---

## 🔧 Como Estender

### Adicionar Novo Tipo de Token

#### Passo 1: Definir constante em tokens.h

```c
#define KW_UNSIGNED     26   /* unsigned */
```

#### Passo 2: Adicionar à tabela de keywords em keywords.c

```c
{"unsigned",    KW_UNSIGNED},
```

#### Passo 3: Adicionar string de debug em tokens.c

```c
case KW_UNSIGNED: return "KW_UNSIGNED";
```

#### Passo 4: Atualizar máquina de estados em lexer.c

```c
if (strcmp(lexer->lexema, "...") == 0) tipo = KW_UNSIGNED;
```

### Adicionar Novo Padrão Léxico

Exemplo: Números hexadecimais (0xABCD)

#### Passo 1: Adicionar tipo em tokens.h

```c
#define TK_NUM_HEX      56   /* Número hexadecimal */
```

#### Passo 2: Reconhecer em analex() em lexer.c

```c
if (c == '0' && (prox == 'x' || prox == 'X')) {
    // Ler dígitos hexadecimais
    while (eh_digito_hex(c = ler_caractere(lexer))) {
        ADICIONAR_LEXEMA(c);
    }
    volta_caractere(lexer, c);
    return gravar_token_lexema(lexer, TK_NUM_HEX, ...);
}
```

---

## 📊 Tabela de Resumo

| Aspecto | Valor |
|---------|-------|
| **Total de Tokens** | ~75 tipos |
| **Palavras-Chave** | 27 |
| **Constantes** | 6 |
| **Operadores** | 28 |
| **Símbolos** | 11 |
| **Especiais** | 2 |
| **Tamanho Máximo Lexema** | 256 caracteres |
| **Tamanho Buffer Entrada** | 1024 bytes |
| **Estados Autômato** | q0 a q79 (80 estados) |
| **Funções Obrigatórias** | 4 |
| **Funções Suplementares** | 8+ |

---

## 📖 Referências Técnicas

1. **"Compiladores - Princípios, Técnicas e Ferramentas"**
   - Autores: Aho, Lam, Sethi, Ullman
   - Capítulos: 2-3 (Análise Léxica)

2. **ISO/IEC 9899:2018 - Linguagem C**
   - Standard oficial da linguagem C
   - Seções: Lexical Elements

3. **JFLAP - Java Formal Languages and Automata Package**
   - Ferramenta: http://www.jflap.org/
   - Uso: Design visual do autômato

---

## ✅ Checklist de Desenvolvimento

- [x] Definir todos os tokens
- [x] Criar expressões regulares
- [x] Desenhar autômato finito
- [x] Implementar `ler_caractere()`
- [x] Implementar `volta_caractere()`
- [x] Implementar `analex()` com máquina de estados
- [x] Implementar `gravar_token_lexema()`
- [x] Criar tabela de símbolos
- [x] Implementar keywords lookup
- [x] Testes com múltiplos tipos de tokens
- [x] Tratamento de erros léxicos
- [x] Manual do usuário
- [x] Manual do programador

---

**FIM DO MANUAL DO PROGRAMADOR**

*Última Atualização: 14/04/2026*  
*Versão: 1.0*

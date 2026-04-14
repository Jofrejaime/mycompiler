# 🎯 ANALISADOR LÉXICO - FASE 1 DO COMPILADOR

**Instituto Superior Politécnico de Tecnologias e Ciências (ISPTEC)**  
**Disciplina:** Compiladores - M3  
**Professor:** André Filemon  
**Aluno:** Jofre Jaime Jamuinda Mutumbungo 
**Data de Entrega:** 14/04/2026  
**Data de Defesa:** 16/04/2026

---

## 📋 Conteúdo do Projeto

### 📁 Estrutura de Diretórios

```
mycompiler/
│
├── 📄 README.md                              ← Este arquivo
├── 🔨 Makefile                               ← Build automation
├── 🔨 build.sh                               ← Script de compilação
│
├── 📖 MANUAIS
│   ├── mu.md                                 ← Manual do Usuário (8 páginas)
│   ├── mp.md                                 ← Manual do Programador (20+ páginas)
│   └── ARQUITETURA_COMPILADOR.md             ← Guia de Arquitetura
│
├── 📚 TEORIA E PLANEJAMENTO
│   ├── PLANO_COMPILADOR_COMPLETO.md          ← Plano das 7 fases
│   ├── AS_7_FASES_COMPILACAO.md              ← Detalhamento por fase
│   ├── GUIA_COMPLETO_COMPILADOR.md           ← Guia de leitura
│   ├── RESUMO_VISUAL.md                      ← Mapa mental visual
│   └── enunciado_fase1.txt                   ← Enunciado do professor
│
├── 🔧 CÓDIGO-FONTE
│   ├── main.c                                ← Programa principal
│   ├── teste_entrada.c                       ← Arquivo de teste
│   │
│   └── src/lexer/
│       ├── lexer.h                           ← Interface do lexer
│       ├── lexer.c                           ← Implementação (máquina de estados)
│       ├── tokens.h                          ← Definição de tokens
│       ├── tokens.c                          ← Tabela de símbolos
│       ├── keywords.h                        ← Palavras-chave
│       └── keywords.c                        ← Lookup de keywords
│
├── 📊 AUTÔMATO
│   └── CompiladorAutomatos.jff               ← Autômato visual (JFLAP)
│
├── 📤 SAÍDAS
│   ├── analisador_lexico                     ← Executável compilado
│   └── tabela_simbolos.txt                   ← Resultado da análise
│
└── 📋 DOCUMENTAÇÃO ADICIONAL
    ├── INDICE.md                             ← Índice geral
    ├── ANALISE_AUTOMATO.md                   ← Análise do autômato
    ├── REORGANIZACAO_ESTADOS.md              ← Reorganização de estados
    ├── DIAGRAMA_TRANSICOES.md                ← Diagramas de transição
    ├── GUIA_PRATICO_CORRECOES.md             ← Guia prático
    └── ESTRUTURA_OTIMIZADA.md                ← Estrutura ideal
```

---

## ✨ Características Implementadas

### ✅ Requisitos Obrigatórios do Professor

- [x] **analex()** - Função que implementa autômatos finitos determinísticos
- [x] **ler_caractere()** - Leitura caractere por caractere
- [x] **volta_caractere()** - Retraçamento (OUTRO no autômato)
- [x] **gravar_token_lexema()** - Guardar na tabela de símbolos
- [x] **Máquina de Estados** - Implementada conforme seu autômato .jff
- [x] **Tabela de Símbolos** - Estrutura dinâmica com tokens e lexemas
- [x] **Manual do Usuário** - mu.md (8 páginas)
- [x] **Manual do Programador** - mp.md (20+ páginas)
- [x] **Autômato Testar** - CompiladorAutomatos.jff validado

### ⭐ Funcionalidades Extras

- [x] 75+ tipos de tokens (palavras-chave, operadores, símbolos, etc.)
- [x] Reconhecimento de números (inteiro, float, notação científica)
- [x] Reconhecimento de strings e character literals
- [x] Comentários de linha (//) e bloco (/* */)
- [x] Rastreamento de linha e coluna de cada token
- [x] Tratamento de erros léxicos
- [x] Tabela de símbolos com formatação visual
- [x] Saída em arquivo de texto
- [x] Build automation com Makefile
- [x] Script de compilação (build.sh)

---

## 🚀 Como Usar

### Requisitos

- GCC (compilador C)
- Linux, macOS ou Windows (com Git Bash)
- ~50 MB de espaço em disco

### Passo 1: Compilar

#### Opção A: Com Make (Recomendado)
```bash
cd mycompiler
make
```

#### Opção B: Com script bash
```bash
cd mycompiler
bash build.sh
```

#### Opção C: Manualmente
```bash
gcc -Wall -std=c99 -I. -O2 \
    main.c \
    src/lexer/lexer.c \
    src/lexer/tokens.c \
    src/lexer/keywords.c \
    -o analisador_lexico
```

### Passo 2: Executar

#### Analisar arquivo de teste
```bash
./analisador_lexico teste_entrada.c
```

#### Analisar seu próprio arquivo
```bash
./analisador_lexico seu_programa.c saida.txt
```

#### Resultado
- Exibição no console: Tabela de símbolos formatada
- Arquivo gerado: `tabela_simbolos.txt`

---

## 📊 Resultado de Teste

```
Arquivo analisado: teste_entrada.c
Total de tokens analisados: 479
Erros léxicos encontrados: 1 (# do preprocessador)
Linhas do arquivo: 82

✓ Análise concluída com SUCESSO!
```

---

## 🎫 Tokens Implementados

### Categorias

| Categoria | Quantidade | Exemplos |
|-----------|-----------|----------|
| **Palavras-chave** | 27 | int, char, if, while, return, ... |
| **Identificadores** | 1 | variavel, funcao, x, ... |
| **Constantes** | 6 | NUM_INT, NUM_FLOAT, STRING, ... |
| **Operadores** | 28 | +, -, *, ==, <=, &&, ... |
| **Símbolos** | 11 | (, ), {, }, ;, :, ... |
| **Especiais** | 2 | EOF, ERROR |
| **TOTAL** | **75+** | |

---

## 📖 Leitura Recomendada

1. **Primeiro:** [mu.md](mu.md) - Manual do Usuário (15 min)
2. **Segundo:** [mp.md](mp.md) - Manual do Programador (60 min)
3. **Terceiro:** [PLANO_COMPILADOR_COMPLETO.md](PLANO_COMPILADOR_COMPLETO.md) - Teoria (30 min)
4. **Opcional:** Outros documentos conforme necessário

---

## 🔍 Exemplo de Saída

### Entrada (teste_entrada.c)
```c
int fibonacci(int n) {
    if (n <= 1)
        return n;
    else
        return fibonacci(n - 1) + fibonacci(n - 2);
}
```

### Saída (tabela_simbolos.txt)
```
Nº   │ Tipo                │ Lexema         │ Linha │ Coluna
─────┼─────────────────────┼────────────────┼───────┼─────────
  1  │ KW_INT              │ int            │     1 │     1
  2  │ IDENTIFICADOR       │ fibonacci      │     1 │     5
  3  │ SYM_LPAREN ())      │ (              │     1 │    13
  4  │ KW_INT              │ int            │     1 │    14
  5  │ IDENTIFICADOR       │ n              │     1 │    17
  6  │ SYM_RPAREN ())      │ )              │     1 │    17
  7  │ SYM_LBRACE ({)      │ {              │     1 │    19
  8  │ KW_IF               │ if             │     2 │     6
  9  │ SYM_LPAREN ())      │ (              │     2 │     8
 10  │ IDENTIFICADOR       │ n              │     2 │     9
 11  │ OP_LE (<=)          │ <=             │     2 │    10
 12  │ NUM_INT             │ 1              │     2 │    13
...
```

---

## 🐛 Tratamento de Erros

### Erro Léxico Detectado

Quando o analisador encontra um caractere inválido:
```
⚠️  ERRO LÉXICO na linha 4, coluna 2: '#'
```

Os erros são:
1. **Reportados** no console
2. **Registrados** na tabela de símbolos
3. **Identificáveis** com linha e coluna exate

### Erros Conhecidos

- `#` (preprocessador) - Não implementado (fora do escopo C puro)
- Unicode em identificadores - Não suportado (ASCII apenas)

---

## 🎓 Conceitos Implementados

✅ **Autômato Finito Determinístico (DFA)**  
✅ **Máquina de Estados**  
✅ **Expressões Regulares**  
✅ **Tabela de Símbolos**  
✅ **Tabela de Palavras-chave (Lookup)**  
✅ **Buffer de Entrada (1KB)**  
✅ **Backtracking/Retraçamento**  
✅ **Rastreamento de Posição (linha/coluna)**  
✅ **Tratamento de Erros Léxicos**  
✅ **Análise de Comentários**

---

## 📋 Checklist de Entrega

- [x] Código-fonte completo em C
- [x] Autômato finito determinístico (CompiladorAutomatos.jff)
- [x] Funções obrigatórias (analex, ler_caractere, volta_caractere, gravar_token_lexema)
- [x] Máquina de estados implementada
- [x] Tabela de símbolos funcional
- [x] Manual do Usuário (mu.md)
- [x] Manual do Programador (mp.md)
- [x] Expressões regulares documentadas
- [x] Pseudocódigo de funções
- [x] Arquivo de teste (teste_entrada.c)
- [x] Compilação sem erros
- [x] Executável funcional (analisador_lexico)
- [x] Saída formatada  (tabela_simbolos.txt)

---

## 📞 Informações de Entrega

**Professor:** André Filemon  
**Email:** compiladores2023@gmail.com  
**Disciplina:** Compiladores - M3  
**Data Limite:** 14/04/2026 às 23:50  
**Defesa:** 16/04/2026

---

## 📚 Referências

1. **Livro:** "Compiladores - Princípios, Técnicas e Ferramentas"
   - Aho, Lam, Sethi, Ullman
   - Capítulos 2-3: Análise Léxica

2. **Padrão C:** ISO/IEC 9899:2018

3. **Ferramenta JFLAP:** http://www.jflap.org/

---

## 💡 Dicas Práticas

### Para Testar com Seus Arquivos

```bash
./analisador_lexico seu_arquivo.c resultado.txt
cat resultado.txt
```

### Para Compilar com Debug

```bash
gcc -g -O0 main.c src/lexer/*.c -o analisador_lexico
gdb ./analisador_lexico
```

### Para Estender com Novos Tokens

Ver seção "Como Estender" em [mp.md](mp.md)

---

## ✅ Status do Projeto

```
Fase 1: ANÁLISE LÉXICA
├─ Projeto: ✓ COMPLETO
├─ Código: ✓ FUNCIONANDO
├─ Testes: ✓ APROVADOS
├─ Manuais: ✓ DOCUMENTADOS
└─ Entrega: ✓ PRONTA
```

---

## 🎉 Conclusão

Parabéns! Você tem um **analisador léxico completo** e funcional para a linguagem C!

### Próximas Fases (não abordadas aqui)

- Fase 2: Análise Sintática (Parser)
- Fase 3: Análise Semântica
- Fase 4: Geração de Código Intermediário
- Fase 5: Otimização
- Fase 6: Geração de Código C
- Fase 7: Compilação e Linking

---

**Desenvolvido com ❤️ para a disciplina de Compiladores**  
**ISPTEC - 14/04/2026**

---

**FIM DO README**

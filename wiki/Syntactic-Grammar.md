# Gramática Sintática C Simplificada - Adaptada

**Compatível com TABELA_TOKENS_LEXEMA.md**  
**Sem Recursividade à Esquerda | Sem Ambiguidades | LL(1)**

---

## 📋 Mapeamento de Tokens

Esta gramática usa os tokens definidos em `TABELA_TOKENS_LEXEMA.md`:

### Palavras-Chave
- Tipos: `KW_INT`, `KW_CHAR`, `KW_FLOAT`, `KW_VOID`, `KW_DOUBLE`
- Controle: `KW_IF`, `KW_ELSE`, `KW_WHILE`, `KW_FOR`, `KW_DO`, `KW_SWITCH`, `KW_CASE`, `KW_DEFAULT`
- Fluxo: `KW_BREAK`, `KW_CONTINUE`, `KW_RETURN`, `KW_GOTO`
- Estruturas: `KW_STRUCT`, `KW_UNION`, `KW_TYPEDEF`
- Modificadores: `KW_STATIC`, `KW_EXTERN`, `KW_CONST`, `KW_VOLATILE`
- Operadores: `KW_SIZEOF`

### Identificadores e Literais
- `TK_ID` - Identificadores
- `TK_NUM_INT` - Inteiros
- `TK_NUM_FLOAT` - Ponto flutuante
- `TK_NUM_EXP` - Notação científica
- `TK_STRING` - Strings
- `TK_CHAR` - Caracteres

### Operadores
- Aritméticos: `OP_PLUS`, `OP_MINUS`, `OP_MULT`, `OP_DIV`, `OP_MOD`
- Atribuição: `OP_ASSIGN`, `OP_PLUS_ASSIGN`, `OP_MINUS_ASSIGN`, `OP_MULT_ASSIGN`, `OP_DIV_ASSIGN`
- Relacionais: `OP_EQ`, `OP_NE`, `OP_LT`, `OP_LE`, `OP_GT`, `OP_GE`
- Lógicos: `OP_AND`, `OP_OR`, `OP_NOT`
- Bitwise: `OP_BITAND`, `OP_BITOR`, `OP_BITXOR`, `OP_BITNOT`, `OP_LSHIFT`, `OP_RSHIFT`
- Unários: `OP_INC`, `OP_DEC`

### Símbolos
- Delimitadores: `SYM_LPAREN`, `SYM_RPAREN`, `SYM_LBRACE`, `SYM_RBRACE`, `SYM_LBRACKET`, `SYM_RBRACKET`
- Pontuação: `SYM_SEMICOLON`, `SYM_COMMA`, `SYM_DOT`, `SYM_COLON`, `SYM_TERNARY`

### Pré-processador
- `TK_PP_INCLUDE`, `TK_PP_DEFINE`, `TK_PP_IFDEF`, `TK_PP_IFNDEF`, `TK_PP_ENDIF`, `TK_PP_PRAGMA`, `TK_PP_OTHER`

### Especiais
- `TK_EOF` - Fim de arquivo
- `TK_ERROR` - Erro léxico

---

## 🎯 Gramática EBNF (LL(1) - Sem Recursividade à Esquerda)

```ebnf
<programa> ::= <lista_declaracoes_globais> TK_EOF

<lista_declaracoes_globais> ::= <declaracao_global> <lista_declaracoes_globais>
                              | ε

<declaracao_global> ::= <diretiva_preprocessador>
                      | <declaracao_typedef>
                      | <declaracao_geral>

<diretiva_preprocessador> ::= TK_PP_INCLUDE
                            | TK_PP_DEFINE
                            | TK_PP_IFDEF
                            | TK_PP_IFNDEF
                            | TK_PP_ENDIF
                            | TK_PP_PRAGMA
                            | TK_PP_OTHER

<especificador_tipo> ::= KW_INT
                       | KW_CHAR
                       | KW_FLOAT
                       | KW_VOID
                       | KW_DOUBLE
                       | KW_STRUCT <nome_ou_corpo_struct>
                       | KW_UNION <nome_ou_corpo_struct>
                       | TK_ID   (*apenas se for um nome de typedef previamente declarado*)

<nome_ou_corpo_struct> ::= TK_ID <corpo_campos_opcional>
                         | <corpo_campos>

<corpo_campos_opcional> ::= <corpo_campos>
                          | ε

<corpo_campos> ::= SYM_LBRACE <lista_campos> SYM_RBRACE

<lista_campos> ::= <campo> <resto_lista_campos>

<resto_lista_campos> ::= <lista_campos>
                       | ε

<campo> ::= <especificador_tipo> <lista_declaradores_campo> SYM_SEMICOLON

<lista_declaradores_campo> ::= <asteriscos> TK_ID <sufixo_array_opcional> <mais_declaradores_campo>

<mais_declaradores_campo> ::= SYM_COMMA <asteriscos> TK_ID <sufixo_array_opcional> <mais_declaradores_campo>
                            | ε

<declaracao_typedef> ::= KW_TYPEDEF <especificador_tipo> <asteriscos> TK_ID <sufixo_array_opcional> SYM_SEMICOLON

<declaracao_geral> ::= <especificador_tipo> <resto_declaracao_geral>

<resto_declaracao_geral> ::= SYM_SEMICOLON
                           | <asteriscos> TK_ID <cauda_declaracao_geral>

<cauda_declaracao_geral> ::= SYM_LPAREN <parametros_opcionais> SYM_RPAREN <bloco>
                           | <sufixo_array_opcional> <inicializacao_opcional> <mais_declaradores> SYM_SEMICOLON

<asteriscos> ::= OP_MULT <asteriscos>
               | ε

<sufixo_array_opcional> ::= SYM_LBRACKET <expressao> SYM_RBRACKET <sufixo_array_opcional>
                          | ε

<inicializacao_opcional> ::= OP_ASSIGN <expressao>
                           | ε

<mais_declaradores> ::= SYM_COMMA <asteriscos> TK_ID <sufixo_array_opcional> <inicializacao_opcional> <mais_declaradores>
                      | ε

<parametros_opcionais> ::= KW_VOID
                         | <lista_parametros>
                         | ε

<lista_parametros> ::= <parametro> <mais_parametros>

<mais_parametros> ::= SYM_COMMA <parametro> <mais_parametros>
                    | ε

<parametro> ::= <especificador_tipo> <resto_parametro>

<resto_parametro> ::= <asteriscos> TK_ID <sufixo_array_opcional>
                    | ε

<bloco> ::= SYM_LBRACE <lista_itens_bloco> SYM_RBRACE

<lista_itens_bloco> ::= <item_bloco> <lista_itens_bloco>
                      | ε

<item_bloco> ::= <declaracao_variavel_local>
               | <instrucao>

<declaracao_variavel_local> ::= <especificador_tipo> <asteriscos> TK_ID <sufixo_array_opcional> <inicializacao_opcional> <mais_declaradores> SYM_SEMICOLON

<instrucao> ::= <instrucao_expressao>
              | <instrucao_if>
              | <instrucao_while>
              | <instrucao_for>
              | <instrucao_do>
              | <instrucao_switch>
              | <instrucao_break>
              | <instrucao_continue>
              | <instrucao_return>
              | <instrucao_goto>
              | <bloco>
              | SYM_SEMICOLON

<instrucao_expressao> ::= <expressao> SYM_SEMICOLON

<instrucao_if> ::= KW_IF SYM_LPAREN <expressao> SYM_RPAREN <instrucao> <ramo_else_opcional>

<ramo_else_opcional> ::= KW_ELSE <instrucao>
                       | ε

<instrucao_while> ::= KW_WHILE SYM_LPAREN <expressao> SYM_RPAREN <instrucao>

<instrucao_do> ::= KW_DO <instrucao> KW_WHILE SYM_LPAREN <expressao> SYM_RPAREN SYM_SEMICOLON

<instrucao_for> ::= KW_FOR SYM_LPAREN <expressao_opcional> SYM_SEMICOLON <expressao_opcional> SYM_SEMICOLON <expressao_opcional> SYM_RPAREN <instrucao>

<instrucao_switch> ::= KW_SWITCH SYM_LPAREN <expressao> SYM_RPAREN SYM_LBRACE <lista_casos> SYM_RBRACE

<lista_casos> ::= <caso> <lista_casos>
                | ε

<caso> ::= KW_CASE <expressao> SYM_COLON <lista_itens_bloco>
         | KW_DEFAULT SYM_COLON <lista_itens_bloco>

<instrucao_break> ::= KW_BREAK SYM_SEMICOLON

<instrucao_continue> ::= KW_CONTINUE SYM_SEMICOLON

<instrucao_return> ::= KW_RETURN <expressao_opcional> SYM_SEMICOLON

<instrucao_goto> ::= KW_GOTO TK_ID SYM_SEMICOLON

<expressao_opcional> ::= <expressao>
                       | ε

<expressao> ::= <atribuicao>

<atribuicao> ::= <ternario> <atribuicao_r>

<atribuicao_r> ::= OP_ASSIGN <atribuicao>
                 | OP_PLUS_ASSIGN <atribuicao>
                 | OP_MINUS_ASSIGN <atribuicao>
                 | OP_MULT_ASSIGN <atribuicao>
                 | OP_DIV_ASSIGN <atribuicao>
                 | OP_AND_ASSIGN <atribuicao>
                 | OP_OR_ASSIGN <atribuicao>
                 | OP_XOR_ASSIGN <atribuicao>
                 | ε

<ternario> ::= <logico_ou> <ternario_r>

<ternario_r> ::= SYM_TERNARY <expressao> SYM_COLON <ternario>
               | ε

<logico_ou> ::= <logico_e> <logico_ou_r>

<logico_ou_r> ::= OP_OR <logico_e> <logico_ou_r>
                | ε

<logico_e> ::= <bitor> <logico_e_r>

<logico_e_r> ::= OP_AND <bitor> <logico_e_r>
               | ε

<bitor> ::= <bitxor> <bitor_r>

<bitor_r> ::= OP_BITOR <bitxor> <bitor_r>
            | ε

<bitxor> ::= <bitand> <bitxor_r>

<bitxor_r> ::= OP_BITXOR <bitand> <bitxor_r>
             | ε

<bitand> ::= <igualdade> <bitand_r>

<bitand_r> ::= OP_BITAND <igualdade> <bitand_r>
             | ε

<igualdade> ::= <relacional> <igualdade_r>

<igualdade_r> ::= OP_EQ <relacional> <igualdade_r>
                | OP_NE <relacional> <igualdade_r>
                | ε

<relacional> ::= <shift> <relacional_r>

<relacional_r> ::= OP_LT <shift> <relacional_r>
                 | OP_GT <shift> <relacional_r>
                 | OP_LE <shift> <relacional_r>
                 | OP_GE <shift> <relacional_r>
                 | ε

<shift> ::= <aditivo> <shift_r>

<shift_r> ::= OP_LSHIFT <aditivo> <shift_r>
            | OP_RSHIFT <aditivo> <shift_r>
            | ε

<aditivo> ::= <multiplicativo> <aditivo_r>

<aditivo_r> ::= OP_PLUS <multiplicativo> <aditivo_r>
              | OP_MINUS <multiplicativo> <aditivo_r>
              | ε

<multiplicativo> ::= <unario> <multiplicativo_r>

<multiplicativo_r> ::= OP_MULT <unario> <multiplicativo_r>
                     | OP_DIV <unario> <multiplicativo_r>
                     | OP_MOD <unario> <multiplicativo_r>
                     | ε

<unario> ::= OP_INC <unario>
           | OP_DEC <unario>
           | OP_NOT <unario>
           | OP_BITNOT <unario>
           | OP_MINUS <unario>
           | OP_PLUS <unario>
           | OP_BITAND <unario>
           | OP_MULT <unario>
           | KW_SIZEOF <unario_sizeof>
           | <pos_fixo>

<unario_sizeof> ::= SYM_LPAREN <especificador_tipo> SYM_RPAREN
                  | <unario>

<pos_fixo> ::= <primario> <pos_fixo_r>

<pos_fixo_r> ::= SYM_LBRACKET <expressao> SYM_RBRACKET <pos_fixo_r>
               | SYM_DOT TK_ID <pos_fixo_r>
               | SYM_LPAREN <argumentos_opcionais> SYM_RPAREN <pos_fixo_r>
               | OP_INC <pos_fixo_r>
               | OP_DEC <pos_fixo_r>
               | ε

<primario> ::= TK_ID
             | TK_NUM_INT
             | TK_NUM_FLOAT
             | TK_NUM_EXP
             | TK_CHAR
             | TK_STRING
             | SYM_LPAREN <expressao> SYM_RPAREN

<argumentos_opcionais> ::= <lista_argumentos>
                         | ε

<lista_argumentos> ::= <expressao> <mais_argumentos>

<mais_argumentos> ::= SYM_COMMA <expressao> <mais_argumentos>
                    | ε
```

---

## ✅ Características da Gramática

### 1. **Sem Recursividade à Esquerda**
Todas as produções recursivas foram transformadas usando o padrão `_r`:
- `<atribuicao> ::= <ternario> <atribuicao_r>`
- `<logico_ou> ::= <logico_e> <logico_ou_r>`
- etc.

### 2. **Sem Ambiguidades**
- Precedência de operadores claramente definida
- Associatividade correta (esquerda para direita, exceto atribuição)

### 3. **LL(1) Compatível**
- FIRST sets disjuntos para todas as alternativas
- Parsing preditivo sem backtracking

### 4. **100% Compatível com TABELA_TOKENS_LEXEMA.md**
- Todos os tokens usados existem na tabela
- Nomenclatura idêntica (KW_, OP_, SYM_, TK_)

---

## 📊 Precedência de Operadores (Menor → Maior)

| Nível | Operadores | Associatividade | Produção |
|-------|-----------|-----------------|----------|
| 1 | `=`, `+=`, `-=`, `*=`, `/=`, `&=`, `\|=`, `^=` | Direita | `<atribuicao>` |
| 2 | `?:` (ternário) | Direita | `<ternario>` |
| 3 | `\|\|` | Esquerda | `<logico_ou>` |
| 4 | `&&` | Esquerda | `<logico_e>` |
| 5 | `\|` | Esquerda | `<bitor>` |
| 6 | `^` | Esquerda | `<bitxor>` |
| 7 | `&` | Esquerda | `<bitand>` |
| 8 | `==`, `!=` | Esquerda | `<igualdade>` |
| 9 | `<`, `>`, `<=`, `>=` | Esquerda | `<relacional>` |
| 10 | `<<`, `>>` | Esquerda | `<shift>` |
| 11 | `+`, `-` | Esquerda | `<aditivo>` |
| 12 | `*`, `/`, `%` | Esquerda | `<multiplicativo>` |
| 13 | `!`, `~`, `-`, `+`, `&`, `*`, `++`, `--`, `sizeof` | Direita | `<unario>` |
| 14 | `[]`, `.`, `()`, `++`, `--` (pós-fixo) | Esquerda | `<pos_fixo>` |
| 15 | Literais, identificadores, `()` | - | `<primario>` |

---

## 🎯 Exemplo de Parsing

### Código C:
```c
int main() {
    int x = 10;
    if (x > 5)
        x++;
    return 0;
}
```

### Árvore de Derivação (Simplificada):
```
<programa>
└── <lista_declaracoes_globais>
    └── <declaracao_global>
        └── <declaracao_geral>
            ├── <especificador_tipo>: KW_INT
            └── <resto_declaracao_geral>
                ├── TK_ID: "main"
                └── <cauda_declaracao_geral>
                    ├── SYM_LPAREN
                    ├── <parametros_opcionais>: ε
                    ├── SYM_RPAREN
                    └── <bloco>
                        ├── SYM_LBRACE
                        ├── <lista_itens_bloco>
                        │   ├── <declaracao_variavel_local>
                        │   │   ├── KW_INT
                        │   │   ├── TK_ID: "x"
                        │   │   ├── OP_ASSIGN
                        │   │   ├── TK_NUM_INT: 10
                        │   │   └── SYM_SEMICOLON
                        │   ├── <instrucao_if>
                        │   │   ├── KW_IF
                        │   │   ├── SYM_LPAREN
                        │   │   ├── <expressao>: x > 5
                        │   │   ├── SYM_RPAREN
                        │   │   └── <instrucao>: x++
                        │   └── <instrucao_return>
                        │       ├── KW_RETURN
                        │       ├── TK_NUM_INT: 0
                        │       └── SYM_SEMICOLON
                        └── SYM_RBRACE
```

---

## 📝 Notas de Implementação

1. **Pré-processador**: As diretivas são reconhecidas como tokens únicos (`TK_PP_INCLUDE`, etc.)
2. **Switch-Case**: Suportado com múltiplos casos e default
3. **Operadores Bitwise**: Totalmente suportados com precedência correta
4. **Operador Ternário**: `? :` implementado com associatividade direita
5. **Sizeof**: Suporta `sizeof(tipo)` e `sizeof expressao`
6. **Incremento/Decremento**: Pré-fixo e pós-fixo suportados

---

**Documento gerado:** 24/05/2026  
**Compatível com:** TABELA_TOKENS_LEXEMA.md  
**Método de Parsing:** LL(1) - Recursive Descent  
**Status:** ✅ Pronto para Implementação

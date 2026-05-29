# Implementation Plan: Analisador Sintático (Parser)

## Overview

Este plano decompõe a implementação do **Parser da Fase 2** do compilador em tarefas concretas e executáveis. O parser implementa o método **Top-Down Recursivo Descendente sem backtracking (LL(1))**, consumindo tokens da tabela de símbolos gerada pelo Lexer e construindo uma Abstract Syntax Tree (AST) enquanto enriquece a tabela de símbolos com informações semânticas.

**Linguagem de implementação:** C

**Método de parsing:** Recursive Descent Predictive Parser (LL(1))

**Estrutura de arquivos:**
```
src/parser/
├── parser.h          # Interface do parser
├── parser.c          # Implementação principal
├── parser_expr.c     # Parsing de expressões
├── parser_stmt.c     # Parsing de instruções
├── parser_decl.c     # Parsing de declarações
├── ast.h             # Estrutura da AST
├── ast.c             # Funções de AST
├── scope.h           # Gerenciamento de escopos
├── scope.c           # Implementação de escopos
└── parser_error.c    # Tratamento de erros
```

## Tasks

- [x] 1. Configurar infraestrutura básica do parser
  - Criar estrutura de diretórios `src/parser/`
  - Criar arquivos de cabeçalho e implementação vazios
  - Atualizar Makefile para compilar módulos do parser
  - _Requirements: 1.1, 1.2_


- [x] 2. Implementar estruturas de dados fundamentais
  - [x] 2.1 Criar estrutura `ast_node_t` em `ast.h`
    - Definir enum `node_type_t` com todos os tipos de nós (NODE_PROGRAM, NODE_VAR_DECL, NODE_FUNC_DECL, NODE_BLOCK, NODE_IF_STMT, NODE_WHILE_STMT, NODE_FOR_STMT, NODE_DO_WHILE_STMT, NODE_RETURN_STMT, NODE_EXPR_STMT, NODE_BINARY_OP, NODE_UNARY_OP, NODE_ASSIGN, NODE_CALL, NODE_ARRAY_ACCESS, NODE_MEMBER_ACCESS, NODE_IDENTIFIER, NODE_LITERAL)
    - Definir estrutura `ast_node_t` com union para diferentes tipos de nós
    - Incluir campos: type, line, column, data (union com decl, stmt, expr), children, child_count
    - _Requirements: 14.2, 14.3, 14.4_

  - [x] 2.2 Criar estrutura `scope_t` em `scope.h`
    - Definir estrutura `scope_t` com scope_id, parent, local_symbols, symbol_count, symbol_capacity
    - Documentar hierarquia de escopos (global = 0, função, bloco)
    - _Requirements: 7.1, 7.5_

  - [x] 2.3 Criar estrutura `symbol_info_t` em `parser.h`
    - Definir enum para variable_type (VAR_GLOBAL, VAR_LOCAL, VAR_PARAMETER, VAR_STRUCT_FIELD)
    - Definir estrutura `symbol_info_t` com campos: data_type, is_pointer, variable_type, scope_id, memory_address, size_bytes, is_array, array_dimensions, array_dim_count, is_function, function_return_type, function_param_types, function_param_count
    - _Requirements: 5.1, 5.2, 5.3, 5.4, 5.5, 6.1, 6.2, 6.3_

  - [x] 2.4 Criar estrutura `parser_t` em `parser.h`
    - Definir estrutura `parser_t` com campos: symbol_table (ponteiro para tabla_simbolos_t), current_position, scope_stack, scope_stack_size, scope_stack_capacity, next_scope_id, next_memory_address, error_count, panic_mode, ast_root
    - Incluir comentários explicando cada campo
    - _Requirements: 1.1, 1.2, 7.1_


- [x] 3. Implementar funções de consumo de tokens
  - [x] 3.1 Implementar `peek_token()` em `parser.c`
    - Retornar token atual sem avançar posição: `symbol_table->tokens[current_position]`
    - Verificar se current_position está dentro dos limites
    - Retornar token TK_EOF se ultrapassar limite
    - _Requirements: 1.5_

  - [x] 3.2 Implementar `consume_token()` em `parser.c`
    - Retornar token atual e incrementar current_position
    - Usar peek_token() internamente
    - _Requirements: 1.3_

  - [x] 3.3 Implementar `match()` em `parser.c`
    - Verificar se peek_token().tipo == token_type esperado
    - Retornar 1 se match, 0 caso contrário
    - _Requirements: 4.1_

  - [x] 3.4 Implementar `expect()` em `parser.c`
    - Chamar match() para verificar token esperado
    - Se match, chamar consume_token() e retornar
    - Se não match, chamar syntax_error() com mensagem descritiva
    - _Requirements: 4.3, 13.1, 13.2, 13.3, 13.4_


- [~] 4. Implementar gerenciamento de escopos
  - [x] 4.1 Implementar `enter_local_scope()` em `parser.c` (usa `scope_create()` em `scope.c`)
    - Criar novo scope_t com scope_id único (next_scope_id++)
    - Definir parent como scope atual no topo da pilha (ou NULL se global)
    - Empilhar novo scope na scope_stack
    - Redimensionar pilha se necessário (realloc)
    - _Requirements: 7.2_

  - [~] 4.2 Implementar `exit_local_scope()` em `parser.c`
    - (Implementado) Desempilhar scope do topo da pilha e atualizar `current_local_table`
    - (Pendente) Liberar memória do scope removido (hoje não libera; leak)
    - _Requirements: 7.3, 19.3_

  - [ ] 4.3 Implementar `get_current_scope_id()` (helper)
    - Retornar scope_id do scope no topo da pilha
    - Retornar 0 se pilha vazia (escopo global)
    - _Requirements: 7.5_

  - [x] 4.4 Implementar `lookup_symbol()` em `parser.c`
    - Buscar símbolo no scope atual
    - Se não encontrado, buscar recursivamente em parent scopes
    - Retornar symbol_info_t* ou NULL se não encontrado
    - _Requirements: 7.4_

  - [x] 4.5 Implementar `add_symbol_to_scope()` em `scope.c`
    - Adicionar nome de símbolo ao array local_symbols do scope atual
    - Redimensionar array se necessário
    - _Requirements: 7.4_


- [x] 5. Implementar funções de construção de AST
  - [x] 5.1 Implementar `create_ast_node()` em `ast.c`
    - Alocar memória para ast_node_t usando malloc
    - Inicializar campos: type, line, column
    - Inicializar children como NULL e child_count como 0
    - Retornar ponteiro para nó criado
    - _Requirements: 14.1_

  - [x] 5.2 Implementar `add_ast_child()` em `ast.c`
    - Adicionar child ao array children do parent
    - Redimensionar array se necessário (realloc)
    - Incrementar child_count
    - _Requirements: 14.1_

  - [x] 5.3 Implementar `create_binary_op_node()` em `ast.c`
    - Criar nó NODE_BINARY_OP
    - Definir operator, left, right
    - Retornar nó criado
    - _Requirements: 14.4_

  - [x] 5.4 Implementar `create_unary_op_node()` em `ast.c`
    - Criar nó NODE_UNARY_OP
    - Definir operator, operand (em left)
    - Retornar nó criado
    - _Requirements: 14.4_

  - [x] 5.5 Implementar `create_identifier_node()` em `ast.c`
    - Criar nó NODE_IDENTIFIER
    - Copiar nome do identificador para data.expr.identifier
    - Retornar nó criado
    - _Requirements: 14.4_

  - [x] 5.6 Implementar `create_literal_node()` em `ast.c`
    - Criar nó NODE_LITERAL
    - Extrair valor do token (int, float, char, string)
    - Armazenar em data.expr.value
    - Retornar nó criado
    - _Requirements: 14.4_

  - [x] 5.7 Implementar `free_ast()` em `ast.c`
    - Liberar recursivamente todos os children
    - Liberar strings alocadas dinamicamente
    - Liberar o próprio nó
    - _Requirements: 14.6, 19.1, 19.3_


- [x] 6. Implementar enriquecimento da tabela de símbolos
  - [x] 6.1 Implementar `enrich_symbol_type()` em `parser.c`
    - Buscar token na symbol_table pelo nome
    - Adicionar campos data_type e is_pointer ao token
    - _Requirements: 5.1_

  - [x] 6.2 Implementar `enrich_symbol_scope()` em `parser.c`
    - Buscar token na symbol_table pelo nome
    - Adicionar campos scope_id e variable_type ao token
    - _Requirements: 5.2, 5.3_

  - [x] 6.3 Implementar `enrich_symbol_memory()` em `parser.c`
    - Buscar token na symbol_table pelo nome
    - Adicionar campos memory_address e size_bytes ao token
    - _Requirements: 5.4, 5.5_

  - [x] 6.4 Implementar `enrich_symbol_array()` em `parser.c`
    - Buscar token na symbol_table pelo nome
    - Adicionar campos is_array, array_dimensions, array_dim_count ao token
    - _Requirements: 11.3, 11.4_

  - [x] 6.5 Implementar `enrich_symbol_function()` em `parser.c`
    - Buscar token na symbol_table pelo nome
    - Adicionar campos is_function, function_return_type, function_param_types, function_param_count ao token
    - _Requirements: 6.1, 6.2, 6.3_


- [x] 7. Implementar parsing de expressões primárias e pós-fixas
  - [x] 7.1 Implementar `parse_primario()` em `parser_expr.c`
    - Implementar produção: `<primario> ::= TOKEN_IDENTIFIER | TOKEN_INT_LITERAL | TOKEN_FLOAT_LITERAL | TOKEN_CHAR_LITERAL | TOKEN_STRING_LITERAL | TOKEN_LPAREN <expressao> TOKEN_RPAREN`
    - Usar switch/case baseado em peek_token().tipo
    - Para identificadores: criar NODE_IDENTIFIER
    - Para literais: criar NODE_LITERAL com valor extraído
    - Para expressões entre parênteses: chamar parse_expressao() recursivamente
    - Retornar ast_node_t* criado
    - _Requirements: 2.1, 2.2, 10.10_

  - [x] 7.2 Implementar `parse_pos_fixo()` em `parser_expr.c`
    - Implementar produção: `<pos_fixo> ::= <primario> <pos_fixo_r>`
    - Chamar parse_primario() para obter nó base
    - Chamar parse_pos_fixo_r() passando ponteiro para nó base
    - Retornar nó (possivelmente modificado por _r)
    - _Requirements: 10.6, 10.7, 10.8_

  - [x] 7.3 Implementar `parse_pos_fixo_r()` em `parser_expr.c`
    - Implementar produção auxiliar: `<pos_fixo_r> ::= TOKEN_LBRACKET <expressao> TOKEN_RBRACKET <pos_fixo_r> | TOKEN_DOT TOKEN_IDENTIFIER <pos_fixo_r> | TOKEN_LPAREN <argumentos_opcionais> TOKEN_RPAREN <pos_fixo_r> | ε`
    - Usar loop while para processar múltiplos operadores pós-fixos
    - Para `[expr]`: criar NODE_ARRAY_ACCESS com left=base, right=index
    - Para `.id`: criar NODE_MEMBER_ACCESS com left=base, identifier=id
    - Para `(args)`: criar NODE_CALL com left=base, children=args
    - Atualizar ponteiro *left_ptr com novo nó criado
    - _Requirements: 10.6, 10.7, 10.8_


- [x] 8. Implementar parsing de expressões unárias
  - [x] 8.1 Implementar `parse_unario()` em `parser_expr.c`
    - Implementar produção: `<unario> ::= TOKEN_NOT <unario> | TOKEN_MINUS <unario> | TOKEN_AMP <unario> | TOKEN_STAR <unario> | <pos_fixo>`
    - Verificar se token atual é operador unário (!, -, &, *)
    - Se sim: consumir operador, chamar parse_unario() recursivamente, criar NODE_UNARY_OP
    - Se não: chamar parse_pos_fixo()
    - Retornar nó criado
    - _Requirements: 10.5_


- [x] 9. Implementar parsing de expressões com precedência de operadores
  - [x] 9.1 Implementar `parse_multiplicativo()` e `parse_multiplicativo_r()` em `parser_expr.c`
    - Implementar produção: `<multiplicativo> ::= <unario> <multiplicativo_r>`
    - Implementar produção auxiliar: `<multiplicativo_r> ::= TOKEN_STAR <unario> <multiplicativo_r> | TOKEN_SLASH <unario> <multiplicativo_r> | TOKEN_PERCENT <unario> <multiplicativo_r> | ε`
    - parse_multiplicativo(): chamar parse_unario(), depois parse_multiplicativo_r()
    - parse_multiplicativo_r(): usar loop while para processar *, /, %
    - Criar NODE_BINARY_OP para cada operador encontrado
    - _Requirements: 10.1, 10.9_

  - [x] 9.2 Implementar `parse_aditivo()` e `parse_aditivo_r()` em `parser_expr.c`
    - Implementar produção: `<aditivo> ::= <multiplicativo> <aditivo_r>`
    - Implementar produção auxiliar: `<aditivo_r> ::= TOKEN_PLUS <multiplicativo> <aditivo_r> | TOKEN_MINUS <multiplicativo> <aditivo_r> | ε`
    - parse_aditivo(): chamar parse_multiplicativo(), depois parse_aditivo_r()
    - parse_aditivo_r(): usar loop while para processar +, -
    - Criar NODE_BINARY_OP para cada operador encontrado
    - _Requirements: 10.1, 10.9_

  - [x] 9.3 Implementar `parse_relacional()` e `parse_relacional_r()` em `parser_expr.c`
    - Implementar produção: `<relacional> ::= <aditivo> <relacional_r>`
    - Implementar produção auxiliar: `<relacional_r> ::= TOKEN_LT <aditivo> <relacional_r> | TOKEN_GT <aditivo> <relacional_r> | TOKEN_LEQ <aditivo> <relacional_r> | TOKEN_GEQ <aditivo> <relacional_r> | ε`
    - parse_relacional(): chamar parse_aditivo(), depois parse_relacional_r()
    - parse_relacional_r(): usar loop while para processar <, >, <=, >=
    - Criar NODE_BINARY_OP para cada operador encontrado
    - _Requirements: 10.2, 10.9_

  - [x] 9.4 Implementar `parse_igualdade()` e `parse_igualdade_r()` em `parser_expr.c`
    - Implementar produção: `<igualdade> ::= <relacional> <igualdade_r>`
    - Implementar produção auxiliar: `<igualdade_r> ::= TOKEN_EQ <relacional> <igualdade_r> | TOKEN_NEQ <relacional> <igualdade_r> | ε`
    - parse_igualdade(): chamar parse_relacional(), depois parse_igualdade_r()
    - parse_igualdade_r(): usar loop while para processar ==, !=
    - Criar NODE_BINARY_OP para cada operador encontrado
    - _Requirements: 10.2, 10.9_

  - [x] 9.5 Implementar `parse_logico_e()` e `parse_logico_e_r()` em `parser_expr.c`
    - Implementar produção: `<logico_e> ::= <igualdade> <logico_e_r>`
    - Implementar produção auxiliar: `<logico_e_r> ::= TOKEN_AND <igualdade> <logico_e_r> | ε`
    - parse_logico_e(): chamar parse_igualdade(), depois parse_logico_e_r()
    - parse_logico_e_r(): usar loop while para processar &&
    - Criar NODE_BINARY_OP para cada operador encontrado
    - _Requirements: 10.3, 10.9_

  - [x] 9.6 Implementar `parse_logico_ou()` e `parse_logico_ou_r()` em `parser_expr.c`
    - Implementar produção: `<logico_ou> ::= <logico_e> <logico_ou_r>`
    - Implementar produção auxiliar: `<logico_ou_r> ::= TOKEN_OR <logico_e> <logico_ou_r> | ε`
    - parse_logico_ou(): chamar parse_logico_e(), depois parse_logico_ou_r()
    - parse_logico_ou_r(): usar loop while para processar ||
    - Criar NODE_BINARY_OP para cada operador encontrado
    - _Requirements: 10.3, 10.9_

  - [x] 9.7 Implementar `parse_atribuicao()` e `parse_atribuicao_r()` em `parser_expr.c`
    - Implementar produção: `<atribuicao> ::= <logico_ou> <atribuicao_r>`
    - Implementar produção auxiliar: `<atribuicao_r> ::= TOKEN_ASSIGN <atribuicao> | TOKEN_PLUS_ASSIGN <atribuicao> | TOKEN_MINUS_ASSIGN <atribuicao> | TOKEN_STAR_ASSIGN <atribuicao> | TOKEN_SLASH_ASSIGN <atribuicao> | TOKEN_PERCENT_ASSIGN <atribuicao> | ε`
    - parse_atribuicao(): chamar parse_logico_ou(), depois parse_atribuicao_r()
    - parse_atribuicao_r(): verificar operadores de atribuição, criar NODE_ASSIGN
    - _Requirements: 10.4, 10.9_

  - [x] 9.8 Implementar `parse_expressao()` em `parser_expr.c`
    - Implementar produção: `<expressao> ::= <atribuicao>`
    - Simplesmente chamar parse_atribuicao() e retornar resultado
    - Esta é a função de entrada para parsing de expressões
    - _Requirements: 2.5, 10.9_


- [~] 10. Checkpoint - Validar parsing de expressões
  - Compilar módulo parser_expr.c
  - Criar programa de teste simples com expressões aritméticas, lógicas e relacionais
  - Verificar se AST é construída corretamente para expressões
  - Ensure all tests pass, ask the user if questions arise.


- [x] 11. Implementar parsing de instruções básicas
  - [x] 11.1 Implementar `parse_instrucao_expressao()` em `parser_stmt.c`
    - Implementar produção: `<instrucao_expressao> ::= <expressao> TOKEN_SEMICOLON`
    - Chamar parse_expressao() para obter nó de expressão
    - Consumir TOKEN_SEMICOLON com expect()
    - Criar NODE_EXPR_STMT contendo a expressão
    - Retornar nó criado
    - _Requirements: 9.8_

  - [x] 11.2 Implementar `parse_instrucao_return()` em `parser_stmt.c`
    - Implementar produção: `<instrucao_return> ::= TOKEN_RETURN <expressao_opcional> TOKEN_SEMICOLON`
    - Consumir TOKEN_RETURN
    - Verificar se próximo token não é TOKEN_SEMICOLON
    - Se não for, chamar parse_expressao() para obter valor de retorno
    - Consumir TOKEN_SEMICOLON
    - Criar NODE_RETURN_STMT com expressão opcional
    - Retornar nó criado
    - _Requirements: 9.6_

  - [~] 11.3 Implementar `parse_bloco()` em `parser_stmt.c`
    - Implementar produção: `<bloco> ::= TOKEN_LBRACE <lista_itens_bloco> TOKEN_RBRACE`
    - Consumir TOKEN_LBRACE
    - Chamar `enter_local_scope()` para criar novo escopo
    - Criar NODE_BLOCK
    - Loop: enquanto não encontrar TOKEN_RBRACE ou TK_EOF
      - Verificar se é declaração de variável (is_type_specifier)
      - Se sim: chamar parse_declaracao_variavel_local()
      - Se não: chamar parse_instrucao()
      - Adicionar nó retornado como child do bloco
    - Consumir TOKEN_RBRACE
    - Chamar `exit_local_scope()`
    - Retornar nó de bloco
    - _Requirements: 7.2, 7.3, 9.7_


- [x] 12. Implementar parsing de estruturas de controle
  - [x] 12.1 Implementar `parse_instrucao_if()` em `parser_stmt.c`
    - Implementar produção: `<instrucao_if> ::= TOKEN_IF TOKEN_LPAREN <expressao> TOKEN_RPAREN <instrucao> <ramo_else_opcional>`
    - Consumir TOKEN_IF e TOKEN_LPAREN
    - Chamar parse_expressao() para obter condição
    - Consumir TOKEN_RPAREN
    - Chamar parse_instrucao() para obter ramo then
    - Verificar se próximo token é TOKEN_ELSE
    - Se sim: consumir TOKEN_ELSE, chamar parse_instrucao() para ramo else
    - Criar NODE_IF_STMT com condition, then_branch, else_branch (opcional)
    - Retornar nó criado
    - _Requirements: 9.1_

  - [x] 12.2 Implementar `parse_instrucao_while()` em `parser_stmt.c`
    - Implementar produção: `<instrucao_while> ::= TOKEN_WHILE TOKEN_LPAREN <expressao> TOKEN_RPAREN <instrucao>`
    - Consumir TOKEN_WHILE e TOKEN_LPAREN
    - Chamar parse_expressao() para obter condição
    - Consumir TOKEN_RPAREN
    - Chamar parse_instrucao() para obter corpo do loop
    - Criar NODE_WHILE_STMT com condition e then_branch
    - Retornar nó criado
    - _Requirements: 9.2_

  - [x] 12.3 Implementar `parse_instrucao_do()` em `parser_stmt.c`
    - Implementar produção: `<instrucao_do> ::= TOKEN_DO <instrucao> TOKEN_WHILE TOKEN_LPAREN <expressao> TOKEN_RPAREN TOKEN_SEMICOLON`
    - Consumir TOKEN_DO
    - Chamar parse_instrucao() para obter corpo do loop
    - Consumir TOKEN_WHILE e TOKEN_LPAREN
    - Chamar parse_expressao() para obter condição
    - Consumir TOKEN_RPAREN e TOKEN_SEMICOLON
    - Criar NODE_DO_WHILE_STMT com then_branch e condition
    - Retornar nó criado
    - _Requirements: 9.4_

  - [x] 12.4 Implementar `parse_instrucao_for()` em `parser_stmt.c`
    - Implementar produção: `<instrucao_for> ::= TOKEN_FOR TOKEN_LPAREN <expressao_opcional> TOKEN_SEMICOLON <expressao_opcional> TOKEN_SEMICOLON <expressao_opcional> TOKEN_RPAREN <instrucao>`
    - Consumir TOKEN_FOR e TOKEN_LPAREN
    - Verificar se próximo token não é TOKEN_SEMICOLON, se não for: chamar parse_expressao() para init
    - Consumir TOKEN_SEMICOLON
    - Verificar se próximo token não é TOKEN_SEMICOLON, se não for: chamar parse_expressao() para condition
    - Consumir TOKEN_SEMICOLON
    - Verificar se próximo token não é TOKEN_RPAREN, se não for: chamar parse_expressao() para increment
    - Consumir TOKEN_RPAREN
    - Chamar parse_instrucao() para obter corpo do loop
    - Criar NODE_FOR_STMT com init, condition, increment, then_branch
    - Retornar nó criado
    - _Requirements: 9.3_

  - [x] 12.5 Implementar `parse_instrucao()` em `parser_stmt.c`
    - Implementar produção: `<instrucao> ::= <instrucao_expressao> | <instrucao_if> | <instrucao_while> | <instrucao_for> | <instrucao_do> | <instrucao_return> | <bloco> | TOKEN_SEMICOLON`
    - Usar switch/case baseado em peek_token().tipo
    - TOKEN_IF → parse_instrucao_if()
    - TOKEN_WHILE → parse_instrucao_while()
    - TOKEN_FOR → parse_instrucao_for()
    - TOKEN_DO → parse_instrucao_do()
    - TOKEN_RETURN → parse_instrucao_return()
    - TOKEN_LBRACE → parse_bloco()
    - TOKEN_SEMICOLON → consumir e retornar NULL (empty statement)
    - Caso contrário → parse_instrucao_expressao()
    - Retornar nó criado
    - _Requirements: 2.5, 9.1, 9.2, 9.3, 9.4, 9.6, 9.7, 9.8_


- [~] 13. Checkpoint - Validar parsing de instruções
  - Compilar módulo parser_stmt.c
  - Criar programa de teste com if, while, for, do-while, return, break/continue, switch-case-default, blocos
  - Verificar se AST é construída corretamente para instruções
  - Ensure all tests pass, ask the user if questions arise.



- [x] 14. Implementar parsing de declarações de variáveis
  - [x] 14.1 Implementar `parse_especificador_tipo()` em `parser_decl.c`
    - Implementar produção: `<especificador_tipo> ::= KW_INT | KW_CHAR | KW_FLOAT | KW_VOID | KW_STRUCT <nome_ou_corpo_struct> | TOKEN_IDENTIFIER`
    - Usar switch/case baseado em peek_token().tipo
    - Para KW_INT, KW_CHAR, KW_FLOAT, KW_VOID: consumir e retornar tipo
    - Para KW_STRUCT: consumir e chamar parse_nome_ou_corpo_struct()
    - Para TOKEN_IDENTIFIER: verificar se é typedef name, consumir e retornar
    - Retornar código do tipo (KW_INT, KW_CHAR, etc.)
    - _Requirements: 8.1, 8.5, 12.1, 12.2, 12.3_

  - [x] 14.2 Implementar `parse_declaracao_variavel_global()` em `parser_decl.c`
    - Implementar produção: `<declaracao_variavel_global> ::= <especificador_tipo> <asteriscos> TOKEN_IDENTIFIER <sufixo_array_opcional> <inicializacao_opcional> <mais_declaradores> TOKEN_SEMICOLON`
    - Chamar parse_especificador_tipo() para obter tipo base
    - Contar asteriscos para determinar nível de ponteiro
    - Consumir TOKEN_IDENTIFIER para obter nome da variável
    - Verificar se há colchetes para arrays (loop consumindo [expr])
    - Calcular tamanho total em bytes
    - Atribuir memory_address sequencial (next_memory_address)
    - Chamar enrich_symbol_type(), enrich_symbol_scope(VAR_GLOBAL), enrich_symbol_memory()
    - Se array: chamar enrich_symbol_array()
    - Adicionar símbolo ao escopo global com add_symbol_to_scope()
    - Verificar se há inicialização (TOKEN_ASSIGN)
    - Processar mais declaradores se houver TOKEN_COMMA
    - Consumir TOKEN_SEMICOLON
    - Criar NODE_VAR_DECL e retornar
    - _Requirements: 5.1, 5.2, 5.3, 5.4, 5.5, 8.2, 11.1, 11.2, 11.3, 11.4, 11.5_

  - [ ] 14.4 Suportar inicializacao com lista (`{ ... }`) em declaracoes (ex.: `int d[3] = {0};`)
    - Hoje o parser so aceita `OP_ASSIGN Expressao` como inicializacao
    - Implementar parsing de initializer-list (pelo menos `{ expr (, expr)* }`)
    - (Opcional) suportar inicializacao parcial e chaves aninhadas para arrays multidimensionais
    - _Requirements: 11.5_

  - [x] 14.3 Implementar `is_type_specifier()` em `parser_decl.c`
    - Implementado como `is_type_specifier(parser_t *parser, token_t token)`
    - Keywords (KW_INT, KW_CHAR, ...) são tipos diretamente
    - `TK_ID` só é considerado tipo quando for um typedef conhecido (consultando `global_symbol_table`)
    - _Requirements: 2.1, 8.1, 8.5_


- [x] 15. Implementar parsing de declarações de funções
  - [x] 15.1 Implementar `parse_lista_parametros()` em `parser_decl.c`
    - Implementar produção: `<lista_parametros> ::= <parametro> <mais_parametros> | KW_VOID | ε`
    - Verificar se próximo token é KW_VOID (função sem parâmetros)
    - Se não: loop consumindo parâmetros separados por TOKEN_COMMA
    - Para cada parâmetro: chamar parse_especificador_tipo(), contar asteriscos, consumir TOKEN_IDENTIFIER
    - Enriquecer símbolo com enrich_symbol_type(), enrich_symbol_scope(VAR_PARAMETER)
    - Adicionar parâmetro ao escopo da função
    - Armazenar tipos de parâmetros em array
    - Retornar array de nós de parâmetros e contagem
    - _Requirements: 6.2, 6.3_

  - [x] 15.2 Implementar `parse_declaracao_funcao()` em `parser_decl.c`
    - Implementar produção: `<declaracao_funcao> ::= <especificador_tipo> <asteriscos> TOKEN_IDENTIFIER TOKEN_LPAREN <lista_parametros> TOKEN_RPAREN <bloco>`
    - Chamar parse_especificador_tipo() para obter tipo de retorno
    - Contar asteriscos para ponteiros
    - Consumir TOKEN_IDENTIFIER para obter nome da função
    - Consumir TOKEN_LPAREN
    - Chamar enter_scope() para criar escopo da função
    - Chamar parse_lista_parametros() para obter parâmetros
    - Consumir TOKEN_RPAREN
    - Enriquecer símbolo com enrich_symbol_function(return_type, param_types, param_count)
    - Adicionar função ao escopo global
    - Chamar parse_bloco() para obter corpo da função
    - Chamar exit_scope()
    - Criar NODE_FUNC_DECL com return_type, name, parameters, body
    - Retornar nó criado
    - _Requirements: 6.1, 6.2, 6.3, 8.3_

  - [ ] 15.3 Implementar verificação de função main
    - Verificar se pelo menos uma função declarada tem nome "main"
    - Marcar função main como ponto de entrada
    - Reportar erro se main não for encontrada ao final do parsing
    - _Requirements: 6.4, 8.4_


- [~] 16. Implementar parsing de structs e unions
  - [x] 16.1 Implementar `parse_nome_ou_corpo_struct()` em `parser_decl.c`
    - Implementar produção: `<nome_ou_corpo_struct> ::= TOKEN_IDENTIFIER <corpo_struct_opcional> | <corpo_struct>`
    - Verificar se próximo token é TOKEN_IDENTIFIER (nome do struct)
    - Se sim: consumir nome, verificar se há TOKEN_LBRACE para corpo
    - Se não: deve ter corpo obrigatório (TOKEN_LBRACE)
    - Se houver corpo: chamar parse_corpo_struct()
    - Retornar nó de struct com nome e campos
    - _Requirements: 8.6, 12.1, 12.3_

  - [x] 16.2 Implementar `parse_corpo_struct()` em `parser_decl.c`
    - Implementar produção: `<corpo_struct> ::= TOKEN_LBRACE <lista_campos_struct> TOKEN_RBRACE`
    - Consumir TOKEN_LBRACE
    - Loop: enquanto não encontrar TOKEN_RBRACE
      - Chamar parse_especificador_tipo() para tipo do campo
      - Consumir TOKEN_IDENTIFIER para nome do campo
      - Verificar arrays com colchetes
      - Enriquecer símbolo com enrich_symbol_scope(VAR_STRUCT_FIELD)
      - Consumir TOKEN_SEMICOLON
      - Adicionar campo ao array de campos
    - Consumir TOKEN_RBRACE
    - Calcular tamanho total do struct (soma dos campos)
    - Retornar array de campos
    - _Requirements: 12.1, 12.4, 12.6_

  - [~] 16.3 Implementar cálculo de tamanho de struct/union
    - Para struct: somar tamanhos de todos os campos
    - Para union: usar máximo dos tamanhos dos campos
    - Considerar alinhamento de memória (padding)
    - Retornar tamanho total em bytes
    - _Requirements: 12.4, 12.5_


- [~] 17. Implementar parsing de declarações globais e programa completo
  - [x] 17.1 Implementar `parse_declaracao_global()` em `parser_decl.c`
    - Implementar produção: `<declaracao_global> ::= <diretiva_include> | <diretiva_define> | <declaracao_typedef> | <declaracao_geral>`
    - Usar switch/case baseado em peek_token().tipo
    - TOKEN_HASH: verificar se é #include ou #define
    - KW_TYPEDEF: chamar parse_declaracao_typedef()
    - Especificadores de tipo: chamar parse_declaracao_geral()
    - parse_declaracao_geral() decide se é variável ou função baseado em lookahead
    - Adicionar nó ao programa
    - _Requirements: 8.1, 8.2, 8.3, 8.5, 8.6_

  - [~] 17.2 Implementar `parse_diretiva_include()` em `parser_decl.c`
    - Implementar produção: `<diretiva_include> ::= TOKEN_HASH KW_INCLUDE TOKEN_STRING_LITERAL`
    - Consumir TOKEN_HASH, verificar se próximo é KW_INCLUDE
    - Consumir TOKEN_STRING_LITERAL ou TOKEN_LT ... TOKEN_GT
    - Registrar arquivo incluído (para referência)
    - Não criar nó AST (diretivas são pré-processamento)
    - _Requirements: 8.1_

  - [~] 17.3 Implementar `parse_diretiva_define()` em `parser_decl.c`
    - Implementar produção: `<diretiva_define> ::= TOKEN_HASH KW_DEFINE TOKEN_IDENTIFIER <valor_opcional>`
    - Consumir TOKEN_HASH, verificar se próximo é KW_DEFINE
    - Consumir TOKEN_IDENTIFIER para nome da macro
    - Consumir tokens até fim de linha para valor
    - Registrar macro (para referência)
    - Não criar nó AST (diretivas são pré-processamento)
    - _Requirements: 8.1_

  - [x] 17.4 Implementar `parse_declaracao_typedef()` em `parser_decl.c`
    - Implementar produção: `<declaracao_typedef> ::= KW_TYPEDEF <especificador_tipo> TOKEN_IDENTIFIER TOKEN_SEMICOLON`
    - Consumir KW_TYPEDEF
    - Chamar parse_especificador_tipo() para obter tipo base
    - Consumir TOKEN_IDENTIFIER para nome do typedef
    - Consumir TOKEN_SEMICOLON
    - Registrar typedef na tabela global (marcar `symbol_info_t.is_typedef = 1`)
    - Criar NODE_TYPEDEF_DECL
    - _Requirements: 8.5_

  - [ ] 17.6 Suportar `typedef struct/union` com corpo (`typedef struct X { ... } Alias;` e `typedef struct { ... } Alias;`)
    - Hoje `parse_declaracao_typedef()` não aceita corpo `{...}` imediatamente após `KW_STRUCT/KW_UNION`
    - Implementar caminho que parseia campos (reaproveitar parse_campos_struct_union) e consome `}` antes do alias
    - Registrar alias como typedef e (opcional) registrar o tag-name `struct X`/`union X`
    - _Requirements: 8.5, 12.1_

  - [x] 17.5 Implementar `parse_lista_declaracoes_globais()` em `parser_decl.c`
    - Implementar produção: `<lista_declaracoes_globais> ::= <declaracao_global> <lista_declaracoes_globais> | ε`
    - Loop: enquanto não encontrar TK_EOF
      - Chamar parse_declaracao_global()
      - Adicionar nó retornado como child do programa
    - Retornar quando encontrar TK_EOF
    - _Requirements: 8.1, 8.2, 8.3, 8.4, 8.5, 8.6_


- [~] 18. Implementar tratamento de erros sintáticos
  - [x] 18.1 Implementar `syntax_error()` em `parser_error.c`
    - Receber mensagem de erro, token esperado, token encontrado
    - Formatar mensagem: "Erro sintático na linha X, coluna Y: esperado 'TOKEN_X', encontrado 'TOKEN_Y'"
    - Imprimir mensagem em stderr
    - Incrementar parser->error_count
    - Ativar parser->panic_mode
    - _Requirements: 13.1, 13.2, 13.3, 13.4_

  - [ ] 18.2 Implementar `panic_mode_recovery()` em `parser_error.c`
    - Consumir tokens até encontrar ponto de sincronização
    - Pontos de sincronização: TOKEN_SEMICOLON, TOKEN_RBRACE, TK_EOF
    - Desativar parser->panic_mode após sincronização
    - Permitir que parsing continue após erro
    - _Requirements: 13.5_

  - [ ] 18.3 Implementar `synchronize()` em `parser_error.c`
    - Função auxiliar para encontrar próximo ponto seguro
    - Verificar se token atual é início de declaração ou instrução
    - Consumir tokens até encontrar token de sincronização
    - Retornar quando encontrar ponto seguro
    - _Requirements: 13.5_


- [~] 19. Implementar integração com Lexer e inicialização
  - [x] 19.1 Implementar `parser_init()` em `parser.c`
    - Alocar memória para parser_t
    - Receber tabla_simbolos_t* como parâmetro
    - Inicializar current_position = 0
    - Inicializar scope_stack com capacidade inicial (ex: 16)
    - Inicializar next_scope_id = 0
    - Inicializar next_memory_address = 0
    - Inicializar error_count = 0, panic_mode = 0
    - Inicializar ast_root = NULL
    - Retornar ponteiro para parser_t
    - _Requirements: 1.1, 1.2, 19.1, 19.2_

  - [~] 19.2 Implementar `parser_free()` em `parser.c`
    - Liberar scope_stack e todos os scopes
    - Liberar AST com free_ast()
    - Liberar estrutura parser_t
    - _Requirements: 19.3, 19.4_

  - [x] 19.3 Implementar `parser_run()` em `parser.c`
    - Função principal de entrada do parser
    - Chamar parse_programa() para iniciar parsing
    - Verificar se error_count > 0
    - Se sim: retornar código de erro
    - Se não: retornar código de sucesso
    - _Requirements: 15.1, 15.2_


- [~] 20. Implementar funções de debug e visualização
  - [ ] 20.1 Implementar `print_symbol_table_enriched()` em `parser.c`
    - Iterar sobre todos os tokens na symbol_table
    - Para cada token com informações enriquecidas:
      - Imprimir: nome, tipo, escopo, endereço, tamanho
      - Se array: imprimir dimensões
      - Se função: imprimir tipo de retorno e parâmetros
    - Formatar saída em tabela legível
    - _Requirements: 15.5_

  - [x] 20.2 Implementar `print_ast()` em `ast.c`
    - Imprimir AST em formato textual hierárquico
    - Usar indentação para mostrar níveis
    - Para cada nó: imprimir tipo, linha, coluna
    - Para declarações: imprimir nome e tipo
    - Para expressões: imprimir operador
    - Para instruções: imprimir tipo de instrução
    - Percorrer recursivamente todos os children
    - _Requirements: 15.6_

  - [x] 20.3 Implementar `print_scope_stack()` em `scope.c`
    - Imprimir pilha de escopos atual
    - Para cada escopo: imprimir scope_id, parent_id, símbolos locais
    - Mostrar hierarquia de escopos
    - Função auxiliar para debug
    - _Requirements: 7.1, 7.2, 7.3, 7.4, 7.5_


- [~] 21. Criar suite de testes para o parser
  - [x] 21.0 Testes jÃ¡ existentes no repositÃ³rio (sanidade)
    - `test_scope_add.c` (escopos + alocaÃ§Ã£o)
    - `test_lookup_symbol.c` (lookup hierÃ¡rquico)
    - `test_ast_structure.c` (AST: create/print/free)
    - `test_switch.c`, `test_switch2.c` (switch/case/default)
    - `test_typedef.c` (typedef simples + uso como tipo)
    - `test_decl_complex.c` (stress: typedef + struct/union + ponteiros/arrays)
  - [ ] 21.1 Criar testes para declarações de variáveis
    - Testar variáveis globais de todos os tipos (int, char, float)
    - Testar variáveis locais em funções e blocos
    - Testar ponteiros de múltiplos níveis (int*, int**, int***)
    - Testar arrays unidimensionais e multidimensionais
    - Testar inicialização de variáveis
    - Verificar enriquecimento correto da symbol_table
    - _Requirements: 18.1, 18.3_

  - [ ] 21.2 Criar testes para declarações de funções
    - Testar função main obrigatória
    - Testar funções com diferentes tipos de retorno
    - Testar funções com múltiplos parâmetros
    - Testar funções sem parâmetros (void)
    - Testar funções com corpo vazio e com instruções
    - Verificar enriquecimento correto da symbol_table
    - _Requirements: 18.1, 18.3_

  - [ ] 21.3 Criar testes para estruturas de controle
    - Testar if com e sem else
    - Testar while, for, do-while
    - Testar switch-case-default
    - Testar break, continue, return
    - Testar blocos aninhados
    - Verificar construção correta da AST
    - _Requirements: 18.4_

  - [ ] 21.4 Criar testes para expressões
    - Testar expressões aritméticas com precedência correta
    - Testar expressões lógicas e relacionais
    - Testar operadores de atribuição
    - Testar operadores unários
    - Testar acesso a arrays e structs
    - Testar chamadas de função
    - Verificar construção correta da AST
    - _Requirements: 18.5_

  - [ ] 21.5 Criar testes para structs e unions
    - Testar declaração de struct com corpo
    - Testar declaração de struct sem corpo
    - Testar union
    - Testar structs aninhados
    - Testar arrays de structs
    - Verificar cálculo correto de tamanhos
    - _Requirements: 18.7_

  - [ ] 21.6 Criar testes para erros sintáticos
    - Testar tokens inesperados
    - Testar falta de ponto-e-vírgula
    - Testar parênteses/chaves não balanceados
    - Testar declarações inválidas
    - Verificar mensagens de erro corretas
    - Verificar recuperação de erro (modo pânico)
    - _Requirements: 18.2_

  - [ ] 21.7 Criar testes para gerenciamento de escopos
    - Testar variáveis globais vs locais com mesmo nome
    - Testar escopos aninhados (blocos dentro de funções)
    - Testar parâmetros de função
    - Testar campos de struct
    - Verificar scope_id correto para cada símbolo
    - _Requirements: 18.3_

  - [ ] 21.8 Criar testes para arrays multidimensionais
    - Testar arrays 2D, 3D, até 8D
    - Testar acesso a elementos de arrays multidimensionais
    - Testar inicialização de arrays multidimensionais
    - Verificar cálculo correto de tamanhos
    - _Requirements: 18.6_


- [ ] 22. Criar Manual do Usuário
  - [ ] 22.1 Escrever seção de introdução
    - Explicar o que é o parser e sua função no compilador
    - Explicar a relação com o Lexer (Fase 1)
    - Explicar o formato de entrada (arquivo .c)
    - Explicar o formato de saída (Symbol_Table + AST)
    - _Requirements: 17.1, 17.2_

  - [ ] 22.2 Escrever seção de exemplos de uso
    - Fornecer exemplo de programa C válido simples
    - Fornecer exemplo de programa C com funções
    - Fornecer exemplo de programa C com structs
    - Fornecer exemplo de programa C com arrays
    - Mostrar saída do parser para cada exemplo
    - _Requirements: 17.3_

  - [ ] 22.3 Escrever seção de mensagens de erro
    - Listar tipos de erros sintáticos detectados
    - Fornecer exemplos de código com erro
    - Mostrar mensagem de erro correspondente
    - Explicar como interpretar linha e coluna do erro
    - Explicar como corrigir erros comuns
    - _Requirements: 17.4, 17.5_

  - [ ] 22.4 Formatar e revisar Manual do Usuário
    - Garantir 4-8 páginas conforme requisito acadêmico
    - Adicionar índice e numeração de páginas
    - Adicionar exemplos visuais (tabelas, diagramas)
    - Revisar português e clareza
    - Gerar PDF final
    - _Requirements: 17.1, 17.2, 17.3, 17.4, 17.5_


- [ ] 23. Criar Manual do Programador
  - [ ] 23.1 Escrever seção de arquitetura do parser
    - Documentar estrutura de diretórios e arquivos
    - Documentar estruturas de dados principais (parser_t, ast_node_t, scope_t, symbol_info_t)
    - Documentar fluxo de execução do parser
    - Incluir diagrama de arquitetura
    - _Requirements: 16.1_

  - [ ] 23.2 Escrever seção de gramática original
    - Documentar gramática EBNF original completa
    - Identificar todas as regras com recursividade à esquerda
    - Identificar ambiguidades na gramática original
    - Explicar por que transformações são necessárias
    - _Requirements: 16.1, 16.2_

  - [ ] 23.3 Escrever seção de transformações gramaticais
    - Documentar eliminação de recursividade à esquerda imediata
    - Documentar eliminação de recursividade à esquerda indireta
    - Documentar fatoração de gramática
    - Mostrar gramática transformada completa
    - Explicar padrão de produções auxiliares com sufixo "_r"
    - _Requirements: 16.2, 16.3_

  - [ ] 23.4 Escrever seção de FIRST sets
    - Documentar algoritmo de cálculo de FIRST sets
    - Listar FIRST sets de todos os não-terminais
    - Documentar tabela de parsing preditivo
    - Explicar como FIRST sets garantem parsing LL(1)
    - _Requirements: 16.4_

  - [ ] 23.5 Escrever seção de mapeamento gramática-código
    - Documentar mapeamento entre não-terminais e funções C
    - Listar todas as funções de parsing com assinaturas
    - Explicar convenção de nomenclatura (parse_*)
    - Fornecer exemplos de implementação de funções
    - Documentar funções auxiliares (_r)
    - _Requirements: 16.5_

  - [ ] 23.6 Escrever seção de enriquecimento da tabela de símbolos
    - Documentar campos adicionados aos tokens
    - Explicar cálculo de endereços de memória
    - Explicar cálculo de tamanhos de tipos
    - Documentar gerenciamento de escopos
    - Fornecer exemplos de enriquecimento
    - _Requirements: 16.1_

  - [ ] 23.7 Escrever seção de construção da AST
    - Documentar tipos de nós da AST
    - Explicar hierarquia de nós
    - Documentar funções de construção de AST
    - Fornecer exemplos de AST para programas simples
    - _Requirements: 16.1_

  - [ ] 23.8 Escrever seção de tratamento de erros
    - Documentar estratégia de modo pânico
    - Documentar pontos de sincronização
    - Explicar recuperação de erros
    - Fornecer exemplos de mensagens de erro
    - _Requirements: 16.1_

  - [ ] 23.9 Formatar e revisar Manual do Programador
    - Garantir 8-16 páginas conforme requisito acadêmico
    - Adicionar índice e numeração de páginas
    - Adicionar diagramas e tabelas
    - Revisar português e clareza técnica
    - Gerar PDF final
    - _Requirements: 16.1, 16.2, 16.3, 16.4, 16.5_


- [ ] 24. Integração final e validação completa
  - [ ] 24.1 Integrar parser com lexer existente
    - Verificar compatibilidade de interfaces
    - Testar fluxo completo: arquivo .c → Lexer → Parser → AST
    - Verificar que symbol_table é corretamente compartilhada
    - Ajustar tipos de tokens se necessário
    - _Requirements: 15.3, 15.4_

  - [ ] 24.2 Executar suite completa de testes
    - Executar todos os testes criados na tarefa 21
    - Verificar que todos os testes passam
    - Corrigir bugs encontrados
    - Documentar resultados dos testes
    - _Requirements: 18.1, 18.2, 18.3, 18.4, 18.5, 18.6, 18.7, 18.8_

  - [ ] 24.3 Validar performance do parser
    - Testar com arquivo de 10.000 linhas
    - Medir tempo de execução (deve ser < 5 segundos)
    - Medir uso de memória (deve ser < 100 MB)
    - Otimizar se necessário
    - _Requirements: 20.1, 20.2, 20.3, 20.4, 20.5_

  - [ ] 24.4 Validar gerenciamento de memória
    - Executar parser com valgrind ou ferramenta similar
    - Verificar que não há vazamentos de memória
    - Verificar que toda memória alocada é liberada
    - Corrigir vazamentos se encontrados
    - _Requirements: 19.1, 19.2, 19.3, 19.4, 19.5_

  - [ ] 24.5 Preparar entrega final
    - Compilar todos os módulos do parser
    - Gerar executável final do compilador (Lexer + Parser)
    - Testar executável com programas de exemplo
    - Preparar arquivo README com instruções de compilação e uso
    - Preparar apresentação para defesa (21-05-2026)
    - _Requirements: 15.1, 15.2, 15.3, 15.4, 15.5, 15.6_


## Task Dependencies

```
1 → 2 → 3 → 4 → 5 → 6 → 7 → 8 → 9 → 10 (checkpoint)
                                    ↓
                              11 → 12 → 13 (checkpoint)
                                    ↓
                              14 → 15 → 16 → 17
                                    ↓
                              18 → 19 → 20
                                    ↓
                              21 (testes)
                                    ↓
                              22 (Manual Usuário) + 23 (Manual Programador)
                                    ↓
                              24 (Integração final)
```

## Notas Importantes

### Tabela de Declarações (Escopo Global e Local)

A **tabela de declarações** mencionada pelo usuário é implementada através do sistema de **gerenciamento de escopos** (tasks 4.x) e **enriquecimento da tabela de símbolos** (tasks 6.x):

1. **Escopo Global (scope_id = 0):**
   - Criado em `parse_programa()` (task 17.5)
   - Contém declarações globais de variáveis (task 14.2)
   - Contém declarações de funções (task 15.2)
   - Contém declarações de structs/unions (task 16.x)
   - Contém declarações typedef (task 17.4)

2. **Escopos Locais (scope_id > 0):**
   - Criados em `parse_bloco()` (task 11.3) e `parse_declaracao_funcao()` (task 15.2)
   - Contêm declarações locais de variáveis (task 11.3)
   - Contêm parâmetros de função (task 15.1)
   - Hierarquia de escopos mantida via `scope_t->parent`

3. **Enriquecimento da Symbol_Table:**
   - Cada símbolo recebe: `data_type`, `variable_type` (GLOBAL/LOCAL/PARAMETER), `scope_id`, `memory_address`, `size_bytes`
   - Arrays recebem: `is_array`, `array_dimensions`, `array_dim_count`
   - Funções recebem: `is_function`, `function_return_type`, `function_param_types`, `function_param_count`

### Ordem de Implementação Recomendada

1. **Fase 1 (Tasks 1-10):** Infraestrutura, estruturas de dados, expressões
2. **Checkpoint 1:** Validar parsing de expressões
3. **Fase 2 (Tasks 11-13):** Instruções e estruturas de controle
4. **Checkpoint 2:** Validar parsing de instruções
5. **Fase 3 (Tasks 14-17):** Declarações (variáveis, funções, structs, programa completo)
6. **Fase 4 (Tasks 18-20):** Erros, integração, debug
7. **Fase 5 (Task 21):** Testes completos
8. **Fase 6 (Tasks 22-23):** Documentação (manuais)
9. **Fase 7 (Task 24):** Integração final e validação

### Deadline e Defesa

- **Data de entrega:** 20-05-2026
- **Data de defesa:** 21-05-2026
- **Entregáveis:**
  - Código-fonte completo do parser
  - Manual do Usuário (4-8 páginas)
  - Manual do Programador (8-16 páginas) com gramática fatorada e eliminação de recursividade
  - Executável funcional do compilador (Lexer + Parser)

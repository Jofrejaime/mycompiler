# Requirements Document

## Introduction

Este documento especifica os requisitos para a Fase 2 do compilador: **Análise Sintática (Parser)**. O analisador sintático valida a sintaxe do código-fonte C através do método Top-Down Recursivo Descendente sem backtracking, consumindo tokens da tabela de símbolos gerada pelo Lexer (Fase 1) e enriquecendo-a com informações semânticas necessárias para as fases subsequentes do compilador.

O parser implementa a gramática EBNF simplificada de C, transformada para eliminar ambiguidades, recursividade à esquerda e preparada para análise preditiva LL(1).

## Glossary

- **Parser**: O analisador sintático que valida a estrutura gramatical do código-fonte
- **Lexer**: O analisador léxico (Fase 1) que gera tokens
- **Token**: Unidade léxica identificada pelo Lexer (tipo + lexema + posição)
- **Symbol_Table**: Tabela de símbolos compartilhada entre Lexer e Parser (estructura tabla_simbolos_t)
- **Grammar**: Conjunto de regras de produção EBNF que define a sintaxe válida de C
- **AST**: Abstract Syntax Tree - árvore sintática abstrata representando a estrutura do programa
- **FIRST_Set**: Conjunto dos primeiros terminais que podem iniciar uma produção gramatical
- **Recursive_Descent**: Método de parsing onde cada não-terminal é implementado como função void
- **Syntax_Error**: Erro detectado quando a sequência de tokens não obedece à gramática
- **Scope**: Contexto de visibilidade de variáveis (global, função, bloco)
- **Memory_Address**: Endereço de memória atribuído a variáveis durante análise sintática
- **Data_Type**: Tipo de dados de uma variável (int, char, float, void, struct, union)
- **Variable_Type**: Classificação da variável (local, global, parâmetro, campo de struct)

## Requirements

### Requirement 1: Parser Initialization and Token Consumption

**User Story:** Como desenvolvedor do compilador, eu quero que o Parser seja inicializado com a tabela de símbolos do Lexer, para que possa consumir tokens sequencialmente durante a análise sintática.

#### Acceptance Criteria

1. THE Parser SHALL receber como entrada a estrutura tabla_simbolos_t preenchida pelo Lexer
2. THE Parser SHALL manter um ponteiro para a posição atual de leitura na Symbol_Table
3. WHEN o Parser solicita o próximo token, THE Parser SHALL incrementar a posição de leitura na Symbol_Table
4. WHEN a posição de leitura atinge o final da Symbol_Table, THE Parser SHALL retornar TK_EOF
5. THE Parser SHALL permitir consultar o token atual sem consumi-lo (lookahead)

### Requirement 2: Grammar Rule Implementation via Recursive Descent

**User Story:** Como desenvolvedor do compilador, eu quero que cada não-terminal da gramática seja implementado como uma função void, para que o parser siga o método Top-Down Recursivo Descendente.

#### Acceptance Criteria

1. THE Parser SHALL implementar cada não-terminal da Grammar como uma função void em C
2. THE Parser SHALL nomear funções de parsing com prefixo "parse_" seguido do nome do não-terminal (ex: parse_programa, parse_declaracao_global)
3. WHEN uma função de parsing é chamada, THE Parser SHALL consumir tokens conforme a regra de produção correspondente
4. WHEN uma função de parsing detecta sintaxe inválida, THE Parser SHALL reportar Syntax_Error com linha e coluna
5. THE Parser SHALL implementar no mínimo as seguintes funções: parse_programa, parse_declaracao_global, parse_declaracao_variavel, parse_declaracao_funcao, parse_bloco, parse_instrucao, parse_expressao

### Requirement 3: Left Recursion Elimination

**User Story:** Como desenvolvedor do compilador, eu quero que a gramática seja transformada para eliminar recursividade à esquerda, para que o parser Recursive_Descent funcione sem backtracking.

#### Acceptance Criteria

1. THE Grammar SHALL ser analisada para identificar regras com recursividade à esquerda imediata
2. THE Grammar SHALL ser analisada para identificar regras com recursividade à esquerda indireta
3. THE Parser SHALL implementar regras transformadas usando sufixo "_r" para produções auxiliares (ex: aditivo_r, multiplicativo_r)
4. THE Parser SHALL garantir que nenhuma função de parsing chame a si mesma como primeira ação
5. FOR ALL regras de expressões aritméticas, THE Parser SHALL usar transformação iterativa com produções auxiliares

### Requirement 4: FIRST Set Analysis and Predictive Parsing

**User Story:** Como desenvolvedor do compilador, eu quero que o parser use análise FIRST para decidir qual produção seguir, para que possa fazer parsing preditivo sem backtracking.

#### Acceptance Criteria

1. WHEN uma regra gramatical tem múltiplas produções alternativas, THE Parser SHALL usar o token atual para decidir qual produção seguir
2. THE Parser SHALL documentar os FIRST_Set de cada não-terminal no manual do programador
3. WHEN o token atual não pertence ao FIRST_Set de nenhuma produção válida, THE Parser SHALL reportar Syntax_Error
4. THE Parser SHALL implementar decisões de parsing usando estruturas switch/case baseadas no tipo de token
5. THE Parser SHALL garantir que os FIRST_Set de produções alternativas sejam disjuntos

### Requirement 5: Symbol Table Enrichment - Variable Declarations

**User Story:** Como desenvolvedor do compilador, eu quero que o parser grave informações adicionais na tabela de símbolos durante declarações de variáveis, para que a fase de análise semântica tenha dados completos.

#### Acceptance Criteria

1. WHEN o Parser reconhece uma declaração de variável, THE Parser SHALL gravar o Data_Type na Symbol_Table
2. WHEN o Parser reconhece uma declaração de variável, THE Parser SHALL gravar o Variable_Type (local/global/parâmetro) na Symbol_Table
3. WHEN o Parser reconhece uma declaração de variável, THE Parser SHALL gravar o Scope atual na Symbol_Table
4. WHEN o Parser reconhece uma declaração de variável, THE Parser SHALL calcular e gravar o tamanho em bytes na Symbol_Table
5. WHEN o Parser reconhece uma declaração de variável, THE Parser SHALL atribuir um Memory_Address sequencial na Symbol_Table

### Requirement 6: Symbol Table Enrichment - Function Declarations

**User Story:** Como desenvolvedor do compilador, eu quero que o parser grave informações de funções na tabela de símbolos, para que chamadas de função possam ser validadas semanticamente.

#### Acceptance Criteria

1. WHEN o Parser reconhece uma declaração de função, THE Parser SHALL gravar o tipo de retorno na Symbol_Table
2. WHEN o Parser reconhece uma declaração de função, THE Parser SHALL gravar a lista de tipos de parâmetros na Symbol_Table
3. WHEN o Parser reconhece uma declaração de função, THE Parser SHALL gravar a quantidade de parâmetros na Symbol_Table
4. WHEN o Parser reconhece a função main, THE Parser SHALL marcar como ponto de entrada do programa na Symbol_Table
5. THE Parser SHALL criar um novo Scope ao entrar no corpo de uma função

### Requirement 7: Scope Management

**User Story:** Como desenvolvedor do compilador, eu quero que o parser gerencie escopos hierárquicos, para que variáveis locais e globais sejam distinguidas corretamente.

#### Acceptance Criteria

1. THE Parser SHALL manter uma pilha de Scope durante a análise sintática
2. WHEN o Parser entra em um bloco (TOKEN_LBRACE), THE Parser SHALL criar um novo Scope e empilhá-lo
3. WHEN o Parser sai de um bloco (TOKEN_RBRACE), THE Parser SHALL desempilhar o Scope atual
4. THE Parser SHALL associar cada variável declarada ao Scope atual no topo da pilha
5. THE Parser SHALL numerar Scope de forma única e sequencial (escopo_0 = global, escopo_1, escopo_2, ...)

### Requirement 8: Program Structure Recognition

**User Story:** Como desenvolvedor do compilador, eu quero que o parser reconheça a estrutura completa de um programa C, para que valide programas sintaticamente corretos.

#### Acceptance Criteria

1. THE Parser SHALL reconhecer diretivas de pré-processador (#include, #define)
2. THE Parser SHALL reconhecer declarações globais de variáveis
3. THE Parser SHALL reconhecer declarações de funções com corpo
4. THE Parser SHALL reconhecer a função main como obrigatória
5. THE Parser SHALL reconhecer declarações typedef
6. THE Parser SHALL reconhecer declarações struct e union

### Requirement 9: Statement Recognition

**User Story:** Como desenvolvedor do compilador, eu quero que o parser reconheça todas as estruturas de controle de C, para que valide comandos sintaticamente corretos.

#### Acceptance Criteria

1. THE Parser SHALL reconhecer instruções if com ramo else opcional
2. THE Parser SHALL reconhecer loops while
3. THE Parser SHALL reconhecer loops for com três expressões opcionais
4. THE Parser SHALL reconhecer loops do-while
5. THE Parser SHALL reconhecer instruções switch-case-default
6. THE Parser SHALL reconhecer instruções break, continue, return
7. THE Parser SHALL reconhecer blocos de comandos delimitados por chaves
8. THE Parser SHALL reconhecer instruções de expressão terminadas por ponto-e-vírgula

### Requirement 10: Expression Recognition

**User Story:** Como desenvolvedor do compilador, eu quero que o parser reconheça expressões C com precedência correta de operadores, para que valide expressões sintaticamente corretas.

#### Acceptance Criteria

1. THE Parser SHALL reconhecer expressões aritméticas (+, -, *, /, %)
2. THE Parser SHALL reconhecer expressões relacionais (<, >, <=, >=, ==, !=)
3. THE Parser SHALL reconhecer expressões lógicas (&&, ||, !)
4. THE Parser SHALL reconhecer operadores de atribuição (=, +=, -=, *=, /=)
5. THE Parser SHALL reconhecer operadores unários (-, !, &, *, ++, --)
6. THE Parser SHALL reconhecer acesso a arrays com colchetes (expr[expr])
7. THE Parser SHALL reconhecer acesso a membros de struct com ponto (expr.id)
8. THE Parser SHALL reconhecer chamadas de função com argumentos (id(args))
9. THE Parser SHALL implementar precedência de operadores conforme especificação C
10. THE Parser SHALL reconhecer expressões primárias (identificadores, literais, expressões entre parênteses)

### Requirement 11: Array Declaration Recognition

**User Story:** Como desenvolvedor do compilador, eu quero que o parser reconheça declarações de arrays unidimensionais e multidimensionais, para que valide arrays sintaticamente corretos.

#### Acceptance Criteria

1. THE Parser SHALL reconhecer declarações de arrays unidimensionais (tipo id[tamanho])
2. THE Parser SHALL reconhecer declarações de arrays multidimensionais (tipo id[tam1][tam2]...)
3. WHEN o Parser reconhece um array, THE Parser SHALL calcular o tamanho total em bytes (tipo * dimensões)
4. WHEN o Parser reconhece um array, THE Parser SHALL gravar as dimensões na Symbol_Table
5. THE Parser SHALL reconhecer inicialização de arrays com listas de valores

### Requirement 12: Struct and Union Recognition

**User Story:** Como desenvolvedor do compilador, eu quero que o parser reconheça declarações de struct e union, para que valide tipos compostos sintaticamente corretos.

#### Acceptance Criteria

1. THE Parser SHALL reconhecer declarações de struct com corpo (struct nome { campos })
2. THE Parser SHALL reconhecer declarações de union com corpo (union nome { campos })
3. THE Parser SHALL reconhecer declarações de struct sem corpo (struct nome)
4. WHEN o Parser reconhece um struct, THE Parser SHALL calcular o tamanho total somando tamanhos dos campos
5. WHEN o Parser reconhece um union, THE Parser SHALL calcular o tamanho como o máximo dos campos
6. THE Parser SHALL reconhecer campos de struct/union com tipos primitivos e compostos

### Requirement 13: Syntax Error Reporting

**User Story:** Como usuário do compilador, eu quero que o parser reporte erros sintáticos de forma clara, para que possa corrigir meu código-fonte.

#### Acceptance Criteria

1. WHEN o Parser detecta um Syntax_Error, THE Parser SHALL reportar a linha e coluna do erro
2. WHEN o Parser detecta um Syntax_Error, THE Parser SHALL reportar o token encontrado
3. WHEN o Parser detecta um Syntax_Error, THE Parser SHALL reportar o token esperado
4. WHEN o Parser detecta um Syntax_Error, THE Parser SHALL exibir mensagem descritiva em português
5. THE Parser SHALL continuar a análise após erro (modo pânico) até encontrar ponto de sincronização (;, }, EOF)

### Requirement 14: AST Construction

**User Story:** Como desenvolvedor do compilador, eu quero que o parser construa uma Abstract Syntax Tree, para que as fases subsequentes processem a estrutura do programa.

#### Acceptance Criteria

1. THE Parser SHALL construir nós de AST durante o parsing
2. THE Parser SHALL criar nós de AST para declarações (variáveis, funções, structs)
3. THE Parser SHALL criar nós de AST para instruções (if, while, for, return, expressões)
4. THE Parser SHALL criar nós de AST para expressões com operadores e operandos
5. THE Parser SHALL retornar a raiz da AST ao final do parsing bem-sucedido
6. THE Parser SHALL liberar memória da AST quando não for mais necessária

### Requirement 15: Parser Output and Integration

**User Story:** Como desenvolvedor do compilador, eu quero que o parser produza saída estruturada, para que possa integrar com as fases subsequentes.

#### Acceptance Criteria

1. THE Parser SHALL retornar código de sucesso (0) quando o parsing é bem-sucedido
2. THE Parser SHALL retornar código de erro (não-zero) quando há Syntax_Error
3. THE Parser SHALL exportar a Symbol_Table enriquecida para a fase de análise semântica
4. THE Parser SHALL exportar a AST para a fase de análise semântica
5. THE Parser SHALL imprimir a Symbol_Table enriquecida em modo debug
6. THE Parser SHALL imprimir a AST em formato textual em modo debug

### Requirement 16: Parser Grammar Transformation Documentation

**User Story:** Como desenvolvedor do compilador, eu quero documentação completa das transformações gramaticais, para que possa entender e manter o parser.

#### Acceptance Criteria

1. THE Manual_Programador SHALL documentar a gramática original EBNF
2. THE Manual_Programador SHALL documentar todas as regras com recursividade à esquerda identificadas
3. THE Manual_Programador SHALL documentar as transformações aplicadas para eliminar recursividade
4. THE Manual_Programador SHALL documentar os FIRST_Set calculados para cada não-terminal
5. THE Manual_Programador SHALL documentar o mapeamento entre não-terminais e funções de parsing

### Requirement 17: Parser User Manual

**User Story:** Como usuário do compilador, eu quero um manual explicando como usar o parser, para que possa compilar meus programas C.

#### Acceptance Criteria

1. THE Manual_Usuario SHALL explicar o formato de entrada do Parser (arquivo .c)
2. THE Manual_Usuario SHALL explicar o formato de saída do Parser (Symbol_Table + AST)
3. THE Manual_Usuario SHALL fornecer exemplos de programas C válidos
4. THE Manual_Usuario SHALL fornecer exemplos de erros sintáticos e suas mensagens
5. THE Manual_Usuario SHALL explicar como interpretar mensagens de erro sintático

### Requirement 18: Parser Testing and Validation

**User Story:** Como desenvolvedor do compilador, eu quero validar o parser com casos de teste, para que garanta correção da análise sintática.

#### Acceptance Criteria

1. THE Parser SHALL ser testado com programas C sintaticamente corretos
2. THE Parser SHALL ser testado com programas C com erros sintáticos conhecidos
3. THE Parser SHALL ser testado com declarações de variáveis de todos os tipos
4. THE Parser SHALL ser testado com todas as estruturas de controle (if, while, for, do-while, switch)
5. THE Parser SHALL ser testado com expressões complexas aninhadas
6. THE Parser SHALL ser testado com arrays multidimensionais
7. THE Parser SHALL ser testado com structs e unions aninhados
8. THE Parser SHALL ser testado com a função main obrigatória

### Requirement 19: Memory Management

**User Story:** Como desenvolvedor do compilador, eu quero que o parser gerencie memória corretamente, para que não haja vazamentos de memória.

#### Acceptance Criteria

1. THE Parser SHALL alocar memória dinamicamente para nós da AST
2. THE Parser SHALL alocar memória dinamicamente para estruturas de Scope
3. THE Parser SHALL liberar toda memória alocada ao finalizar
4. WHEN ocorre um Syntax_Error, THE Parser SHALL liberar memória parcialmente alocada antes de retornar
5. THE Parser SHALL usar funções de alocação seguras (malloc, calloc, realloc)

### Requirement 20: Parser Performance

**User Story:** Como usuário do compilador, eu quero que o parser seja eficiente, para que compile programas rapidamente.

#### Acceptance Criteria

1. THE Parser SHALL processar arquivos de até 10.000 linhas em menos de 5 segundos
2. THE Parser SHALL consumir no máximo 100 MB de memória para programas de até 10.000 linhas
3. THE Parser SHALL fazer no máximo uma passagem sobre a sequência de tokens
4. THE Parser SHALL evitar cópias desnecessárias de dados durante o parsing
5. THE Parser SHALL usar estruturas de dados eficientes para a pilha de Scope


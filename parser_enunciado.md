O que o trabalho pede
🎯 Objetivo geral

Construir um mini analisador sintático (parser) — ou seja, a segunda fase de um compilador — capaz de:

Validar a sintaxe de um subconjunto de código-fonte
Trabalhar com base nos tokens gerados pelo analisador léxico
🔗 Relação com o analisador léxico

O analisador sintático:

Depende diretamente do lexer
Solicita tokens e lexemas
Utiliza a tabela de símbolos gerada

Além disso, ao analisar, deve enriquecer a tabela de símbolos com informações como:

Endereço de memória
Tipo de dados
Tipo de variável
Valor atribuído
Escopo da variável
Tamanho em bytes

Essas informações serão usadas posteriormente na análise semântica

🧠 O que o parser deve reconhecer
📌 Para linguagem C:
Função principal (main)
Declarações (variáveis e funções)
Tipos de dados
Estruturas de controlo:
Seleção (if, else)
Repetição (while, for, etc.)
Operadores:
Aritméticos
Relacionais
Lógicos
Unários
Comandos de entrada/saída
Blocos { }
Arrays (1D e multidimensionais)
struct e union
Expressões (aritméticas, relacionais, lógicas, unárias)
📌 Para linguagem Java:
package, import
Declaração de classes
Declaração de variáveis e métodos
Estruturas de controlo
Arrays
Operadores (mesmas categorias do C)
Blocos
Expressões
Modificadores (ex: public, private)
⚙️ Método de construção do parser

O parser deve ser:

Top-down (recursivo descendente) sem backtracking

🔧 Transformações obrigatórias na gramática

Antes de implementar, o estudante deve:

a) Eliminar ambiguidades
b) Eliminar recursividade à esquerda
c) Eliminar recursividade indireta (segunda ordem)
d) Analisar o primeiro símbolo (lookahead)
e) Factorizar regras gramaticais
🧩 Implementação
Cada não-terminal da gramática deve ser implementado como:
Uma função (void) em C
Um método/classe em linguagens OO
📚 Pesquisa obrigatória

Cada estudante deve:

Pesquisar a gramática da linguagem escolhida

📦 Entregáveis

1. Manual do utilizador (4–8 páginas)

Deve conter:

Explicação de como usar o analisador
Formato de entrada e saída do programa
2. Manual do programador (8–16 páginas)

Deve conter:

Factorização da gramática
Eliminação da recursividade à esquerda
⚠️ Observações
Trabalho individual
Cópias → anuladas
Linguagens permitidas:
C, C++, Java, C#
📅 Datas
Entrega: 25-05-2026 até 23:55
Defesa: 26-05-2026
🧨 Nota importante do enunciado

O analisador sintático depende do analisador léxico

Ou seja:

👉 Se o lexer estiver errado, o parser não funciona

🧠 Resumo fiel

O trabalho consiste em:

✔ Construir um parser recursivo descendente
✔ Baseado numa gramática tratada (LL)
✔ Integrado com o lexer
✔ Capaz de validar sintaxe
✔ E preparar dados para análise semântica


// Arquivo de teste para o Analisador Léxico
// Este programa contém vários tipos de tokens para demonstração

#include <stdio.h>

/* Comentário em bloco:
   Teste do analisador léxico
   Deve reconhecer todos os tokens
*/

int fibonacci(int n) {
    if (n <= 1)
        return n;
    else
        return fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
    int x = 10;
    int y = 20;
    float resultado = 3.14;
    double valor_exp = 1.5e-3;
    
    char letra = 'A';
    char *mensagem = "Hello, World!";
    
    if (x < y) {
        printf("x é menor que y\n");
    } else if (x > y) {
        printf("x é maior que y\n");
    } else {
        printf("x é igual a y\n");
    }
    
    // Loop simples
    while (x < y) {
        x = x + 1;
    }
    
    // Loop for
    for (int i = 0; i < 10; i++) {
        printf("Número: %d\n", i);
    }
    
    // Operadores diversos
    int a = 5, b = 3;
    int soma = a + b;
    int diferenca = a - b;
    int produto = a * b;
    int divisao = a / b;
    int modulo = a % b;
    
    // Operadores relacionais
    int teste1 = (a == b);
    int teste2 = (a != b);
    int teste3 = (a > b);
    int teste4 = (a >= b);
    int teste5 = (a < b);
    int teste6 = (a <= b);
    
    // Operadores lógicos
    int teste_and = (x > 0 && y > 0);
    int teste_or = (x < 0 || y < 0);
    int teste_not = !(x == 0);
    
    // Operadores bitwise
    int bit_and = a & b;
    int bit_or = a | b;
    int bit_xor = a ^ b;
    int bit_not = ~a;
    int shift_left = a << 2;
    int shift_right = b >> 1;
    
    // Operadores unários
    x++;
    y--;
    ++x;
    --y;
    
    // Operadores de atribuição composta
    soma += 5;
    diferenca -= 3;
    produto *= 2;
    divisao /= 2;
    
    // Estrutura simples
    struct Pessoa {
        char nome[50];
        int idade;
        float altura;
    };
    
    // Typedef
    typedef struct Pessoa Pessoa;
    
    // Switch/case
    int opcao = 2;
    switch (opcao) {
        case 1:
            printf("Opção 1\n");
            break;
        case 2:
            printf("Opção 2\n");
            break;
        default:
            printf("Opção inválida\n");
    }
    
    // Do-while
    do {
        x--;
    } while (x > 0);
    
    // Continue e break
    for (int j = 0; j < 20; j++) {
        if (j == 5)
            continue;
        if (j == 15)
            break;
    }
    
    // Sizeof
    int tamanho = sizeof(int);
    
    // Ponteiros
    int *ptr = &a;
    int valor_ptr = *ptr;
    
    // Arrow operator
    // struct_ptr->campo;
    
    // Goto (evitar em código real!)
    // goto saida;
    // saida:
    
    return 0;
}

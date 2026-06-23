/* TEST: functions.c
   Cobre: protótipos, funções completas, parâmetros sem nome, retorno ponteiro */
#include <stdio.h>
/* Protótipos com nomes */
int    soma(int a, int b);
void   imprime(char *msg);
double media(double x, double y);

/* Protótipos SEM nomes (TAREFA 4) */
int    abs_val(int);
float  pot(float, int);
void   noop(void);

/* Função com retorno void */
void imprime(char *msg) {
    return;
}

/* Função com retorno int */
int soma(int a, int b) {
    int resultado = a + b;
    return resultado;
}

/* Função com ponteiro de retorno */
int* busca(int *arr, int n, int val) {
    int i;
    i = 0;
    return arr;
}

/* Função sem parâmetros */
int conta() {
    return 0;
}

/* Função com parâmetro array */
int tamanho(char str[], int max) {
    return max;
}

/* Função recursiva */
int fatorial(int n) {
    if (n <= 1) return 1;
    return n * fatorial(n - 1);
}

/* Declaração forward depois de uso (válido em C) */
int main() {
    int x = soma(2, 3);
    return x;
}

/* ============================================================================
   TESTE SEMÂNTICO 05 — TIPOS NOS ARGUMENTOS DE FUNÇÕES
   
   Alínea d): verificação do número e tipo dos argumentos em chamadas de função.
   
   Erros esperados:
     - Linha 32: 'calcular': 3 argumentos fornecidos, mas a função espera 2
     - Linha 35: 'imprimir': 0 argumentos fornecidos, mas a função espera 1
   ============================================================================ */

/* Função que recebe dois inteiros e devolve a soma */
int calcular(int a, int b) {
    return a + b;
}

/* Função que recebe um float e devolve o dobro */
float dobro(float x) {
    return x * 2;
}

/* Função que recebe um char e imprime (simplificada) */
void imprimir(char c) {
    return;
}

int main() {
    int x = 10;
    int y = 5;
    float f = 3.14;
    char letra = 'Z';

    /* Chamadas correctas — sem erro */
    int r1 = calcular(x, y);
    float r2 = dobro(f);
    imprimir(letra);

    /* ERRO (alínea d): 3 argumentos fornecidos, função espera 2 */
    int r3 = calcular(x, y, x);

    /* ERRO (alínea d): 0 argumentos fornecidos, função espera 1 */
    imprimir();

    /* Chamada correcta com promoção de tipo — sem erro */
    float r4 = dobro(x);   /* int promovido para float: compatível */

    return r1;
}

/* ============================================================================
   TESTE SEMÂNTICO 04 — INCOMPATIBILIDADE DE TIPOS EM OPERAÇÕES
   
   Alínea c): incompatibilidade de tipos numa operação/expressão.
   
   Erros esperados:
     - Linha 22: operação aritmética entre 'void' e 'int' — incompatível
     - Linha 25: soma de dois ponteiros — inválida
   ============================================================================ */

void funcao_void() {
    return;
}

int soma(int a, int b) {
    return a + b;
}

int main() {
    int x = 10;
    int y = 5;

    /* Operações válidas — sem erro */
    int s = x + y;
    float f = x;    /* promoção int → float: válida */

    /* ERRO (alínea c): não se pode usar void numa operação aritmética. */
    int bad_op = x + funcao_void();

    /* ERRO (alínea c): soma de dois ponteiros inteiros — inválida em C */
    int *p1 = 0;
    int *p2 = 0;
    int *p3 = p1 + p2;

    /* Operação compatível: int + int → int */
    int r = soma(x, y);

    return r;
}

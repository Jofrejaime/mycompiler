/* ============================================================================
   TESTE SEMÂNTICO 09 — TIPOS DE ARGUMENTOS COM PONTEIROS E STRUCTS

   Alínea d): verificação do TIPO (incluindo nível de ponteiro) dos argumentos.
   Complementa sem05 (que testa o NÚMERO de argumentos).

   Erros esperados:
     - passar 'int' onde se espera 'int*'
     - passar 'int*' onde se espera 'int'

   Casos que devem PASSAR (sem erro):
     - passar 'struct Ponto*' onde se espera 'struct Ponto*'
     - passar 'int' onde se espera 'int' (com promoção)
   ============================================================================ */

struct Ponto {
    int x;
    int y;
};

/* Espera um ponteiro para int */
int usa_ptr(int *p) {
    return *p;
}

/* Espera um int por valor */
int usa_val(int v) {
    return v;
}

/* Espera um ponteiro para struct */
int usa_struct(struct Ponto *pt) {
    return pt->x;
}

int main() {
    int n = 10;
    int *pn = &n;
    struct Ponto ponto;

    /* Chamadas correctas — sem erro */
    int a = usa_ptr(pn);          /* int* → int*  : OK */
    int b = usa_val(n);           /* int  → int   : OK */
    int c = usa_struct(&ponto);   /* struct* → struct* : OK */

    /* ERRO (alínea d): int fornecido onde se espera int* */
    int d = usa_ptr(n);

    /* ERRO (alínea d): int* fornecido onde se espera int */
    int e = usa_val(pn);

    return a + b + c + d + e;
}

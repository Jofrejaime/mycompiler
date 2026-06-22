/* ============================================================
   arrays_stress.c — Auditoria completa de arrays
   Cobre: A1 a A10
   ============================================================ */

/* A1: Array simples */
int a_simples[10];

/* A2: Array multidimensional */
int m_multi[10][20];

/* A3: Array de ponteiros */
int *ptrs[10];

/* A4: Inicializacao simples */
int a_init[3] = {1, 2, 3};

/* A5: Inicializacao parcial */
int a_parcial[5] = {1, 2};

/* A6: Inicializacao aninhada */
int m_nested[2][2] = {{1, 2}, {3, 4}};

/* A8: Arrays em parametros — definicao das funcoes */
void f_array_param(int a[10]) {
    return;
}

void g_matrix_param(int m[5][5]) {
    return;
}

/* A9: Arrays em struct */
struct Pessoa {
    char nome[50];
    int notas[10];
};

/* A10: 5 dimensoes */
int extremo[1][2][3][4][5];

/* A7: Acesso a arrays — dentro de funcao */
int main() {
    /* A1 local */
    int loc[10];

    /* A2 local multidimensional */
    int mat[4][4];

    /* A7: acesso */
    int i = 0;
    int j = 0;
    int v;
    v = loc[i];
    v = mat[i][j];
    v = ptrs[i];

    /* A7: acesso encadeado */
    v = m_multi[i][j];

    /* A3: array de ponteiros local */
    int *lptrs[5];
    v = lptrs[0];

    /* A4 local */
    int linit[3] = {10, 20, 30};

    /* A6 local aninhado */
    int lnest[2][2] = {{5, 6}, {7, 8}};

    return v;
}

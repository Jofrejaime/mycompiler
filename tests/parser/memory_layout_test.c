/* memory_layout_test.c — Parte 1, 2 e 5 da auditoria
   Testa: double, structs com double, layout de memoria, valores literais */

/* === PARTE 1: DOUBLE SIMPLES === */
double gd_a;
double gd_b;

/* Esperado: gd_a addr=0 size=8, gd_b addr=8 size=8 */

/* === PARTE 1: DOUBLE ARRAY === */
double gd_vec[10];
double gd_mat[10][20];

/* Esperado: gd_vec size=80, gd_mat size=1600 */

/* === PARTE 2: STRUCT A - double simples === */
struct A {
    double x;
};

/* Esperado: campo x offset=0 size=8, total=8 */

/* === PARTE 2: STRUCT B - int + double === */
struct B {
    int a;
    double b;
};

/* Esperado: a offset=0 size=4, b offset=4 size=8, total=12 */

/* === PARTE 2: STRUCT C - char + double + int === */
struct C {
    char c;
    double d;
    int e;
};

/* Esperado: c offset=0 size=1, d offset=1 size=8, e offset=9 size=4, total=13 */

/* === PARTE 2: STRUCT D - array de double === */
struct D {
    double v[10];
};

/* Esperado: v offset=0 size=80, total=80 */

/* === PARTE 3: VALORES LITERAIS - globais === */
int    gi_const   = 42;
float  gf_const   = 3;
char   gc_const   = 65;
double gd_const   = 2;

/* === PARTE 5: LAYOUT CONSISTENTE === */
int g1;
int g2;
int g3;
char gc1;
double gd1;

/* Esperado sequencial:
   g1  addr=X   size=4
   g2  addr=X+4 size=4
   g3  addr=X+8 size=4
   gc1 addr=X+12 size=1
   gd1 addr=X+13 size=8  */

int main() {
    /* PARTE 1: double local */
    double ld_a;
    double ld_b;
    /* Esperado: ld_a offset=0 size=8, ld_b offset=8 size=8 */

    /* PARTE 1: double array local */
    double ld_vec[10];
    /* Esperado: size=80 */

    /* PARTE 3: valores literais locais */
    int   li_const   = 99;
    float lf_const   = 1;
    char  lc_const   = 65;

    /* PARTE 5: mixed types local */
    int    lx1;
    char   lx2;
    double lx3;
    int    lx4;

    return 0;
}

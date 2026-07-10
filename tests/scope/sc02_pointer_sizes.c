/* sc02_pointer_sizes.c — Suite: Escopo
   Categoria: Ponteiros (simples e multi-nível)
   Verifica: ponteiro=1 em Ponteiro; size=8 bytes (ptr 64-bit);
             int **pp → Ponteiro=2, size=8 */

int   *p_int;
char  *p_char;
float *p_float;
int  **pp_int;
int ***ppp_int;

int main() {
    int   *lp;
    int  **lpp;
    char  *lpc;
    return 0;
}

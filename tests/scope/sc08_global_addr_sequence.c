/* sc08_global_addr_sequence.c — Suite: Escopo
   Categoria: Endereço global sequencial (sem gaps, sem overlaps)
   Verifica: addr(sym_N+1) == addr(sym_N) + size(sym_N)
   Tipos mistos para forçar todos os tamanhos */

int    v1;   /* addr=0,  size=4  → próx=4  */
char   v2;   /* addr=4,  size=1  → próx=5  */
float  v3;   /* addr=5,  size=4  → próx=9  */
double v4;   /* addr=9,  size=8  → próx=17 */
int    v5;   /* addr=17, size=4  → próx=21 */
char   v6;   /* addr=21, size=1  → próx=22 */

/* sc09_union_sizes.c — Suite: Escopo
   Categoria: Unions — tamanho = maior campo
   Verifica:
     union U → size = max(tamanho de cada campo)
     variável do tipo union → size correto (B3 equivalente para unions) */

union U1 { int i; char c; };          /* size = max(4,1) = 4  */
union U2 { int i; double d; };        /* size = max(4,8) = 8  */
union U3 { int i; float f; char c; }; /* size = max(4,4,1) = 4 */
union U4 { double d; int arr[5]; };   /* size = max(8,20) = 20 */

union U1 u1;   /* size=4,  addr=0 */
union U2 u2;   /* size=8,  addr=4 */
int      gap;  /* size=4,  addr=12 */
union U3 u3;   /* size=4,  addr=16 */
int      last; /* size=4,  addr=20 — confirma sem overlap */

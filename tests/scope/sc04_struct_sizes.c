/* sc04_struct_sizes.c — Suite: Escopo
   Categoria: Structs — tamanho total e variáveis derivadas
   Verifica:
     struct:S  → size = soma dos campos
     variável  → size = size da struct (B3 fix)
     addr      → não sobrepõe a variável seguinte */

struct S1 { int x; };                      /* size=4  */
struct S2 { int x; int y; };               /* size=8  */
struct S3 { int x; char c; float f; };     /* size=9  */
struct S4 { double d; int n; char b; };    /* size=13 */

struct S1 vs1;   /* size=4,  addr=0  */
struct S2 vs2;   /* size=8,  addr=4  */
int    gap;      /* size=4,  addr=12 */
struct S3 vs3;   /* size=9,  addr=16 */
int    sentinel; /* size=4,  addr=25 — confirma que vs3 nao sobrepoe */

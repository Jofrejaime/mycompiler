/* sc05_typedef_namespace.c — Suite: Escopo
   Categoria: Namespace typedef vs struct tag (B2 fix)
   Verifica:
     typedef struct Ponto Ponto → registo como typedef (is_typedef=1)
     struct:Ponto              → registo separado como struct
     Ponto p                   → variável de tipo struct, size=8 */

typedef struct Ponto Ponto;

struct Ponto {
    int x;
    int y;
};

Ponto p1;           /* size=8, addr=0 */
struct Ponto p2;    /* size=8, addr=8 */

typedef struct Node {
    int val;
    int next;
} Node;

Node n1;            /* size=8 */
struct Node n2;     /* size=8 */

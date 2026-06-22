/* audit_final.c — Auditoria final pre-semantica
   Cobre: ARRAY_ACCESS, MEMBER_ACCESS, ARROW, typedef+struct, lookup */

/* === TYPEDEF STRUCT ANONIMA === */
typedef struct {
    int idade;
    char nome[50];
    float peso;
} Pessoa;

/* === TYPEDEF STRUCT NOMEADA === */
typedef struct Node {
    int valor;
    struct Node *proximo;
} Node;

/* === STRUCT SEM TYPEDEF === */
struct Ponto {
    int x;
    int y;
    int *dados;
    int coords[3];
};

/* === VARIAVEIS GLOBAIS === */
int g_arr[10];
int g_mat[10][20];
int *g_ptr;
Pessoa g_pessoa;
struct Ponto g_ponto;
Node g_node;

int main() {
    /* A1: ARRAY_ACCESS simples */
    int i = 0;
    int v = g_arr[i];

    /* A2: ARRAY_ACCESS duplo m[i][j] */
    int j = 1;
    int w = g_mat[i][j];

    /* A3: MEMBER_ACCESS DOT */
    int idade = g_pessoa.idade;
    int x     = g_ponto.x;

    /* A4: MEMBER_ACCESS ARROW */
    Pessoa *pp = &g_pessoa;
    int id2 = pp->idade;

    struct Ponto *qp = &g_ponto;
    int x2 = qp->x;

    /* A5: ARRAY dentro de struct via DOT */
    int c0 = g_ponto.coords[0];

    /* A6: ARRAY dentro de struct via ARROW */
    int c1 = qp->coords[1];

    /* A7: Chamada de funcao */
    int r = 0;

    /* A8: Node (typedef struct nomeada) */
    Node *np = &g_node;
    int val = np->valor;

    return r;
}

/* === FUNCOES PARA VALIDAR ASSINATURAS === */
int soma(int a, int b)          { return a + b; }
void nada(void)                 { return; }
int *busca(int *arr, int n)     { return arr; }
int tamanho(char s[], int max)  { return max; }
int proto_sem_nome(int, char);
int proto_sem_nome(int x, char c) { return x; }

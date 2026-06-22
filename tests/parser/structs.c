/* TEST: structs.c
   Cobre: struct simples, arrays, ponteiros, múltiplos declaradores, typedef, union */

/* Struct simples */
struct Ponto {
    int x;
    int y;
};

/* Struct com array */
struct Pessoa {
    char nome[64];
    int  idade;
    float peso;
};

/* Struct com ponteiro */
struct No {
    int        valor;
    struct No *proximo;
};

/* Struct com múltiplos declaradores por campo (TAREFA 2) */
struct Rect {
    int largura, altura;
    float *dados;
};

/* Union */
union Dado {
    int   inteiro;
    float real;
    char  byte;
};

/* Typedef struct */
typedef struct Ponto Ponto_t;

/* Typedef struct inline */
typedef struct {
    int r;
    int g;
    int b;
} Cor;

/* Variáveis de struct */
struct Ponto  origem;
struct Pessoa pessoas[10];
struct No    *lista;
Cor           fundo;

/* Sizeof de struct */
int main() {
    int s1 = sizeof(struct Ponto);
    int s2 = sizeof(int);
    int s3 = sizeof(char);
    int s4 = sizeof(double);
    return 0;
}

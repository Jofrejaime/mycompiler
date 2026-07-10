/* ============================================================================
   TESTE DE COBERTURA DA GRAMÁTICA — Parser × Gramatica.txt

   Exercita TODAS as produções da gramática. Este ficheiro deve ser aceite
   pelo compilador com ZERO erros (exit 0).

   Cada secção indica as produções cobertas.
   ============================================================================ */

/* <diretiva_preprocessador>: TK_PP_INCLUDE, TK_PP_DEFINE */
#include <stdio.h>
#define LIMITE 100

/* <declaracao_typedef> ::= KW_TYPEDEF <especificador_tipo> <asteriscos> TK_ID */
typedef int inteiro;
typedef char *texto;

/* <especificador_tipo> ::= KW_STRUCT <nome_ou_corpo_struct>
   <corpo_campos>, <lista_campos>, <campo>, <lista_declaradores_campo>,
   <mais_declaradores_campo> (multi-declaradores), <sufixo_array_opcional> */
struct Ponto {
    int x, y;
    float peso;
    char nome[16];
    int *ref;
    int matriz[2][3];
};

/* <especificador_tipo> ::= KW_UNION <nome_ou_corpo_struct> */
union Valor {
    int i;
    float f;
};

/* <declaracao_geral> só com tipo e ';' (resto_declaracao_geral → SYM_SEMICOLON) */
struct Ponto;

/* Protótipos: <cauda_declaracao_geral> com parâmetros nomeados e sem nome,
   <parametros_opcionais> ::= KW_VOID | <lista_parametros> | ε */
int soma(int a, int b);
float escala(float, int);
void nada(void);
int variavel_global;

/* Globais: <mais_declaradores> com múltiplos declaradores e inicialização,
   <asteriscos> (níveis de ponteiro), <sufixo_array_opcional> */
int g1 = 1, g2, *g3, g4[3];

/* <inicializacao_opcional> com init-list (D1) */
int tabela[3] = {10, 20, 30};

/* Função completa: <cauda_declaracao_geral> ::= '(' params ')' <bloco> */
int soma(int a, int b) {
    /* <declaracao_variavel_local> com <mais_declaradores> */
    int r = 0, tmp;

    /* <atribuicao_r>: todos os operadores de atribuição da gramática */
    tmp = 1;
    tmp += 2;
    tmp -= 1;
    tmp *= 3;
    tmp /= 2;
    tmp &= 7;
    tmp |= 8;
    tmp ^= 1;

    /* <ternario> */
    r = (a > b) ? a : b;

    /* <logico_ou>, <logico_e> */
    if (a > 0 && b > 0 || tmp != 0) {
        r = r + 1;
    }

    /* <bitor>, <bitxor>, <bitand> */
    r = (a | b) ^ (a & b);

    /* <igualdade>: OP_EQ, OP_NE */
    if (a == b) { r = 0; }
    if (a != b) { r = 1; }

    /* <relacional>: OP_LT, OP_GT, OP_LE, OP_GE */
    if (a < b)  { r = 2; }
    if (a > b)  { r = 3; }
    if (a <= b) { r = 4; }
    if (a >= b) { r = 5; }

    /* <shift> */
    r = a << 1;
    r = b >> 2;

    /* <aditivo>, <multiplicativo> */
    r = a + b - 1;
    r = a * b / 2 % 3;

    /* <unario>: OP_INC, OP_DEC, OP_NOT, OP_BITNOT, OP_MINUS, OP_PLUS,
       OP_BITAND (endereço), OP_MULT (deref) */
    ++tmp;
    --tmp;
    r = !a;
    r = ~a;
    r = -a;
    r = +b;
    g3 = &r;
    r = *g3;

    /* <unario_sizeof>: sizeof(tipo), sizeof(tipo*) (D3), sizeof expr */
    r = sizeof(int);
    r = sizeof(int*);
    r = sizeof r;

    /* <pos_fixo_r>: array, chamada, '.', '->', pós-incremento/decremento */
    tabela[0] = tabela[1] + tabela[2];
    r = soma(1, 2);
    tmp++;
    tmp--;

    /* <primario>: id, int, float, char, string, (expr) */
    r = variavel_global;
    r = 42;
    {
        float fx = 3.14;
        char c = 'z';
        texto s = "cobertura";
        r = (a + b);
        fx = fx + 1.0;
        c = c;
        s = s;
    }

    return r;
}

/* Membros de struct/union: '.', '->' */
int usar_struct(struct Ponto *p, struct Ponto q) {
    union Valor v;
    v.i = 7;
    p->x = q.y;
    p->matriz[1][2] = q.matriz[0][0];
    return v.i + p->ref[0];
}

/* Estruturas de controlo: <instrucao_while>, <instrucao_do>, <instrucao_for>
   (com as 3 secções presentes e ausentes), <instrucao_switch> com <lista_casos>,
   break, continue, goto + rótulo (D2), bloco aninhado, ';' vazio */
int controlo(int n) {
    int i;
    int acc = 0;

    while (n > 0) {
        n = n - 1;
    }

    do {
        acc = acc + 1;
    } while (acc < 3);

    for (i = 0; i < LIMITE; i = i + 1) {
        if (i == 2) { continue; }
        if (i > 5)  { break; }
        acc = acc + i;
    }

    /* for com secções vazias: <expressao_opcional> → ε */
    for (;;) {
        break;
    }

    switch (acc) {
        case 0:
            acc = 10;
            break;
        case 1:
            acc = 20;
            break;
        default:
            acc = 30;
    }

    if (acc > 0)
        acc = acc - 1;
    else
        acc = 0;

    goto fim;

fim:
    ;               /* instrucao ::= SYM_SEMICOLON (vazia) */
    { acc = acc; }  /* instrucao ::= <bloco> */

    return acc;
}

/* Função void com return sem valor: <instrucao_return> com expressao_opcional → ε */
void nada(void) {
    return;
}

int main(void) {
    struct Ponto p;
    struct Ponto *pp = &p;
    inteiro k = 5;

    p.x = 1;
    p.y = 2;
    nada();
    variavel_global = soma(k, controlo(3)) + usar_struct(pp, p);
    return 0;
}

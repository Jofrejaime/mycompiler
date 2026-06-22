/* ============================================================
   scope_stress.c — Auditoria completa de escopos
   Cobre: B1-B13
   ============================================================ */

/* B1: escopo global apenas */
int g_x = 0;
int g_y = 0;

/* B10/B11: prototipos multiplos — NAO devem criar escopos persistentes */
int soma(int a, int b);
int sub(int a, int b);
int mul(int);

/* B2: funcao simples — 1 escopo de funcao */
int funcao_simples() {
    return 0;
}

/* B3: bloco interno */
int com_bloco_interno() {
    int x = 1;
    {
        int y = 2;
        x = x + y;
    }
    return x;
}

/* B4: if sem bloco — NAO deve criar escopo */
int if_sem_bloco() {
    int x = 1;
    int y = 0;
    if (x > 0)
        y = 1;
    return y;
}

/* B5: if com bloco */
int if_com_bloco() {
    int x = 1;
    if (x > 0) {
        int y = 2;
        x = y;
    }
    return x;
}

/* B6: while sem bloco — NAO deve criar escopo */
int while_sem_bloco() {
    int x = 5;
    while (x > 0)
        x--;
    return x;
}

/* B7: while com bloco */
int while_com_bloco() {
    int x = 5;
    while (x > 0) {
        int tmp = x;
        x = tmp - 1;
    }
    return x;
}

/* B8: for sem bloco */
int for_sem_bloco() {
    int i = 0;
    int s = 0;
    for (i = 0; i < 10; i++)
        s = s + i;
    return s;
}

/* B9: for com bloco */
int for_com_bloco() {
    int s = 0;
    int i;
    for (i = 0; i < 10; i++) {
        int tmp = i;
        s = s + tmp;
    }
    return s;
}

/* B13: switch com bloco interno */
int switch_teste(int x) {
    int r = 0;
    switch (x) {
        case 1: {
            int a = 10;
            r = a;
            break;
        }
        case 2:
            r = 20;
            break;
        default:
            r = -1;
            break;
    }
    return r;
}

/* B12: struct — NAO cria escopo */
struct Ponto {
    int x;
    int y;
};

/* Implementacoes dos prototipos */
int soma(int a, int b) { return a + b; }
int sub(int a, int b)  { return a - b; }
int mul(int a)         { return a * a; }

int main() {
    int r = soma(1, 2);
    r = sub(r, 1);
    r = mul(r);
    return r;
}

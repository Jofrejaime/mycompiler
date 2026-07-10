/* ============================================================================
   TESTE SEMÂNTICO 07 — CONDIÇÕES NAS ESTRUTURAS DE CONTROLO
   
   Alínea f): verificação dos argumentos/condições nas estruturas de controlo.
   
   Erros esperados:
     - Linha 35: condição do 'switch' deve ser inteira, mas é float
   
   Sem erros:
     - Condições com int, char, float em if/while/for são aceites (tipo escalar)
     - Só void seria inválido — mas void não pode aparecer como expressão standalone
   ============================================================================ */

int obter_valor() {
    return 10;
}

float obter_float() {
    return 3.14;
}

int main() {
    int i = 5;
    char c = 'A';
    float f = 1.5;
    int *p = 0;

    /* IF com condição int — válido */
    if (i > 0) {
        i = i - 1;
    }

    /* IF com condição char — válido (char é escalar) */
    if (c) {
        c = 'B';
    }

    /* IF com resultado de função int — válido */
    if (obter_valor()) {
        i = 0;
    }

    /* WHILE com condição float — válido em C (escalar) */
    while (f > 0.0) {
        f = f - 0.1;
    }

    /* FOR com condição int — válido */
    int j = 0;
    for (j = 0; j < 10; j++) {
        i = i + j;
    }

    /* DO-WHILE com condição int — válido */
    do {
        i = i + 1;
    } while (i < 20);

    /* SWITCH com expressão int — válido */
    switch (i) {
        case 1:
            c = 'X';
            break;
        case 2:
            c = 'Y';
            break;
        default:
            c = 'Z';
            break;
    }

    /* ERRO (alínea f): switch com expressão float — inválido em C */
    switch (f) {
        case 1:
            i = 1;
            break;
        default:
            i = 0;
            break;
    }

    return i;
}

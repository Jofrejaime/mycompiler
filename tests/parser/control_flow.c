/* TEST: control_flow.c
   Cobre: if/else, while, do-while, for (todos os slots), switch, break, continue, goto, label */

int main() {
    int i = 0;
    int x = 10;

    /* IF simples */
    if (x > 0) x = x - 1;

    /* IF-ELSE */
    if (x == 0) {
        x = 1;
    } else {
        x = x * 2;
    }

    /* IF-ELSE IF-ELSE encadeado */
    if (x < 0) {
        x = 0;
    } else if (x == 0) {
        x = 1;
    } else {
        x = x + 1;
    }

    /* WHILE */
    while (x > 0) {
        x--;
    }

    /* DO-WHILE */
    do {
        x++;
    } while (x < 5);

    /* FOR completo */
    for (i = 0; i < 10; i++) {
        x = x + i;
    }

    /* FOR sem init (slot 0 = NULL) */
    for (; i < 20; i++) {
        x++;
    }

    /* FOR sem cond (slot 1 = NULL) */
    for (i = 0;; i++) {
        if (i >= 5) break;
    }

    /* FOR sem inc (slot 2 = NULL) */
    for (i = 0; i < 3;) {
        i++;
    }

    /* FOR vazio (slots 0,1,2 = NULL) */
    for (;;) {
        break;
    }

    /* SWITCH */
    switch (x) {
        case 0:
            x = 100;
            break;
        case 1:
            x = 200;
            /* fall-through */
        case 2:
            x = 300;
            break;
        default:
            x = -1;
            break;
    }

    /* NESTED loops */
    int j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (i == j) continue;
            x = x + i * j;
        }
    }

    /* GOTO e LABEL */
    goto fim;
    x = 9999;
fim:
    return x;
}

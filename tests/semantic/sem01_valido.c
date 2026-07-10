/* ============================================================================
   TESTE SEMÂNTICO 01 — PROGRAMA VÁLIDO
   
   Cobre: declarações, atribuições, operações aritméticas, chamadas de função,
          estruturas de controlo, arrays e tipos compatíveis.
   
   Resultado esperado: ZERO erros semânticos.
   ============================================================================ */

int soma(int a, int b) {
    return a + b;
}

float media(float x, float y) {
    return (x + y) / 2;
}

int fatorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * fatorial(n - 1);
}

int main() {
    /* Declarações simples com inicializadores compatíveis */
    int x = 10;
    int y = 20;
    float f = 3.14;
    char c = 'A';

    /* Atribuições compatíveis */
    x = y + 5;
    f = x;          /* int → float: promoção válida */

    /* Chamada de função com argumentos correctos */
    int s = soma(x, y);
    float m = media(f, 2.5);

    /* Estruturas de controlo com condições válidas */
    if (x > 0) {
        y = x * 2;
    }

    while (y > 0) {
        y = y - 1;
    }

    int i = 0;
    for (i = 0; i < 10; i++) {
        x = x + i;
    }

    int j = 0;
    do {
        j = j + 1;
    } while (j < 5);

    /* Array */
    int arr[5];
    arr[0] = 1;
    arr[4] = x + s;

    /* Factorial */
    int fat = fatorial(5);

    return 0;
}

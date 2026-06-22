/* TEST: expressions.c
   Cobre: todos os operadores, precedência, ternário, atribuições compostas, sizeof */

int main() {
    int a = 10;
    int b = 3;
    int c;

    /* Aritméticos */
    c = a + b;
    c = a - b;
    c = a * b;
    c = a / b;
    c = a % b;

    /* Relacionais */
    c = a < b;
    c = a > b;
    c = a <= b;
    c = a >= b;

    /* Igualdade */
    c = a == b;
    c = a != b;

    /* Lógicos */
    c = a && b;
    c = a || b;
    c = !a;

    /* Bitwise */
    c = a & b;
    c = a | b;
    c = a ^ b;
    c = ~a;
    c = a << 2;
    c = a >> 1;

    /* Atribuições compostas (suportadas pelo lexer) */
    a += 5;
    a -= 2;
    a = a * 3;
    a = a / 4;
    a = a % 2;
    a &= 255;
    a |= 1;
    a ^= 16;

    /* Incremento/decremento */
    a++;
    a--;
    ++a;
    --a;

    /* Ternário */
    c = (a > b) ? a : b;
    c = (a == 0) ? 1 : (b == 0) ? 2 : 3;

    /* sizeof expressão */
    c = sizeof(a);
    c = sizeof(a + b);

    /* sizeof tipo (TAREFA 3) */
    c = sizeof(int);
    c = sizeof(char);
    c = sizeof(double);
    c = sizeof(float);

    /* Parênteses e precedência */
    c = (a + b) * (a - b);
    c = a + b * 2;
    c = !(a && b) || (c != 0);

    /* Endereço e desreferência */
    int *p = &a;
    c = *p;
    *p = 99;

    /* Chamada de função */
    c = sizeof(c);

    /* Acesso a array */
    int arr[5];
    arr[0] = 1;
    arr[a - 9] = arr[0] + 1;

    return c;
}

/* sc07_nested_scopes.c — Suite: Escopo
   Categoria: Escopos aninhados (if/while/for/blocos)
   Verifica:
     - cada bloco cria um escopo separado
     - variáveis do bloco interior não aparecem no exterior
     - shadowing: variável interna com mesmo nome começa em offset=0 no seu escopo */

int g;

int main() {
    int x;       /* escopo da função */
    int y;

    {
        int x;   /* shadowing — novo escopo, offset=0 */
        int z;
    }

    if (g) {
        int a;   /* escopo do if */
        int b;
    }

    while (g) {
        int w;   /* escopo do while */
    }

    {
        int inner1;
        {
            int inner2;  /* escopo duplamente aninhado */
        }
    }

    return 0;
}

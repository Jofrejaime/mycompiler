/* ============================================================================
   TESTE SEMÂNTICO 08 — REDECLARAÇÃO DE PARÂMETRO E DE VARIÁVEL LOCAL

   Alínea b): variável declarada duas vezes no mesmo escopo — reforça sem03 com
   os casos de parâmetro duplicado e multi-declarador duplicado, garantindo que
   o SHADOWING legítimo (bloco aninhado, funções distintas) NÃO é reportado.

   Erros esperados (alínea b):
     - parâmetro 'p' redeclarado
     - variável 'x' redeclarada no mesmo escopo  (int x, x;)
     - variável 'y' redeclarada no mesmo escopo  (dois int y no mesmo bloco)
   ============================================================================ */

/* ERRO: dois parâmetros com o mesmo nome */
int f(int p, int p) {
    return p;
}

int shadowing_ok(int a) {
    int a2 = a;         /* nome diferente — OK */
    {
        int a2b = a2;   /* bloco aninhado, nome diferente — OK */
        return a2b;
    }
}

int outra(int a) {      /* mesmo nome 'a' de shadowing_ok, função distinta — OK */
    return a;
}

int main() {
    /* ERRO: multi-declarador com nome repetido */
    int x, x;

    /* ERRO: duas declarações separadas no mesmo bloco */
    int y;
    int y;

    /* OK: shadowing num bloco aninhado */
    int z = 1;
    {
        int z = 2;      /* escopo interior — não é redeclaração */
        z = z + 1;
    }

    return x + y + z;
}

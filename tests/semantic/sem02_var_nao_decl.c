/* ============================================================================
   TESTE SEMÂNTICO 02 — VARIÁVEL NÃO DECLARADA
   
   Alínea a): uso de variável antes de ser declarada / nunca declarada.
   
   Erros esperados:
     - Linha 16: 'z' não declarada
     - Linha 20: 'valor_desconhecido' não declarada
   ============================================================================ */

int main() {
    int x = 10;
    int y = 5;

    /* ERRO (alínea a): 'z' nunca foi declarada */
    int resultado = x + z;

    /* Uso correcto — sem erro */
    int soma = x + y;

    /* ERRO (alínea a): 'valor_desconhecido' nunca foi declarada */
    if (valor_desconhecido > 0) {
        soma = soma + 1;
    }

    return 0;
}

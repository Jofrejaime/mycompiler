/* ============================================================================
   TESTE SEMÂNTICO 06 — ATRIBUIÇÃO DE TIPO INCOMPATÍVEL
   
   Alínea e): verificação de atribuição de tipo incompatível a uma variável.
   
   Erros esperados:
     - Linha 23: atribuição incompatível — não é possível atribuir 'void' a 'int'
     - Linha 27: inicializador incompatível — string literal para variável int
   ============================================================================ */

void funcao_sem_retorno() {
    return;
}

int obter_numero() {
    return 42;
}

int main() {
    int x;
    float y;
    char c;

    /* Atribuições válidas — sem erro */
    x = 10;
    y = 3.14;
    c = 'A';
    x = c;          /* char → int: promoção válida */
    y = x;          /* int → float: promoção válida */

    /* ERRO (alínea e): inicializador do tipo string para variável int */
    int numero = "texto";

    /* ERRO (alínea e): atribuição de void a int */
    x = funcao_sem_retorno();

    /* Atribuições válidas com resultado de função */
    x = obter_numero();   /* int = int: válido */

    /* Atribuição entre ponteiros compatíveis — sem erro */
    int *p = 0;
    int *q = p;

    return 0;
}

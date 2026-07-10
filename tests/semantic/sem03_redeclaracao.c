/* ============================================================================
   TESTE SEMÂNTICO 03 — VARIÁVEL DECLARADA DUAS VEZES NO MESMO ESCOPO
   
   Alínea b): redeclaração de variável no mesmo escopo.
   
   Erros esperados (detectados pelo parser via report_redeclaration):
     - Linha 16: 'contador' redeclarada no mesmo escopo
     - Linha 22: 'nome' redeclarada no mesmo escopo
   ============================================================================ */

int main() {
    /* Primeira declaração — válida */
    int contador = 0;

    /* ERRO (alínea b): 'contador' já existe neste escopo */
    int contador = 10;

    /* Correcto: escopo diferente (bloco aninhado) */
    {
        int nome = 5;   /* novo 'nome' no escopo interno — válido */
    }

    /* ERRO (alínea b): 'nome' no escopo externo já existe (bloco de main) */
    char nome = 'A';
    char nome = 'B';

    return contador;
}

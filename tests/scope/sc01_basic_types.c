/* sc01_basic_types.c — Suite: Escopo
   Categoria: Tipos primitivos globais e locais
   Verifica: tipo, tamanho, endereço sequencial, categoria=variable */

/* Globais — addr sequencial: int=4, char=1, float=4, double=8 */
int    g_int;
char   g_char;
float  g_float;
double g_double;

int main() {
    /* Locais — offset sequencial dentro do escopo da função */
    int    l_int;
    char   l_char;
    float  l_float;
    double l_double;
    return 0;
}

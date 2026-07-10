/* sc06_function_params.c — Suite: Escopo
   Categoria: Parâmetros de funções
   Verifica:
     - parâmetros aparecem no escopo da função como categoria=parameter
     - endereços sequenciais dentro do escopo
     - tipos e tamanhos corretos */

int soma(int a, int b) {
    return a + b;
}

float media(float x, float y, float z) {
    float r;
    r = x;
    return r;
}

void copia(char *src, char *dst, int n) {
    int i;
    i = 0;
    return;
}

double potencia(double base, int exp) {
    double r;
    r = base;
    return r;
}

int main() {
    int x;
    x = soma(1, 2);
    return 0;
}

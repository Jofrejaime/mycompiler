/* sc03_arrays.c — Suite: Escopo
   Categoria: Arrays (1D, 2D, 3D) globais e locais
   Verifica: size = elem_size * d1 * d2 * d3
             Array=yes(N); addr sequencial */

int    arr1[10];          /* size = 40         */
char   arr2[20];          /* size = 20         */
float  arr3[5];           /* size = 20         */
double arr4[4];           /* size = 32         */
int    mat2[3][4];        /* size = 48         */
int    mat3[2][3][4];     /* size = 96         */

int main() {
    int   la[8];          /* size = 32         */
    char  lb[16];         /* size = 16         */
    int   lmat[2][5];     /* size = 40         */
    return 0;
}

/* TEST: declarations.c
   Cobre: variáveis globais, arrays, ponteiros, typedef, múltiplos declaradores */

/* Tipos básicos */
int    g_int;
char   g_char;
float  g_float;
double g_double;

/* Ponteiros */
int  *p_int;
char *p_char;
int  **pp_int;

/* Arrays */
int  arr1[10];
char str1[256];
int  matrix[4][4];

/* Múltiplos declaradores na mesma linha */
int a, b, c;
int *x, y, *z;

/* Inicializadores */
int   val = 42;
float pi  = 3.14;
char  ch  = 'A';

/* Typedef simples */
typedef int   inteiro;
typedef char *string;
typedef float real;

/* Typedef de array */
typedef int vetor[10];

/* Variáveis com typedef */
inteiro n = 10;
real    r = 2.71;

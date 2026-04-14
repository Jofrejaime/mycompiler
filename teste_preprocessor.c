#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 100
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#ifdef DEBUG
  #pragma warning(disable: 4996)
#endif
#ifndef RELEASE
  int debug_mode = 1;
#endif

int main() {
    printf("Teste de preprocessor\n");
    return 0;
}

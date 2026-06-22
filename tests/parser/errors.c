/* TEST: errors.c
   Código inválido proposital — testa recuperação de erros do parser.
   Esperado: erros reportados, parser continua, AST parcial gerada. */

/* ERRO 1: ponto-e-vírgula em falta */
int x = 10
int y = 20;

/* ERRO 2: inicializador em campo de struct */
struct Ruim {
    int valor = 5;
    char nome[10];
};

/* ERRO 3: tipo desconhecido */
unknowntype z;

/* ERRO 4: chave a mais no escopo global */
}

/* Recuperação: declaração válida depois dos erros */
int recuperado = 99;

/* ERRO 5: parêntese sem fechar em expressão */
int funcao_ruim() {
    int a = (10 + 20;
    return a;
}

/* ERRO 6: return sem semicolon */
int outra_funcao() {
    int b = 5;
    return b
}

/* Função válida depois dos erros */
int funcao_boa() {
    return 42;
}

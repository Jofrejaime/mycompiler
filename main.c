#include <stdio.h>
#include <stdlib.h>
#include "src/lexer/lexer.h"
#include "src/lexer/tokens.h"
#include "utils/printer.h"

int main(int argc, char *argv[]) {
    print_header();
    
    /* Validar argumentos */
    if (argc < 2) {
        fprintf(stderr, "ERRO: Numero de argumentos invalido\n\n");
        print_usage(argv[0]);
        return 1;
    }
    
    const char *input_file = argv[1];
    const char *output_file = (argc >= 3) ? argv[2] : "tabela_simbolos.txt";
    
    printf("Arquivo de entrada: %s\n", input_file);
    printf("Arquivo de saida:   %s\n\n", output_file);
    
    /* Criar lexer */
    printf("Criando lexer...\n");
    lexer_t *lexer = criar_lexer(input_file);
    
    if (lexer == NULL) {
        fprintf(stderr, "ERRO: Nao foi possivel abrir arquivo '%s'\n", input_file);
        return 2;
    }
    
    printf("OK: lexer criado\n\n");
    
    /* Executar analise lexica */
    printf("Executando analise lexica...\n");
    printf("-----------------------------------\n\n");
    
    token_t token;
    int error_count = 0;
    
    do {
        token = analex(lexer);
        
        if (token.tipo == TK_ERROR) {
            printf("ERRO na linha %d, coluna %d: '%s'\n", 
                   token.linha, token.coluna, token.lexeme);
            error_count++;
        }
    } while (token.tipo != TK_EOF);
    
    printf("\n-----------------------------------\n");
    printf("OK: Analise lexica concluida\n\n");
    
    /* Exibir tabela de simbolos */
    printf("Tabela de simbolos:\n\n");
    imprimir_tabla_simbolos(lexer->tabla_simbolos);
    
    /* Salvar resultado */
    printf("\nSalvando em '%s'...\n", output_file);
    save_symbol_table(output_file, lexer, error_count);
    printf("OK: Resultado salvo\n");
    
    /* Exibir resumo */
    print_summary(lexer, error_count);
    
    /* Limpeza */
    printf("Liberando recursos...\n");
    liberar_lexer(lexer);
    printf("OK: Programa finalizado com sucesso!\n\n");
    
    return (error_count == 0) ? 0 : 1;
}

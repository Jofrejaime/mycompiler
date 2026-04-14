#include <stdio.h>
#include <stdlib.h>
#include "src/lexer/lexer.h"
#include "src/lexer/tokens.h"

/* ============================================================================
   ANALISADOR LÉXICO - PROGRAMA PRINCIPAL
   
   Trabalho Prático: Análise Léxica (Fase 1 do Compilador)
   Professor: André Filemon
   Disciplina: Compiladores - M3
   
   FUNÇÃO PRINCIPAL:
   1. Lê arquivo-fonte em C
   2. Executa análise léxica (função analex())
   3. Exibe tabela de símbolos com tokens e lexemas
   4. Salva resultado em arquivo de saída
   
   Compilação:
   $ gcc -o lexer_compiler main.c src/lexer/lexer.c src/lexer/tokens.c src/lexer/keywords.c -I.
   
   Uso:
   $ ./lexer_compiler entrada.c saida.txt
   $ ./lexer_compiler entrada.c          (saída em stdout + arquivo_out.txt)
   
   ============================================================================ */

void exibir_uso(const char *programa) {
    printf("USO: %s <arquivo_entrada> [arquivo_saida]\n\n", programa);
    printf("Argumentos:\n");
    printf("  arquivo_entrada  Caminho do arquivo-fonte em C a analisar\n");
    printf("  arquivo_saida    (Opcional) Caminho para salvar tabela de símbolos\n");
    printf("                   Padrão: 'tabela_simbolos.txt'\n\n");
    printf("Exemplo:\n");
    printf("  %s programa.c\n", programa);
    printf("  %s programa.c resultado.txt\n", programa);
}

void exibir_cabecalho() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                           ║\n");
    printf("║              ANALISADOR LÉXICO - FASE 1 DO COMPILADOR C→C                ║\n");
    printf("║                                                                           ║\n");
    printf("║  Instituto Superior Politécnico de Tecnologias e Ciências (ISPTEC)        ║\n");
    printf("║  Disciplina: Compiladores - M3                                            ║\n");
    printf("║  Professor: André Filemon                                                 ║\n");
    printf("║                                                                           ║\n");
    printf("║  Baseado em: Compiladores - Princípios, Técnicas e Ferramentas           ║\n");
    printf("║              (Aho, Lam, Sethi, Ullman)                                    ║\n");
    printf("║                                                                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

int main(int argc, char *argv[]) {
    exibir_cabecalho();
    
    /* Validar argumentos */
    if (argc < 2) {
        fprintf(stderr, "ERRO: Número de argumentos inválido\n\n");
        exibir_uso(argv[0]);
        return 1;
    }
    
    const char *arquivo_entrada = argv[1];
    const char *arquivo_saida = (argc >= 3) ? argv[2] : "tabela_simbolos.txt";
    
    printf("📂 Arquivo de entrada: %s\n", arquivo_entrada);
    printf("💾 Arquivo de saída:   %s\n\n", arquivo_saida);
    
    /* ========== ETAPA 1: CRIAR LEXER ========== */
    printf("⏳ Criando lexer...\n");
    Lexer *lexer = criar_lexer(arquivo_entrada);
    
    if (lexer == NULL) {
        fprintf(stderr, "❌ ERRO: Não foi possível abrir arquivo '%s'\n", arquivo_entrada);
        return 2;
    }
    
    printf("✓ Lexer criado com sucesso\n\n");
    
    /* ========== ETAPA 2: EXECUTAR ANÁLISE LÉXICA ========== */
    printf("⏳ Executando análise léxica (função analex)...\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    
    Token token;
    int contador_tokens = 0;
    int contador_erros = 0;
    
    /* Processar todos os tokens */
    do {
        token = analex(lexer);
        
        if (token.tipo == TK_ERROR) {
            printf("⚠️  ERRO LÉXICO na linha %d, coluna %d: '%s'\n", 
                   token.linha, token.coluna, token.lexeme);
            contador_erros++;
        } else if (token.tipo != TK_EOF) {
            contador_tokens++;
        }
    } while (token.tipo != TK_EOF);
    
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("✓ Análise léxica concluída\n\n");
    
    /* ========== ETAPA 3: EXIBIR TABELA DE SÍMBOLOS ========== */
    printf("⏳ Exibindo tabela de símbolos...\n\n");
    
    imprimir_tabla_simbolos(lexer->tabla_simbolos);
    
    /* ========== ETAPA 4: SALVAR RESULTADO EM ARQUIVO ========== */
    printf("\n⏳ Salvando resultado em '%s'...\n", arquivo_saida);
    
    FILE *fp_saida = fopen(arquivo_saida, "w");
    if (fp_saida == NULL) {
        fprintf(stderr, "❌ ERRO: Não foi possível criar arquivo '%s'\n", arquivo_saida);
    } else {
        fprintf(fp_saida, "═══════════════════════════════════════════════════════════════\n");
        fprintf(fp_saida, "    TABELA DE SÍMBOLOS - ANÁLISE LÉXICA\n");
        fprintf(fp_saida, "═══════════════════════════════════════════════════════════════\n\n");
        fprintf(fp_saida, "Arquivo analisado: %s\n", arquivo_entrada);
        fprintf(fp_saida, "Total de tokens: %d\n", lexer->tabla_simbolos->quantidade - contador_erros);
        fprintf(fp_saida, "Erros léxicos: %d\n\n", contador_erros);
        
        fprintf(fp_saida, "TOKENS ENCONTRADOS:\n");
        fprintf(fp_saida, "───────────────────────────────────────────────────────────────\n");
        fprintf(fp_saida, "Nº   │ Tipo                │ Lexema         │ Linha │ Coluna\n");
        fprintf(fp_saida, "─────┼─────────────────────┼────────────────┼───────┼─────────\n");
        
        int num = 0;
        for (int i = 0; i < lexer->tabla_simbolos->quantidade; i++) {
            Token *t = &lexer->tabla_simbolos->tokens[i];
            
            if (t->tipo == TK_EOF) continue;
            
            if (t->tipo == TK_ERROR) {
                fprintf(fp_saida, "%3d  │ [ERRO]              │ %-14s │ %5d │ %5d\n",
                        ++num, t->lexeme, t->linha, t->coluna);
            } else {
                fprintf(fp_saida, "%3d  │ %-19s │ %-14s │ %5d │ %5d\n",
                        ++num, tipo_token_para_string(t->tipo), t->lexeme, 
                        t->linha, t->coluna);
            }
        }
        
        fprintf(fp_saida, "\n═══════════════════════════════════════════════════════════════\n");
        fprintf(fp_saida, "FIM DO RELATÓRIO\n");
        fprintf(fp_saida, "═══════════════════════════════════════════════════════════════\n");
        
        fclose(fp_saida);
        printf("✓ Resultado salvo com sucesso\n");
    }
    
    /* ========== ETAPA 5: ESTATÍSTICAS FINAIS ========== */
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                         RESUMO DA ANÁLISE                                 ║\n");
    printf("╠═══════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ Total de tokens analisados: %-47d  ║\n", lexer->tabla_simbolos->quantidade - 1); /* -1 para EOF */
    printf("║ Erros léxicos encontrados:  %-47d  ║\n", contador_erros);
    printf("║ Linhas do arquivo: %-56d  ║\n", lexer->linha);
    printf("╠═══════════════════════════════════════════════════════════════════════════╣\n");
    
    if (contador_erros == 0) {
        printf("║ ✓ Análise concluída com SUCESSO!                                        ║\n");
    } else {
        printf("║ ⚠️  Análise concluída com ERROS (verifique acima)                         ║\n");
    }
    
    printf("╚═══════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* ========== LIMPEZA ========== */
    printf("⏳ Liberando recursos...\n");
    liberar_lexer(lexer);
    printf("✓ Recursos liberados\n\n");
    
    printf("✓ Programa finalizado com sucesso!\n");
    printf("  Tabela de símbolos salva em: %s\n\n", arquivo_saida);
    
    return 0;
}

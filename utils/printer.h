#ifndef UTILS_PRINTER_H
#define UTILS_PRINTER_H

#include "../src/lexer/lexer.h"

void print_header(void);
void print_usage(const char *programa);
void print_summary(lexer_t *lexer, int error_count);
void save_symbol_table(const char *output_file, lexer_t *lexer, int error_count);

/* Core application logic */
int validate_args(int argc, char *argv[], const char **input, const char **output);
lexer_t* run_lexical_analysis(const char *input, const char *output, int *error_count);
void cleanup(lexer_t *lexer);

#endif /* UTILS_PRINTER_H */

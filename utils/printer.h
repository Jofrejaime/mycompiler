#ifndef UTILS_PRINTER_H
#define UTILS_PRINTER_H

#include "../src/lexer/lexer.h"

void print_header(void);
void print_usage(const char *programa);
void print_summary(lexer_t *lexer, int error_count);
void save_symbol_table(const char *output_file, lexer_t *lexer, int error_count);

#endif /* UTILS_PRINTER_H */

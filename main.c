#include <stdio.h>
#include "src/lexer/lexer.h"
#include "utils/printer.h"

int main(int argc, char *argv[]) {
    print_header();
    
    const char *input, *output;
    if (validate_args(argc, argv, &input, &output) != 0)
        return 1;
    
    int error_count;
    lexer_t *lexer = run_lexical_analysis(input, output, &error_count);
    if (lexer == NULL)
        return 2;
    
    print_summary(lexer, error_count);
    cleanup(lexer);
    
    return (error_count == 0) ? 0 : 1;
}

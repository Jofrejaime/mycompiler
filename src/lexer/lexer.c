#include "lexer.h"
#include <string.h>

extern token_t fsm_next_token(lexer_t *lexer);

/* ============================================================================
   ANALISADOR LÉXICO - FUNÇÃO PRINCIPAL analex()
   
   Delegação elegante para a Máquina de Estados Finita (FSM)
   
   A máquina de estados é implementada em lexer_fsm.c com lógica completa
   dos grupos léxicos (identificadores, números, operadores, etc.)
   
   ============================================================================ */

token_t analex(lexer_t *lexer) {
    if (lexer == NULL) {
        return (token_t){TK_ERROR, "", 0, 0, {0}};
    }
    
    return fsm_next_token(lexer);
}

#ifndef INCLUDE_LEXER_H_
#define INCLUDE_LEXER_H_

#include <ctype.h>
#include "string_t.h"
#include "arena.h"
#include "token.h"

typedef struct {
    int pos;
    char curr_char;
    arena_t* arena;
    const char* expr;
    int len;
} lexer_t;

lexer_t create_lexer(const char* expr, arena_t* arena);
token_t get_next_token(lexer_t* lexer);

#endif // !INCLUDE_LEXER_H_

#include "lexer.h"

lexer_t create_lexer(const char* expr, arena_t* arena) {
    lexer_t lexer = {
        .pos = 1,
        .curr_char = expr[0],
        .arena = arena,
        .expr = expr,
        .len = strlen(expr)
    };

    return lexer;
}

void advance(lexer_t* lexer) {
    if (lexer->pos >= lexer->len) {
        lexer->curr_char = '\0';
        return;
    }

    lexer->curr_char = lexer->expr[lexer->pos++];
}

token_t get_next_token(lexer_t* lexer) {
    while (lexer->curr_char != '\0') {
        if (isspace(lexer->curr_char)) {
            advance(lexer); 
        } else if (isdigit(lexer->curr_char)) {
            string_t val = create_string(lexer->arena);
            
            while (isdigit(lexer->curr_char)) {
                string_append_chr(&val, lexer->curr_char);
                advance(lexer);
            }

            if (lexer->curr_char == '.') {
                string_append_chr(&val, '.');
                advance(lexer);
                while (isdigit(lexer->curr_char)) {
                    string_append_chr(&val, lexer->curr_char);
                    advance(lexer);
                }
            }

            double num = string_to_double(&val);
            return create_num_token(num);
        } else if (isalpha(lexer->curr_char)) {
            string_t val = create_string(lexer->arena);

            while (isalpha(lexer->curr_char)) {
                string_append_chr(&val, lexer->curr_char);
                advance(lexer);
            }

            return create_func_token(val);
        } else if (lexer->curr_char == '+') {
            advance(lexer);
            return create_op_token('+');
        } else if (lexer->curr_char == '-') {
            advance(lexer);
            return create_op_token('-');
        } else if (lexer->curr_char == '*') {
            advance(lexer);
            return create_op_token('*');
        } else if (lexer->curr_char == '/') {
            advance(lexer);
            return create_op_token('/');
        } else if (lexer->curr_char == '^') {
            advance(lexer);
            return create_op_token('^');
        } else if (lexer->curr_char == '(') {
            advance(lexer);
            return create_token(LPAREN, '(');
        } else if (lexer->curr_char == ')') {
            advance(lexer);
            return create_token(RPAREN, ')');
        } else {
            token_t token = {
                .type = UNKNOWN,
                .val.name = string_literal("UNKNOWN", lexer->arena)
            };

            return token; 
        }
    }

    token_t token = {
        .type = ENDOFFILE,
        .val.name = string_literal("eof", lexer->arena)
    };

    return token;
}

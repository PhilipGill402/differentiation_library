#ifndef INCLUDE_TOKEN_H_
#define INCLUDE_TOKEN_H_

#include <stdio.h>
#include "string_t.h"
#include "arena.h"

typedef enum {
    NUM,
    OP,
    FUNC,
    VAR,
    ENDOFFILE,
    UNKNOWN
} token_type_t;

typedef struct {
    token_type_t type;
    union {
        char op;
        double num;
        string_t name;
    } val;
} token_t;

token_t create_num_token(double num);
token_t create_op_token(char op);
token_t create_func_token(string_t name);
token_t create_var_token(string_t name);
void print_token(const token_t* token);

#endif /* INCLUDE_TOKEN_H_ */

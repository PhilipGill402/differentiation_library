#include "token.h"

token_t create_num_token(double num) {
    token_t token = {
        .type = NUM,
        .val.num = num
    };

    return token;
}

token_t create_op_token(char op) {
    token_t token = {
        .type = OP,
        .val.op = op
    };

    return token;
}

token_t create_func_token(string_t name) {
    token_t token = {
        .type = FUNC,
        .val.name = name
    };

    return token;
}

token_t create_var_token(string_t name) {
    token_t token = {
        .type = VAR,
        .val.name = name 
    };

    return token;
}

void print_token(const token_t* token) {
    printf("TOKEN(");
    
    switch (token->type) {
        case NUM:
            printf("NUM, %f", token->val.num);
            break;
        case OP:
            printf("OP, %c", token->val.op);
            break;
        case FUNC:
            printf("FUNC, ");
            printstr(&token->val.name, 0);
            break;
        case VAR:
            printf("VAR, ");
            printstr(&token->val.name, 0);
            break;
        case ENDOFFILE:
            printf("ENDOFFILE, eof");
            break;
        default:
            printf("UNKNOWN, UNKNOWN");
            break;
    }
    
    printf(")\n");
}

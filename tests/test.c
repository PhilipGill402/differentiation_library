#include "backprop.h"
#include "parser.h"
#include <stdio.h>

int main() {
    arena_t arena = create_arena(PAGE_SIZE);
    
    char* expr = "20/5/2";
    node_t* root = parse(expr, &arena); 

    backprop(root);

    print_node(root); 

    /*
    token_t token = get_next_token(&lexer);
    print_token(&token);

    while (token.type != ENDOFFILE) {
        token = get_next_token(&lexer);
        print_token(&token);
    }

    node_t* x = create_node(3.0, &arena);
    node_t* mul = node_mul(x, x, &arena);
    node_t* add = node_add(mul, x, &arena);

    double ret = backprop(add);

    printf("%f\n", x->grad);
    */

    release_arena(&arena);
}

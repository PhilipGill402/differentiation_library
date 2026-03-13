#include "backprop.h"
#include "parser.h"
#include <stdio.h>

int main() {
    arena_t arena = create_arena(PAGE_SIZE);
    
    char* expr = "x+x^2+y^2";
    parser_t parser = create_parser(expr, &arena);
    set_var(&parser, "x", 4.0);
    set_var(&parser, "y", 2.0);
    
    node_t* root = parse(&parser);

    print_node(root); 

    release_arena(&arena);
}

#include "backprop.h"
#include "parser.h"
#include <stdio.h>

int main() {
    arena_t arena = create_arena(PAGE_SIZE);
    
    char* expr = "sqrt(x*y)";
    parser_t parser = create_parser(expr, &arena);
    set_var(&parser, "x", 4);
    set_var(&parser, "y", 9);
    
    node_t* root = parse(&parser);

    print_node(root); 

    release_arena(&arena);
}

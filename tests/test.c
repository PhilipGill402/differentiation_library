#include "backprop.h"
#include "parser.h"
#include <stdio.h>

int main() {
    char* expr = "sqrt(x*y)";
    parser_t parser = create_parser(expr);
    set_var(&parser, "x", 4);
    set_var(&parser, "y", 9);
    
    node_t* root = parse(&parser);

    print_node(root);

    release_parser(&parser);
}

#include "backprop.h"
#include "parser.h"
#include <stdio.h>

void print_derivatives(parser_t* parser) {
    for (int i = 0; i < parser->variables.size; i++) {
        entry_t entry = *(entry_t*)get(&parser->variables, i);
        printf("d(");
        printstr(&entry.name, 1);
        printf(") = %f\n", entry.node->grad);
    }
}

int main() {
    char* expr = "sqrt(x*y)";
    parser_t parser = create_parser();
    set_expr(expr, &parser);
    set_var(&parser, "x", 4);
    set_var(&parser, "y", 9);
    
    node_t* root = parse(&parser);

    
    print_derivatives(&parser);
    print_node(root);

    release_parser(&parser);
}

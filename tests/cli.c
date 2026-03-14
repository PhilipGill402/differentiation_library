#include "parser.h"
#include <stdio.h>
#define MAX_INPUT_SIZE 100

void print_derivatives(parser_t* parser) {
    for (int i = 0; i < parser->variables.size; i++) {
        entry_t entry = *(entry_t*)get(&parser->variables, i);
        printf("d(");
        printstr(&entry.name, 1);
        printf(") = %f\n", entry.node->grad);
    }
}

int main() {
    char input_buff[MAX_INPUT_SIZE]; 
    parser_t parser = create_parser();
    
    do {
        printf(">> ");
        fgets(input_buff, MAX_INPUT_SIZE, stdin);
        
        char* cmd = strtok(input_buff, " ");
        while (strcmp(cmd, "set") == 0) {
            char* var = strtok(NULL, " ");
            char* val_str = strtok(NULL, " ");
            double val = atof(val_str);
            
            set_var(&parser, var, val);
            printf(">> ");
            fgets(input_buff, MAX_INPUT_SIZE, stdin);
            cmd = strtok(input_buff, " ");
        }
        
        if (strcmp(input_buff, "exit\n") == 0) {
            break;
        }
        
        set_expr(input_buff, &parser);
        node_t* root = parse(&parser);
        print_derivatives(&parser);
        printf("%f\n", root->value); 
    } while (strcmp(input_buff, "exit") != 0);

    release_parser(&parser);
}

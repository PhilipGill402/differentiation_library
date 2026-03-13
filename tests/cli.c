#include "parser.h"
#include <stdio.h>
#define MAX_INPUT_SIZE 100

int main() {
    char input_buff[MAX_INPUT_SIZE]; 

    do {
        printf(">> ");
        scanf("%s", input_buff);

        if (strcmp(input_buff, "exit") == 0) {
            break;
        }
        
        parser_t parser = create_parser(input_buff);
        node_t* root = parse(&parser);
        printf("%f\n", root->value); 
        release_parser(&parser);
    } while (strcmp(input_buff, "exit") != 0);
}

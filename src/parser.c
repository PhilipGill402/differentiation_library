#include "parser.h"

/* HELPERS */
int operator_precedence(token_t token) {
    if (token.type == FUNC) {
        return 5;
    } else if (token.type == OP) {
        if (token.val.op == '^') {
            return 4;
        } else if (token.val.op == '*' || token.val.op == '/') {
            return 2;
        } else if (token.val.op == '+' || token.val.op == '-') {
            return 1;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}

int is_right_associative(token_t token) {
    if (token.type != OP) {
        return 0;
    }

    if (token.val.op == '^') {
        return 1;
    } else {
        return 0;
    }
}

parser_t create_parser(lexer_t* lexer, arena_t* arena) {
    parser_t parser;
    parser.tokens = create_queue(sizeof(token_t), arena);
    parser.op_stack = create_stack(sizeof(token_t), arena);
    parser.output = create_queue(sizeof(token_t), arena);
    parser.arena = arena;
    parser.lexer = lexer;

    return parser;
}

void get_infix(parser_t* parser) {
    token_t token = get_next_token(parser->lexer);
    enqueue(&parser->tokens, &token);

    while (token.type != ENDOFFILE) {
        token = get_next_token(parser->lexer);

        if (token.type == ENDOFFILE) {
            break;
        }

        enqueue(&parser->tokens, &token);
    }

}

void get_postfix(parser_t* parser) {
    while (!is_empty(&parser->tokens)) {
        token_t token = *(token_t*)dequeue(&parser->tokens);
        print_token(&token);
        printf("\n");
        
        if (token.type == NUM) {
            enqueue(&parser->output, &token);
        } else if (token.type == OP) {
            while (!stack_is_empty(&parser->op_stack) && ((*(token_t*)stack_top(&parser->op_stack)).type == OP) && (operator_precedence(*(token_t*)stack_top(&parser->op_stack)) > operator_precedence(token))) {
                token_t op = *(token_t*)stack_pop(&parser->op_stack);
                enqueue(&parser->output, &op);
            }

            stack_push(&parser->op_stack, &token);
        } else if (token.type == LPAREN) {
            stack_push(&parser->op_stack, &token);
        } else if (token.type == RPAREN) {
            while ((*(token_t*)stack_top(&parser->op_stack)).type != LPAREN) {
                token_t op = *(token_t*)stack_pop(&parser->op_stack);
                enqueue(&parser->output, &op);
            }

            stack_pop(&parser->op_stack);
        }
    }

    while (!stack_is_empty(&parser->op_stack)) {
        token_t op = *(token_t*)stack_pop(&parser->op_stack);
        enqueue(&parser->output, &op);
    }
}

node_t* create_graph(parser_t* parser) {
    stack_t node_stack = create_stack(sizeof(node_t*), parser->arena);

    while (!is_empty(&parser->output)) {
        token_t token = *(token_t*)dequeue(&parser->output);

        if (token.type == NUM) {
            node_t* node = create_node(token.val.num, parser->arena);
            stack_push(&node_stack, &node); 
        }
    }
}


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

parser_t init_parser(lexer_t* lexer, arena_t* arena) {
    parser_t parser;
    parser.tokens = create_queue(sizeof(token_t), arena);
    parser.op_stack = create_stack(sizeof(token_t), arena);
    parser.output = create_queue(sizeof(token_t), arena);
    parser.variables = create_vector(sizeof(entry_t), arena);
    parser.nodes = create_vector(sizeof(node_t*), arena);
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
        
        if (token.type == NUM) {
            enqueue(&parser->output, &token);
        } else if (token.type == VAR) {
            enqueue(&parser->output, &token);
        } else if (token.type == OP) {
            while (!stack_is_empty(&parser->op_stack)
                    && ((*(token_t*)stack_top(&parser->op_stack)).type == OP) 
                    && ((operator_precedence(*(token_t*)stack_top(&parser->op_stack)) > operator_precedence(token))
                        || ((operator_precedence(*(token_t*)stack_top(&parser->op_stack)) == operator_precedence(token)) && !is_right_associative(token)))) {
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
    stack_s node_stack = create_stack(sizeof(node_t*), parser->arena);
    
    while (!is_empty(&parser->output)) {
        token_t token = *(token_t*)dequeue(&parser->output);

        if (token.type == NUM) {
            node_t* node = create_node(token.val.num, parser->arena);
            stack_push(&node_stack, &node);
            push_back(&parser->nodes, &node);
        } else if (token.type == VAR) {
            node_t* node = create_var(&token.val.name, &parser->variables, parser->arena);
            stack_push(&node_stack, &node);
            push_back(&parser->nodes, &node);
        } else if (token.type == OP) {
            node_t* right = *(node_t**)stack_pop(&node_stack);
            node_t* left = *(node_t**)stack_pop(&node_stack);
            node_t* node;

            switch (token.val.op) {
                case '+':
                    node = node_add(left, right, parser->arena);
                    stack_push(&node_stack, &node);
                    push_back(&parser->nodes, &node);
                    break;
                case '*':
                    node = node_mul(left, right, parser->arena);
                    stack_push(&node_stack, &node);
                    push_back(&parser->nodes, &node);
                    break;
                case '-':
                    node = node_sub(left, right, parser->arena);
                    stack_push(&node_stack, &node);
                    push_back(&parser->nodes, &node);
                    break;
                case '/':
                    node = node_div(left, right, parser->arena);
                    stack_push(&node_stack, &node);
                    push_back(&parser->nodes, &node);
                    break;
                case '^':
                    node = node_pow(left, right, parser->arena);
                    stack_push(&node_stack, &node);
                    push_back(&parser->nodes, &node);
                    break;
                default:
                    fprintf(stderr, "Did not understand operator '%c' in 'create_graph'\n", token.val.op); 
                    break;
            } 
        }
    }
    
    return *(node_t**)stack_top(&node_stack);
}


parser_t create_parser(const char* expr, arena_t* arena) {
    lexer_t lexer = create_lexer(expr, arena);
    parser_t parser = init_parser(&lexer, arena);

    return parser;
}

node_t* parse(parser_t* parser) {
    get_infix(parser);
    get_postfix(parser);
    node_t* root = create_graph(parser);
    backprop(root, &parser->nodes);

    for (int i = 0; i < parser->variables.size; i++) {
        entry_t entry = *(entry_t*)get(&parser->variables, i);
        print_entry(&entry);
        printf("\n");
    }

    return root;
}


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

double node_sin(node_t* node) {
    return sin(node->value);
}

double node_cos(node_t* node) {
    return cos(node->value);
}

double node_tan(node_t* node) {
    return tan(node->value);
}

double node_sqrt(node_t* node) {
    return sqrt(node->value);
}

void backward_sin(node_t* self) {
    self->left->grad += cos(self->left->value) * self->grad;
}

void backward_cos(node_t* self) {
    self->left->grad += -sin(self->left->value) * self->grad;
}

void backward_tan(node_t* self) {
    double sec = 1.0 / cos(self->left->value);
    self->left->grad += sec * sec * self->grad;
}

void backward_sqrt(node_t* self) {
    double x = 0.5 / sqrt(self->left->value);
    self->left->grad += x * self->grad;
}

/* PARSER */
parser_t init_parser(arena_t* arena) {
    parser_t parser;
    parser.tokens = create_queue(sizeof(token_t), arena);
    parser.op_stack = create_stack(sizeof(token_t), arena);
    parser.output = create_queue(sizeof(token_t), arena);
    parser.variables = create_vector(sizeof(entry_t), arena);
    parser.nodes = create_vector(sizeof(node_t*), arena);
    parser.arena = arena;
    parser.lexer = NULL;

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
        } else if (token.type == FUNC) {
            stack_push(&parser->op_stack, &token);
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

            if (!stack_is_empty(&parser->op_stack) && (*(token_t*)stack_top(&parser->op_stack)).type == FUNC) {
                token_t fn = *(token_t*)stack_pop(&parser->op_stack);
                enqueue(&parser->output, &fn);
            }
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
            node_t* node = create_var_node(&token.val.name, &parser->variables, parser->arena);
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
        } else if (token.type == FUNC) {
            string_t sin_str = string_literal("SIN", parser->arena);
            string_t cos_str = string_literal("COS", parser->arena);
            string_t tan_str = string_literal("TAN", parser->arena);
            string_t sqrt_str = string_literal("SQRT", parser->arena);
            string_t upper_str = string_upper(&token.val.name);
            node_t* left = *(node_t**)stack_pop(&node_stack);
            node_t* node;

            if (string_compare(&upper_str, &sin_str) == 0) {
                node = node_func(left, node_sin, backward_sin, parser->arena);
                stack_push(&node_stack, &node);
                push_back(&parser->nodes, &node);
            } else if (string_compare(&upper_str, &cos_str) == 0) {
                node = node_func(left, node_cos, backward_cos, parser->arena);
                stack_push(&node_stack, &node);
                push_back(&parser->nodes, &node);
            } else if (string_compare(&upper_str, &tan_str) == 0) {
                node = node_func(left, node_tan, backward_tan, parser->arena);
                stack_push(&node_stack, &node);
                push_back(&parser->nodes, &node);
            } else if (string_compare(&upper_str, &sqrt_str) == 0) {
                node = node_func(left, node_sqrt, backward_sqrt, parser->arena);
                stack_push(&node_stack, &node);
                push_back(&parser->nodes, &node);
            } else {
                printstr(&token.val.name, 0);
                fprintf(stderr, "Function is not defined\n");
            }
        }
    }
    
    return *(node_t**)stack_top(&node_stack);
}


parser_t create_parser() {
    arena_t* arena = malloc(sizeof(arena_t));
    *arena = create_arena(PAGE_SIZE);

    parser_t parser = init_parser(arena);

    return parser;
}

void set_expr(const char* expr, parser_t* parser) {
    lexer_t* lexer = reserve(sizeof(lexer_t), parser->arena); 
    *lexer = create_lexer(expr, parser->arena);

    parser->lexer = lexer;
}

node_t* parse(parser_t* parser) {
    get_infix(parser);
    get_postfix(parser);

    node_t* root = create_graph(parser);
    backprop(root, &parser->nodes);

    return root;
}

void create_var(parser_t* parser, char* name) {
    string_t str_name = string_literal(name, parser->arena); 
    create_var_node(&str_name, &parser->variables, parser->arena); 
}

void set_var(parser_t* parser, char* name, double val) {
    string_t str_name = string_literal(name, parser->arena);
    
    for (int i = 0; i < parser->variables.size; i++) {
        entry_t entry = *(entry_t*)get(&parser->variables, i);
        if (string_compare(&str_name, &entry.name) == 0) {
            entry.node->value = val;
            return;
        }
    }
    
    node_t* node = reserve(sizeof(node_t), parser->arena);
    node->value = val;
    node->grad = 0;
    node->left = NULL;
    node->right = NULL;
    node->backward = NULL;
    node->is_var = 1;
    node->name = str_name;

    entry_t entry = {
        .name = str_name,
        .node = node 
    };

    push_back(&parser->variables, &entry);
}

void release_parser(parser_t* parser) {
    release_arena(parser->arena); 
    free(parser->arena);
    parser->arena = NULL;
}



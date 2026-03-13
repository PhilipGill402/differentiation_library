#ifndef INCLUDE_PARSER_H_
#define INCLUDE_PARSER_H_

#include <math.h>
#include "lexer.h"
#include "token.h"
#include "string_t.h"
#include "arena.h"
#include "queue.h"
#include "stack.h"
#include "vector.h"
#include "node.h"
#include "entry.h"
#include "backprop.h"

typedef struct {
    queue_t tokens;
    stack_s op_stack;
    queue_t output;
    vector_t variables;
    vector_t nodes;
    arena_t* arena;
    lexer_t* lexer;
} parser_t;

parser_t init_parser(lexer_t* lexer, arena_t* arena);
void get_infix(parser_t* parser);
void get_postfix(parser_t* parser);
node_t* create_graph(parser_t* parser);
parser_t create_parser(const char* expr, arena_t* arena);
node_t* parse(parser_t* parser);
void create_var(parser_t* parser, char* name);
void set_var(parser_t* parser, char* name, double val);

#endif // !INCLUDE_PARSER_H_

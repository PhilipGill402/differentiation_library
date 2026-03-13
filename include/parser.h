#ifndef INCLUDE_PARSER_H_
#define INCLUDE_PARSER_H_

#include "lexer.h"
#include "token.h"
#include "string_t.h"
#include "arena.h"
#include "queue.h"
#include "stack.h"
#include "node.h"

typedef struct {
    queue_t tokens;
    stack_s op_stack;
    queue_t output;
    arena_t* arena;
    lexer_t* lexer;
} parser_t;

parser_t create_parser(lexer_t* lexer, arena_t* arena);
void get_infix(parser_t* parser);
void get_postfix(parser_t* parser);
node_t* create_graph(parser_t* parser);
node_t* parse(const char* expr, arena_t* arena);

#endif // !INCLUDE_PARSER_H_

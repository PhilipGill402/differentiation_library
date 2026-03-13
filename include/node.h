#ifndef INCLUDE_NODE_H_
#define INCLUDE_NODE_H_

#include <math.h>
#include "arena.h"

typedef struct node_t node_t;

typedef void (*backward_fn)(node_t* self);

struct node_t {
    double value;
    double grad;
    node_t* left;
    node_t* right;
    backward_fn backward;   // function pointer to differentiation function
};

node_t* create_node(double val, arena_t* arena);
node_t* node_add(node_t* node_a, node_t* node_b, arena_t* arena);
node_t* node_sub(node_t* node_a, node_t* node_b, arena_t* arena);
node_t* node_mul(node_t* node_a, node_t* node_b, arena_t* arena);
node_t* node_div(node_t* node_a, node_t* node_b, arena_t* arena);
node_t* node_pow(node_t* node_a, node_t* node_b, arena_t* arena);
void print_node(node_t* node);

#endif // !INCLUDE_NODE_H_

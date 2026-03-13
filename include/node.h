#ifndef INCLUDE_NODE_H_
#define INCLUDE_NODE_H_

#include <math.h>
#include "arena.h"
#include "string_t.h"
#include "entry.h"
#include "vector.h"

typedef struct node_t node_t;

typedef void (*backward_fn)(node_t* self);

struct node_t {
    double value;
    double grad;
    node_t* left;
    node_t* right;
    backward_fn backward;   // function pointer to differentiation function
                            
    int is_var;
    string_t name;
};

node_t* create_node(double val, arena_t* arena);
node_t* create_var(string_t* name, vector_t* variables, arena_t* arena);
node_t* node_add(node_t* node_a, node_t* node_b, arena_t* arena);
node_t* node_sub(node_t* node_a, node_t* node_b, arena_t* arena);
node_t* node_mul(node_t* node_a, node_t* node_b, arena_t* arena);
node_t* node_div(node_t* node_a, node_t* node_b, arena_t* arena);
node_t* node_pow(node_t* node_a, node_t* node_b, arena_t* arena);
void print_node(node_t* node);

void print_entry(const entry_t* entry);
#endif // !INCLUDE_NODE_H_

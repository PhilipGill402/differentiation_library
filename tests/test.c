#include "backprop.h"
#include <stdio.h>

int main() {
    arena_t arena = create_arena(PAGE_SIZE);

    node_t* a = create_node(2.0, &arena);
    node_t* b = create_node(3.0, &arena);
    node_t* c = node_mul(a, b, &arena);
    node_t* d = node_add(c, a, &arena);

    double ret = backprop(d);

    release_arena(&arena);
}

#include "node.h"

void backward_mul(node_t* self) {
    self->left->grad += self->right->value * self->grad;
    self->right->grad += self->left->value * self->grad;
}

void backward_add(node_t* self) {
    self->left->grad += 1 * self->grad;
    self->right->grad += 1 * self->grad;
}

node_t* create_node(double val, arena_t* arena) {
    node_t* node = reserve(sizeof(node_t), arena);
    node->value = val;
    node->grad = 0;
    node->left = NULL;
    node->right = NULL;
    node->backward = NULL;

    return node;
}

node_t* node_add(node_t* node_a, node_t* node_b, arena_t* arena) {
    node_t* node = reserve(sizeof(node_t), arena);
    node->value = node_a->value + node_b->value;
    node->grad = 0;
    node->left = node_a;
    node->right = node_b;
    node->backward = backward_add;

    return node;
}

node_t* node_mul(node_t* node_a, node_t* node_b, arena_t* arena) {
    node_t* node = reserve(sizeof(node_t), arena);
    node->value = node_a->value * node_b->value;
    node->grad = 0;
    node->left = node_a;
    node->right = node_b;
    node->backward = backward_mul;

    return node;
}

void print_node(node_t* node) {
    printf("NODE(Value: %f, Gradient: %f)", node->value, node->grad);
}

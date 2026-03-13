#include "node.h"

void backward_add(node_t* self) {
    self->left->grad += 1 * self->grad;
    self->right->grad += 1 * self->grad;
}

void backward_sub(node_t* self) {
    self->left->grad += 1 * self->grad;
    self->right->grad += -1 * self->grad;
}

void backward_mul(node_t* self) {
    self->left->grad += self->right->value * self->grad;
    self->right->grad += self->left->value * self->grad;
}

void backward_div(node_t* self) {
    double x = self->left->value;
    double y = self->right->value;

    self->left->grad += (1.0 / y) * self->grad;
    self->right->grad += (-x / (y * y)) * self->grad;
}

void backward_pow(node_t* self) {
    double x = self->left->value;
    double y = self->right->value;

    self->left->grad += y * pow(x, y - 1) * self->grad;
    self->right->grad += log(x) * self->value * self->grad;
}

node_t* create_node(double val, arena_t* arena) {
    node_t* node = reserve(sizeof(node_t), arena);
    node->value = val;
    node->grad = 0;
    node->left = NULL;
    node->right = NULL;
    node->backward = NULL;
    node->is_var = 0;

    return node;
}

node_t* create_var_node(string_t* name, vector_t* variables, arena_t* arena) {
    node_t* node; 

    for (int i = 0; i < variables->size; i++) {
        entry_t entry = *(entry_t*)get(variables, i);
        if (string_compare(name, &entry.name) == 0) {
            return entry.node;
        }
    }
    
    node = reserve(sizeof(node_t), arena);
    node->value = 0;
    node->grad = 0;
    node->left = NULL;
    node->right = NULL;
    node->backward = NULL;
    node->is_var = 1;
    node->name = *name;
    
    entry_t entry = {
        .name = *name,
        .node = node
    };
    push_back(variables, &entry);

    return node;
}

node_t* node_add(node_t* node_a, node_t* node_b, arena_t* arena) {
    node_t* node = reserve(sizeof(node_t), arena);
    node->value = node_a->value + node_b->value;
    node->grad = 0;
    node->left = node_a;
    node->right = node_b;
    node->backward = backward_add;
    node->is_var = 0;

    return node;
}

node_t* node_sub(node_t* node_a, node_t* node_b, arena_t* arena) {
    node_t* node = reserve(sizeof(node_t), arena);
    node->value = node_a->value - node_b->value;
    node->grad = 0;
    node->left = node_a;
    node->right = node_b;
    node->backward = backward_sub;
    node->is_var = 0;

    return node;
}

node_t* node_mul(node_t* node_a, node_t* node_b, arena_t* arena) {
    node_t* node = reserve(sizeof(node_t), arena);
    node->value = node_a->value * node_b->value;
    node->grad = 0;
    node->left = node_a;
    node->right = node_b;
    node->backward = backward_mul;
    node->is_var = 0;

    return node;
}

node_t* node_div(node_t* node_a, node_t* node_b, arena_t* arena) {
    node_t* node = reserve(sizeof(node_t), arena);
    node->value = node_a->value / node_b->value;
    node->grad = 0;
    node->left = node_a;
    node->right = node_b;
    node->backward = backward_div;
    node->is_var = 0;

    return node;
}

node_t* node_pow(node_t* node_a, node_t* node_b, arena_t* arena) {
    node_t* node = reserve(sizeof(node_t), arena);
    node->value = pow(node_a->value, node_b->value);
    node->grad = 0;
    node->left = node_a;
    node->right = node_b;
    node->backward = backward_pow;
    node->is_var = 0;

    return node;
}

void print_node(node_t* node) {
    printf("NODE(Value: %f, Gradient: %f)", node->value, node->grad);
}

void print_entry(const entry_t* entry) {
    printf("ENTRY(Name: ");
    printstr(&entry->name, 1);
    printf(", Value: %f, Gradient: %f)", entry->node->value, entry->node->grad);
}

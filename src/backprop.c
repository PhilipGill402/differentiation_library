#include "backprop.h"

/* HELPERS */
void reset_node(node_t* node) {
    if (node == NULL) {
        return;
    } 

    node->grad = 0;

    reset_node(node->left);
    reset_node(node->right);
}

void calc_grad(node_t* node) {
    if (node == NULL || node->backward == NULL) {
        printf("%f\n", node->grad); 
        return;
    }
    
    node->backward(node);

    calc_grad(node->left);
    calc_grad(node->right);
}

double backprop(node_t* output) {
    //reset all nodes starting at the output node 
    reset_node(output);
    output->grad = 1;
    calc_grad(output);

    return output->grad;
}

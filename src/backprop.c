#include "backprop.h"

double backprop(node_t* root, vector_t* nodes) {
    //reset all nodes starting at the output node 
    for (int i = 0; i < nodes->size; i++) {
        node_t* node = *(node_t**)get(nodes, i);
        node->grad = 0.0;
    }

    root->grad = 1;
    
    for (int i = nodes->size - 1; i >= 0; i--) {
        node_t* node = *(node_t**)get(nodes, i);
        if (node->backward != NULL) {
            node->backward(node);
        }
    }

    return root->grad;
}

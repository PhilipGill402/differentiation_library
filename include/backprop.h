#ifndef INCLUDE_BACKPROP_H_
#define INCLUDE_BACKPROP_H_

#include "node.h"
#include "vector.h"

double backprop(node_t* root, vector_t* nodes);

#endif

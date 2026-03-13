#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct arena_t arena_t;
void* reserve(size_t size, arena_t* arena);
void release(void* ptr, arena_t* arena);
void* move(void* ptr, size_t size, arena_t* arena);

typedef struct stack_s {
    arena_t* allocator; 
    int size;
    int capacity;
    int top;
    size_t element_size;
    void* stack; 
} stack_s;

stack_s create_stack(size_t element_size, arena_t* allocator);
int stack_push(stack_s* stack, void* val);
void* stack_pop(stack_s* stack);
void* stack_top(const stack_s* stack);
int stack_size(const stack_s* stack);
int stack_is_empty(const stack_s* stack);
void stack_release(stack_s* stack);

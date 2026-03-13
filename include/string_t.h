#ifndef INCLUDE_STRING_H_
#define INCLUDE_STRING_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct arena_t arena_t;
void* reserve(size_t size, arena_t* arena);
void release(void* ptr, arena_t* arena);
void* move(void* ptr, size_t size, arena_t* arena);

typedef struct {
    arena_t* allocator; // optional but if used then everything will be allocated on this
    char* str;
    int len;
    int capacity;
} string_t;

string_t create_string(arena_t* arena);
string_t string_literal(const char* str, arena_t* arena);
int string_len(string_t* string);
void string_copy(string_t* dst, string_t* src);
void string_append_chr(string_t* string, char ch);
void string_cat(string_t* dst, string_t* src);
int string_compare(string_t* a, string_t* b);
double string_to_double(string_t* str);
string_t string_upper(string_t* str);
void free_string(string_t* str);
void printstr(const string_t* str, int end);

#endif // !INCLUDE_STRING_H_

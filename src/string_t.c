#include "string_t.h"

string_t create_string(arena_t* allocator) {
    string_t string;
    string.allocator = allocator;

    if (string.allocator) {
        string.str = reserve(sizeof(char) * 10, string.allocator);   
    } else {
        string.str = malloc(sizeof(char) * 10);
    }
    
    string.capacity = 10;
    string.len = 0;

    return string;
}

string_t string_literal(const char* str, arena_t* allocator) {
    string_t string;
    string.allocator = allocator;
    string.len = strlen(str);
    string.capacity = string.len * 2;

    if (string.allocator) {
        string.str = reserve(sizeof(char) * string.capacity, allocator); // this needs to be allocated on the heap so that when we release the string we can just call 'free()'   
    } else {
        string.str = malloc(sizeof(char) * string.capacity); // this needs to be allocated on the heap so that when we release the string we can just call 'free()'
    }
    
    strcpy(string.str, str);

    return string;
}

int string_len(string_t* string) {
    return string->len;
}

void string_copy(string_t* dst, string_t* src) {
    if (src->len > dst->capacity) {
        if (dst->allocator) {
            dst->str = move(dst->str, sizeof(char) * 2 * src->len, dst->allocator); 
        } else {
            dst->str = realloc(dst->str, sizeof(char) * 2 * src->len);
        }
        
        if (!dst->str) {
            fprintf(stderr, "realloc");
            return;
        }

        dst->capacity = 2 * src->len;
    } 

    for (int i = 0; i < src->len; i++) {
        dst->str[i] = src->str[i];
    }

    dst->len = src->len;
}

void string_append_chr(string_t* string, char ch) {
    if (string->len + 1 > string->capacity) {
        if (string->allocator) {
            string->str = move(string->str, sizeof(char) * 2 * (string->len + 1), string->allocator); 
        } else {
            string->str = realloc(string->str, sizeof(char) * 2 * (string->len + 1));
        }


        if (!string->str) {
            fprintf(stderr, "realloc");
            return;
        }

        string->capacity = 2 * (string->len + 1);
    }

    string->str[string->len] = ch;
    string->len++;
}

void string_cat(string_t* dst, string_t* src) {
    if (dst->capacity < src->len + dst->len) {
        if (dst->allocator) {
            dst->str = move(dst->str, sizeof(char) * 2 * (src->len + dst->len), dst->allocator); 
        } else {
            dst->str = realloc(dst->str, sizeof(char) * 2 * (src->len + dst->len));
        }


        if (!dst->str) {
            fprintf(stderr, "realloc");
            return;
        }

        dst->capacity = 2 * (src->len + dst->len);
    }
    
    for (int i = dst->len, j = 0; i < dst->len + src->len; i++, j++) {
        dst->str[i] = src->str[j];
    }

    dst->len += src->len;
}

int string_compare(string_t* a, string_t* b) { 
    for (int i = 0; i < a->len && i < b->len; i++) {
        if (a->str[i] != b->str[i]) {
            return a->str[i] - b->str[i];
        }
    } 

    return 0;
}

double string_to_double(string_t* str) {
    char buffer[str->len + 1];
    
    for (int i = 0; i < str->len; i++) {
        buffer[i] = str->str[i];
    }

    buffer[str->len] = '\0';

    return strtod(buffer, NULL);
}

string_t string_upper(string_t* str) {
    string_t new_str = create_string(str->allocator);

    for (int i = 0; i < str->len; i++) {
        string_append_chr(&new_str, toupper(str->str[i])); 
    }

    return new_str;
}

void free_string(string_t* str) {
    if (str->allocator) {
        release(str->str, str->allocator);
    } else {
        free(str->str);
    }

    str->str = NULL;
    str->len = 0;
    str->capacity = 0;
}

void printstr(const string_t* str, int end) {
    for (int i = 0; i < str->len; i++) {
        putchar(str->str[i]);
    }

    if (end == 0) {
        putchar('\n');
    }
}

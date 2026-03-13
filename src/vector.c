#include "vector.h"

vector_t create_vector(size_t element_size, arena_t* allocator){
    //start with 10 elements
    vector_t vec;
    vec.allocator = allocator;

    if (vec.allocator) {
        vec.array = reserve(10 * element_size, vec.allocator);
    } else {
        vec.array = malloc(10 * element_size);
    }

    if (!vec.array){
        fprintf(stderr, "Failed to allocate memory for the array\n");
        vec.size = 0;
        vec.capacity = 0;
        
        //returns zeroed out array in case of error
        return vec;
    }

    vec.size = 0;
    vec.capacity = 10;
    vec.element_size = element_size;

    return vec;
}

vector_iterator_t* iterator(vector_t* vec, arena_t* allocator) {
    return create_iterator(vec, allocator); 
}

void vector_reserve(vector_t* vec, int new_capacity) {
    if (vec->allocator) {
        vec->array = move(vec->array, vec->element_size * new_capacity, vec->allocator); 
    } else {
        void* ptr = realloc(vec->array, vec->element_size * new_capacity);
        vec->array = ptr;
    }

    if (vec->array == NULL){
        fprintf(stderr, "Reallocation failed\n");
        return;
    }
    
    vec->capacity = new_capacity;
}

int vector_size(const vector_t* vec) {
    return vec->size;
}

void push_back(vector_t* vec, void* element) {
    // if the array is full then double its capacity  
    if (vector_size(vec) == vec->capacity){
        vector_reserve(vec, vec->capacity * 2);
    } 
    
    memcpy((char*)vec->array + vec->size * vec->element_size, element, vec->element_size);
    vec->size++;
}

void* pop_back(vector_t* vec){
    if (vector_size(vec) == 0){
        fprintf(stderr, "pop_back() called on empty vector\n");
        return NULL;
    } 

    //if the size of the vector drops to 25% of its capacity then halve its capacity
    if ((vector_size(vec) - 1) * 4 <= vec->capacity){
        vector_reserve(vec, (int)(vec->capacity / 2));
    }

    vec->size--;
    void* element = (char*)vec->array + vec->size * vec->element_size;

    return element;
}

void free_vector(vector_t* vec){
    free(vec->array);
    vec->array = NULL;
    vec->size = 0;
    vec->capacity = 0;
}

bool empty(const vector_t* vec){
    if (vec == NULL) {
        fprintf(stderr, "empty() called on NULL vector\n");
        return true;
    }
    
    return vector_size(vec) == 0;
}

void print_vector(const vector_t* vec, void (*print_element)(void*)) {
    printf("<"); 
    for (int i = 0; i < vector_size(vec); i++) {
        print_element(get(vec, i));
        
        if (i != vector_size(vec) - 1) {
            printf(", ");
        }
    }
    printf(">\n");
}

void* get(const vector_t* vec, int index){
    if (index > vector_size(vec)){
        fprintf(stderr, "'index' is out of the range of the vector");
        return NULL;
    }
    return (char*)vec->array + index * vec->element_size;
}

void* front(const vector_t* vec){
    if (empty(vec)){
        fprintf(stderr, "cannot get first element of empty vector\n");
        return NULL;
    }

    return vec->array;
}

void* back(const vector_t* vec){
    if (empty(vec)){
        fprintf(stderr, "cannot get last element of empty vector\n");
        return NULL;
    }

    return (char*)vec->array + vec->size * vec->element_size;
}

void insert(vector_t* vec, void* element, int index){
    //if the vector is full then reserve double its capacity
    if (vector_size(vec) + 1 > vec->capacity){
        vector_reserve(vec, vec->capacity * 2);
    }

    //shift everything over one
    for (int i = vector_size(vec) + 1; i >= index; i--){
        memcpy((char*)vec->array + i * vec->element_size, (char*)vec->array + (i - 1) * vec->element_size, vec->element_size); 
    }
    //set the element at index to the given element
    memcpy((char*)vec->array + index * vec->element_size, element, vec->element_size);
    vec->size++;
}

void erase(vector_t* vec, int index){
    if (index >= vector_size(vec)){
        fprintf(stderr, "index given is outside the bounds of the vector\n");
        return;
    }
    
    //we're removing the last element
    if (index == vector_size(vec) - 1){
        vec->size--;
    } else {
        for (int i = index; i < vector_size(vec) - 1; i++){
            memcpy((char*)vec->array + i * vec->element_size, (char*)vec->array + (i + 1) * vec->element_size, vec->element_size); 
        }

        vec->size--;
    }
    
    if (vector_size(vec) * 4 <= vec->capacity){
        vector_reserve(vec, (int)(vec->capacity / 2));
    }

    
}

void swap(vector_t* vec, vector_t* other_vec){
    //array 
    void* tmp = vec->array; 
    vec->array = other_vec->array;
    other_vec->array = tmp;
    
    //size
    int temp = vec->size;
    vec->size = other_vec->size;
    other_vec->size = temp;

    //capcaity
    temp = vec->capacity;
    vec->capacity = other_vec->capacity;
    other_vec->capacity = temp;

}

void clear(vector_t* vec){
    vec->size = 0;
    vector_reserve(vec, 10);
}

void assign(vector_t* vec, void* element, int num_copies){
    if (num_copies > vec->capacity){
        vector_reserve(vec, num_copies * 2);
    }

    vec->size = num_copies;

    for (int i = 0; i < vector_size(vec); i++){
        memcpy((char*)vec->array + i * vec->element_size, element, vec->element_size); 
    }
}

int capacity(const vector_t* vec){
    return vec->capacity;
}

void downsize(vector_t* vec, int new_size){
    if (new_size > vector_size(vec)){
        fprintf(stderr, "given size is larger than size of the vector\n");
        return;
    } 
     
    vec->size = new_size;

    if (vector_size(vec) > capacity(vec)){
        vector_reserve(vec, 2 * vector_size(vec));
    }
}

void upsize(vector_t* vec, int new_size, void* element){
    if (new_size < vector_size(vec)){
        fprintf(stderr, "given size is smaller than size of the vector\n");
        return;
    } 
    
    int old_size = vec->size; 
    vec->size = new_size;

    if (vector_size(vec) > capacity(vec)){
        vector_reserve(vec, 2 * vector_size(vec));
    }

    for (int i = old_size; i < new_size; i++){
        memcpy((char*)vec->array + i * vec->element_size, element, vec->element_size); 
    }
}

void shrink_to_fit(vector_t* vec){
    vec->capacity = vec->size;
}

void* data(const vector_t* vec){
    return vec->array;
}



vector_iterator_t* create_iterator(const vector_t* vec, arena_t* allocator){
    vector_iterator_t* itr;

    if (allocator) {
        itr = reserve(sizeof(vector_iterator_t*), allocator);
    } else {
        itr = malloc(sizeof(*itr));
    }

    itr->current = vec->array;
    itr->end = vec->array + vector_size(vec) * vec->element_size;
    itr->element_size = vec->element_size;
    return itr;
}

int has_next(const vector_iterator_t* itr){
    return itr->current != itr->end;
}


void* next(vector_iterator_t* itr){
    if (has_next(itr)){
        void* element = itr->current;
        itr->current = (char*)itr->current + itr->element_size;

        return element;
    } else {
        fprintf(stderr, "vector has no more elements\n");
        return NULL;
    }
}

void resize(vector_t* vec, int new_size){
    if (vector_size(vec) > capacity(vec)){
        vector_reserve(vec, 2 * new_size);
    } 
    
    vec->size = new_size;
}

int contains(const vector_t* vec, void* element, int (*comparator)(void*, void*)) {
    for (int i = 0; i < vec->size; i++) {
        if (comparator(get(vec, i), element) == 0) {
            return 1;
        }
    }

    return 0;
}



#ifndef ALLOCATOR_TESTS_H
#define ALLOCATOR_TESTS_H

#include <stddef.h>

void my_allocator_init(size_t);
void my_allocator_destroy(void);
void* my_malloc(size_t);
void my_free(void*);

void test_free_list_allocator(void);
void test_best_fit_allocator(void);

#endif
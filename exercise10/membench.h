#ifndef MEMBENCH_H
#define MEMBENCH_H

#include <stddef.h>

typedef void (*init_allocator_fn)(size_t);
typedef void (*destroy_allocator_fn)(void);

typedef void* (*malloc_fn)(size_t);
typedef void (*free_fn)(void*);

/**
 * Runs a multi-threaded benchmark for the provided allocation/deallocation
 * functions, comparing performance to the system allocator (malloc/free).
 * Results are printed to stdout.
 *
 * The init/destroy functions are only called once.
 * The provided allocation/deallocation functions are assumed to be thread-safe.
 *
 * @param my_init Initializes the allocator, passing the total amount
 *                of memory that should be reserved for the benchmark.
 * @param my_destroy Destroys the allocator, freeing all reserved memory.
 * @param my_malloc Custom allocation function that mimics malloc() in behavior.
 * @param my_free Custom deallocation function that mimics free() in behavior.
 */
void run_membench_global(init_allocator_fn my_init, destroy_allocator_fn my_destroy,
                         malloc_fn my_malloc, free_fn my_free);

/**
 * Same as run_membench_global, except that the init/destroy functions are
 * called once for each thread of the multi-threaded benchmark.
 */
void run_membench_thread_local(init_allocator_fn my_init, destroy_allocator_fn my_destroy,
                               malloc_fn my_malloc, free_fn my_free);

#endif

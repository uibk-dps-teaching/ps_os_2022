#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "membench.h"

#define POOL_SIZE (1024 * 1024 * 128)
#define MAX_ALLOC_MULTIPLIER 4

typedef struct {
	size_t num_allocations;
	size_t alloc_size;
	init_allocator_fn my_init;
	destroy_allocator_fn my_destroy;
	malloc_fn my_malloc;
	free_fn my_free;
} thread_context;

static int64_t get_timestamp_us(void) {
	struct timeval tv;
	const int ret = gettimeofday(&tv, NULL);
	if(ret != 0) {
		perror("gettimeofday");
		exit(EXIT_FAILURE);
	}
	return tv.tv_sec * 1000 * 1000 + tv.tv_usec;
}

static void* thread_fn(void* arg) {
	thread_context* ctx = arg;
	unsigned int seed = time(NULL);
	void** ptrs = calloc(ctx->num_allocations, sizeof(void*));

	if(ctx->my_init != NULL) {
		ctx->my_init(POOL_SIZE);
	}

	const int64_t before = get_timestamp_us();

	// -----------------------------------
	//    Start of benchmarked section
	// -----------------------------------

	// Make N allocations of random size
	for(size_t i = 0; i < ctx->num_allocations; ++i) {
		const size_t size = ctx->alloc_size * (1 + rand_r(&seed) % MAX_ALLOC_MULTIPLIER);
		ptrs[i] = ctx->my_malloc(size);
		assert(ptrs[i] != NULL);
		memset(ptrs[i], 0xFF, size);
	}

	// Free ~50% of allocations
	for(size_t i = 0; i < ctx->num_allocations; ++i) {
		if(rand_r(&seed) % 2 == 0) {
			ctx->my_free(ptrs[i]);
			ptrs[i] = NULL;
		}
	}

	// Re-allocate those ~50%
	for(size_t i = 0; i < ctx->num_allocations; ++i) {
		if(ptrs[i] == NULL) {
			const size_t size = ctx->alloc_size * (1 + rand_r(&seed) % MAX_ALLOC_MULTIPLIER);
			ptrs[i] = ctx->my_malloc(size);
			assert(ptrs[i] != NULL);
		}
	}

	// Free all allocations
	for(size_t i = 0; i < ctx->num_allocations; ++i) {
		ctx->my_free(ptrs[i]);
	}

	// -----------------------------------
	//    End of benchmarked section
	// -----------------------------------

	const int64_t after = get_timestamp_us();

	free(ptrs);

	if(ctx->my_destroy != NULL) {
		ctx->my_destroy();
	}

	return (void*)(intptr_t)(after - before);
}

static double run_config(size_t num_threads, thread_context* ctx) {
	pthread_t thread_ids[num_threads];
	for(size_t i = 0; i < num_threads; ++i) {
		pthread_create(&thread_ids[i], NULL, thread_fn, ctx);
	}

	double time_sum = 0.0;
	for(size_t i = 0; i < num_threads; ++i) {
		intptr_t delta_time;
		pthread_join(thread_ids[i], (void**)&delta_time);
		time_sum += delta_time;
	}

	return time_sum / num_threads / 1000.0;
}

static void run_membench(init_allocator_fn my_init, destroy_allocator_fn my_destroy,
                         malloc_fn my_malloc, free_fn my_free, bool init_per_thread) {
	if(!init_per_thread) {
		my_init(POOL_SIZE);
	}

	const size_t configs[][3] = {
		{ 1, 1000, 256 }, { 10, 1000, 256 }, { 50, 1000, 256 }, { 100, 1000, 32 }
	};
	const size_t num_configs = sizeof(configs) / sizeof(size_t[3]);

	for(size_t i = 0; i < num_configs; ++i) {
		const size_t num_threads = configs[i][0];
		const size_t num_allocations = configs[i][1];
		const size_t alloc_size = configs[i][2];
		thread_context system_ctx = { .num_allocations = num_allocations,
			                          .alloc_size = alloc_size,
			                          .my_init = NULL,
			                          .my_destroy = NULL,
			                          .my_malloc = malloc,
			                          .my_free = free };
		thread_context custom_ctx = { .num_allocations = num_allocations,
			                          .alloc_size = alloc_size,
			                          .my_init = init_per_thread ? my_init : NULL,
			                          .my_destroy = init_per_thread ? my_destroy : NULL,
			                          .my_malloc = my_malloc,
			                          .my_free = my_free };

		printf("%zu thread(s), %zu allocations of size %zu - %zu byte per thread. Avg time per "
		       "thread:\n",
		       num_threads, num_allocations, alloc_size, alloc_size * MAX_ALLOC_MULTIPLIER);
		printf("\tSystem: %.2lf ms\n", run_config(num_threads, &system_ctx));
		printf("\tCustom: %.2lf ms\n", run_config(num_threads, &custom_ctx));
	}

	if(!init_per_thread) {
		my_destroy();
	}
}

void run_membench_global(init_allocator_fn my_init, destroy_allocator_fn my_destroy,
                         malloc_fn my_malloc, free_fn my_free) {
	run_membench(my_init, my_destroy, my_malloc, my_free, false);
}

void run_membench_thread_local(init_allocator_fn my_init, destroy_allocator_fn my_destroy,
                               malloc_fn my_malloc, free_fn my_free) {
	run_membench(my_init, my_destroy, my_malloc, my_free, true);
}

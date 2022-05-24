# Exercise Sheet 10

In this exercise sheet you will implement _custom allocators_ and compare their performance to the system allocator.

## Task 1

In some cases the system allocator (`malloc()` / `free()`) may not be suitable (for example in embedded programming) or may not perform well for atypical allocation patterns (for example in video games and other highly optimized programs).
In these situations, it can be beneficial to implement a _custom allocator_.

One of the simplest allocators is the so-called _free-list allocator_, which you will implement in this task.
Free-list allocators come in different forms, but the most basic variant allows to allocate _blocks_ of a fixed size from a given area of memory, called the _memory pool_.

As the name implies, a free-list allocator consists of a linked list of free blocks which are available to be allocated.
Importantly, this list is embedded within the memory pool itself, i.e., lives inside the very memory region that is being managed for the user (this is called an _intrusive data structure_).
More specifically, each block should be preceded by a _header_ that contains a pointer to the next free block in the list.
This diagram illustrates how your free-list may look like initially:

```
           │------------------ Memory pool ------------------
           ┌───┬────────┬───┬────────┬───┬────────┬───┬──────
           │   │        │   │        │   │        │   │
Next Free  │   │  Free  │   │  Free  │   │  Free  │   │  ...
    │      │   │        │   │        │   │        │   │
    │      └▲─┬┴────────┴▲─┬┴────────┴▲─┬┴────────┴▲─┬┴──────
    └───────┘ └──────────┘ └──────────┘ └──────────┘ └───────
```

The user can then allocate one block at a time.
After the first two blocks have been allocated, the list may look like this:

```
           │------------------ Memory pool ------------------
           ┌───┬────────┬───┬────────┬───┬────────┬───┬──────
           │   │        │   │        │   │        │   │
Next Free  │   │ Allocd │   │ Allocd │   │  Free  │   │  ...
    │      │   │        │   │        │   │        │   │
    │      └───┴────────┴───┴────────┴▲─┬┴────────┴▲─┬┴──────
    └─────────────────────────────────┘ └──────────┘ └───────
```

If the first block is then free'd again, it's header will point to the third block, which is the next free block:

```
           │------------------ Memory pool ------------------
           ┌───┬────────┬───┬────────┬───┬────────┬───┬──────
           │   │        │   │        │   │        │   │
Next Free  │   │  Free  │   │ Allocd │   │  Free  │   │  ...
    │      │   │        │   │        │   │        │   │
    │      └▲─┬┴────────┴───┴────────┴▲─┬┴────────┴▲─┬┴──────
    └───────┘ └───────────────────────┘ └──────────┘ └───────
```

To obtain a pool of memory that can then managed by your allocator, use `mmap` with the `MAP_PRIVATE` and `MAP_ANONYMOUS` flags.
You can do this once during initialization; it should be the only time your custom allocator interacts with the operating system's memory management facilities (except when releasing the pool at the end).
**Important**: Your solution must not use `malloc` or `free`.

Your allocator should provide these four functions:

- `void* my_malloc(size_t size)` allocates a single block from the list, returning its address.
  If no free block is available, it returns `NULL`.
  While the signature of this function mimics `malloc()`, the `size` parameter does not influence the size of your blocks (they all have the same fixed block size).
  Return `NULL` if `size` exceeds the size of a single block.
- `void my_free(void* ptr)` frees a block, returning it to the list.
- `void my_allocator_init(size_t size)` initializes the memory pool for the allocator to a total size of `size`.
  Your allocator can at most allocate however many blocks (including list headers) fit into `size`.
- `void my_allocator_destroy(void)` frees the memory pool for the allocator.

Alongside this exercise sheet you will find two additional translation units:

- `allocator_tests.c` contains a function `test_free_list_allocator` that checks whether your implementation is behaving correctly.
  We recommend to use this during development.
  If you think your implementation is correct but some of the tests are failing, please consult the tutorium.
- `membench.c` is a benchmarking program designed to evaluate the performance of your allocator against the system allocator.
  Use the `run_membench_global` function to run the benchmark.
  Briefly interpret the results.

Your solution allow running both the tests and the benchmark without manual changes to the code or Makefile (i.e., produce multiple executables or implement a command line parameter).

**Additional notes:**

- Your allocator is not required to support allocating more than one block at a time.
- Assume a fixed block size of `BLOCK_SIZE = 1024` bytes.
- Your allocator must be thread-safe. Make sure to properly synchronize critical sections.
- Make sure to use Valgrind to find potential errors in your implementation.
- There should be no need to modify any of the provided files.
- Use an optimized build for benchmarking.

## Task 2

One of the shortcomings of the simple free-list allocator from Task 1 is that it can only allocate blocks of a fixed size.
To alleviate this problem, turn your allocator into a _best fit_ allocator that supports blocks of varying size.

The basic working principle remains the same, however your list initially starts out with only one huge block that spans the entire memory pool.
Once an allocation of a given `size` is made, the block is split into two:
One part of size `size` is returned to the user, while the remainder is reinserted into the free list.

Given that the free-list now consists of blocks of varying size, your allocator should not simply allocate the first block from the free-list.
Instead, it should try and find the block in the list whose size is closest to the requested `size` (hence the name "best fit" allocator).

Again:

- Use the `test_best_fit_allocator` function from `allocator_tests.c` to check whether your implementation is behaving correctly.
- Benchmark the performance of your allocator using `run_membench_global()`.

**Additional notes:**

- Your list headers will no longer be spaced evenly throughout the memory pool.
  Instead, you have to create them dynamically based on the number of blocks (and their sizes) currently in the list.
- Additionally, headers will now need to store more information than simply a pointer to the next block.
- Keep the number of entries in the free-list as small as possible.
  Once a block is free'd, check whether it can be merged with any of its neighboring blocks.

## Task 3

For this task, instead of a single global allocator that requires synchronization to be thread-safe, create a separate _thread local_ allocator for each thread that uses it.
To do so, you can leverage C11's thread local storage class specifier (the `_Thread_local` keyword).

Convert the allocator(s) you implemented in Task 1 and/or Task 2, and again measure the performance, now using the `run_membench_thread_local()` function.

---

```
exc09_csXXXXXX.zip
└── ???
```

**There is no fixed folder structure prescribed this time, choose a structure that fits your setup and try to avoid code duplication.**

- [ ] Run solutions using `valgrind` where applicable
- [ ] Auto-format all source files
- [ ] Check your submission on ZID-GPL
- [ ] Submit zip
- [ ] Submit tally list

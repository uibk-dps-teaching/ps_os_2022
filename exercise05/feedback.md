# Feedback — Exercise Sheet 5

## Allocation of dynamic memory

### Task 1

The task specified to allocate (shared) memory for a ring buffer capable of storing `B` elements of type `uint64_t`.
We therefore need to allocate `B * sizeof(uint64_t)` byte (where `sizeof(uint64_t)` is always 64 bit, or 8 byte, regardless of the system architecture).
Since we need this calculation in several places (`ftruncate`, in case we are using named shared memory, `mmap`, `munmap`), instead of repeating it each time (which is error-prone), it can be handy to abstract it into a macro like so:

```c
#define SHM_SIZE(buf_size) ((buf_size) * sizeof(uint64_t))
// and use it like this
ftruncate(shm_fd, SHM_SIZE(B));
```

This is especially useful if the size calculation changes later down the line, for example if we decide to store the final result of the calculation in a dedicated element of the buffer:

```c
// (buf_size + 1) for storing result
#define SHM_SIZE(buf_size) ((buf_size + 1) * sizeof(uint64_t))
```

Note that this is not the only way of returning the result.
An arguably more readable solution might be to store a `struct` within the shared memory region which contains both the buffer as well as a field for the result (see next section).

### Task 2

One way of sharing the semaphores between producer and consumer processes is to also store them within the shared memory region.
To do so, we can create a `struct` that contains both semaphores, the ring buffer as well as a field for storing the result.

Note that the buffer array itself has no size.
Instead, it will have whatever size we allocate in addition to `sizeof(shared_context)`.
This is called a _flexible array member_ and only works for the _last_ element of a struct.

```c
typedef struct {
    sem_t write;
    sem_t read;
    uint64_t sum;
    uint64_t buffer[];
} shared_context;

#define SHM_SIZE(buf_size) (sizeof(shared_context) + ((buf_size) * sizeof(uint64_t)))
```

Since `mmap` returns a pointer of type `void*`, we can then simply cast it to our desired type (`shared_context*`), just like we do for `malloc`:

```c
shared_context* ptr = mmap(...)
sem_init(&ptr->write, ...)
```

# Exercise Sheet 5

## Task 1

In this task, you will implement the classic producer-consumer pattern using multiple processes operating on shared memory.

Begin by reading `man 7 shm_overview`.
Take note of additional linker flags that may be required.

Create a program that receives two integer parameters `N` and `B` through the command line.
The program acts as the parent of two child processes, the _producer_ and the _consumer_.
The parent begins by attaching a shared memory segment that has space for a buffer array and the result of the computation performed by the consumer (explained below).
It then creates the two child processes and waits for their termination.
Finally, it prints the result of the computation, releases the shared memory segment and exits.

The producer iteratively writes all numbers in `[1, N]` into an array of size `B` that is stored in shared memory.
If `N` > `B`, the producer continues writing by wrapping around, overwriting the values previously stored within the array (this is called a _ring buffer_).
You can implement this functionality using the modulo operator: `buffer[i % B]`.

The consumer iteratively sums up `N` subsequent numbers from the ring buffer, thereby effectively computing the sum of all numbers in `[1, N]`.
It then writes the resulting sum into the shared memory segment and exits.

Example output:

```
> ./task1 10 10
Result: 55
```

Analyze the obtained output.
Is the computed result the expected value?
Does the computed value change across different runs?
What happens when you try different and larger values for `N` and `B`, for example 10,000 or 100,000?
Try to explain the behavior.

Notes:

- There is no need to attach multiple shared memory segments.
  Think about a good way of storing the entire context (the ring buffer and the result) inside a single shared memory segment.
- Make sure to use numeric data types that can store sufficiently large values, for example `uint64_t`, which is defined in `<stdint.h>`.
- This exercise MUST be implemented using the POSIX shared memory mechanisms.

## Task 2

In this task, you will use _semaphores_ to synchronize access to shared data structures.
Begin by reading `man 7 sem_overview`.
Take note of any additional compiler flags that may be required.
This exercise MUST be implemented using the POSIX semaphore facilities.

### Task 2a

Implement the same program as before.
However this time, ensure that the consumer only reads each value in `[1, N]` exactly once from the ring buffer.
Likewise, ensure that the producer only overwrites values in the ring buffer that have already been read by the consumer.
Use two semaphores to implement this behavior.
You _must not_ use any other synchronization mechanisms (i.e. mutexes, atomics, etc).
Your program should now always compute the correct result.

**Important**: Make sure that the producer and consumer can perform work on the ring buffer _concurrently_, i.e., the semaphores should be used for _counting_.

Measure the execution time of your solutions for Task 1 and Task 2a using `/usr/bin/time`.
What do you notice?

### Task 2b

Answer the following questions:

- What is the difference between shared memory and e.g. a global variable after calling `fork()`?
- What is a race condition?
- Why do race conditions occur?
- Is the output of a program with a race condition always incorrect?
- If you run your program 100 times and always get the correct result, does that mean that there is no race condition?
- What is synchronization?
- Why are semaphores well suited for synchronizing producer-consumer patterns?
- There are two ways of creating a POSIX semaphore.
  Which would you use when?
- Could Task 2 also be implemented using a single semaphore?

## Task 3

In this task we will look at **POSIX** message queues — see `mq_overview(7)`.

The goal of this task is to simulate a printer queue, with multiple printing jobs being processed according to their priority.
The simulation consists of two executables: The `print_server` receives input via a **message queue** and "prints" the given input by writing it to `stdout`.
One or more clients, simply called `print`, submit jobs to the message queue.

Server behavior:

- The server receives the name of the message queue as its only argument.
  To avoid naming collisions with other students on ZID-GPL, we recommend naming your queue based on your user name.
- It creates the message queue and continuously waits for incoming print jobs, taking priorities into account.
- Upon receiving a job, it prints `"Serving print job with priority <priority>:"`, followed by the actual text to be printed.
- Simulate printing speed by sleeping 200 milliseconds after printing each character.
  You can print single characters with `putc`, make sure to flush `stdout` to see them being printed one by one.
- Find a way of **gracefully** shutting down the server.
  You may find inspiration in earlier exercise sheets.
  Upon shutdown, the server discards all outstanding jobs, prints `"Shutting down"`, cleans up and exits.

The client receives the message queue name as well as a _priority_ as its arguments.
It then opens the message queue created by the server, sends a print job with its contents read from `stdin` and then immediately exits.

Example output:

```
TERMINAL 1                     TERMINAL 2                            TERMINAL 3
$ ./print_server "/csXXXX"
                               $ ./print "/csXXXX" 1 < file1.txt
Serving print job with priority 1:
<content of file1.txt>
                                                                     $ ./print "/csXXXXX" 10 < file2.txt
                               $ ./print "/csXXXX" 99 < file3.txt

Serving print job with priority 99:
<content of file3.txt>

Serving print job with priority 10:
<content of file2.txt>
```

In this example, the printer starts printing the content of `file1.txt` since it is the first job to be received.
While this message content is printed, 2 other jobs are added to the print queue.
After the first job has been completed, the server takes the next message (according to priority) from the queue.

Additional notes:

- Pick a reasonable message size, you may assume that the provided input fits into one message
- Ensure the message queue is properly cleaned up on shutdown
- Make sure to use appropriate file access modes

---

```
exc05_csXXXXXX.zip
├── task1/
    ├── Makefile
│   └── task1.c
├── task2/
│   ├── Makefile
│   ├── task2.c
│   └── solution.txt
└── task3/
    ├── Makefile
    ├── task3_printer.c
    └── task3_printer_server.c
```

- [ ] Read and understand [Exercise Sheet 3 Feedback](../exercise03/feedback.md)
- [ ] Check that your solution conforms to the guidelines given in these feedback documents
- [ ] Run solutions using `valgrind` where applicable
- [ ] Auto-format all source files
- [ ] Check your zip archive using [`check_exc05`](check_exc05)¹
- [ ] Check your submission on ZID-GPL
- [ ] Submit zip
- [ ] Submit tally list

¹ If `check_exc05` reports that your zip is not correct, run it on ZID-GPL.

# Exercise Sheet 6

## Task 1

Begin by skimming through `man pthreads`.
Take note of additional compiler flags that may be required.

### Task 1a

Write a program that stores a single global variable `my_global` of type `int` and initialize it to `1`.

The program begins by printing the value of `my_global` and then creates a child process.
The child increments the value of `my_global` by `1` and exits.
The parent waits for the child to exit and prints the value again.

Next, the program (now referred to as the _main thread_) spawns a single POSIX thread.
The thread does the same thing as the child process, i.e., increments the global variable and immediately exits.
The main thread waits for the thread to finish, and prints the value one more time.

What do you notice? Explain the behavior.

### Task 1b

Write a program that receives an arbitrary number of integers as arguments.
Let `N` be the number of arguments provided by the user.

The program creates `N` threads, each of which is assigned an ID `i`, with `0 < i <= N`.
The `i`-th thread computes a _prefix sum_ of all numbers up to and including the `i`-th argument (i.e., the first thread simply returns the first number, the second thread computes the sum of the first and second number, and so on).

The main thread then waits for each thread to complete and prints their result _in order_, with the format `"sum<i> = <sum>"`.

Investigate how you can pass multiple arguments to a thread function, as well as how to receive a result from it.
The program **must not** make use of any global variables.

Ensure that all threads compute their sums concurrently, not one after another.

Example output:

```
$ ./task1b 4 3 9 5 21
sum1 = 4
sum2 = 7
sum3 = 16
sum4 = 21
sum5 = 42
```

## Task 2

In this task you will revisit the producer-consumer pattern, this time with multiple consumers implemented through POSIX threads.

Your program should spawn 5 threads, the _consumers_, which all try to read elements from a queue storing integers.
For the queue, you can use the basic implementation that is provided in [myqueue.h](myqueue.h).

- When a consumer thread successfully reads an element, it adds it to its local `sum`.
  When the element is `0`, it prints out the sum, returns it to the main thread and exits.
- The main thread acts as the _producer_.
  After spawning all 5 consumers, it feeds 10,000 entries of value `1` into the queue, followed by 5 entries of value `0`.
- The main thread then waits until all consumers have finished and computes the final sum from all the partial results, prints it to the console and exits.

To avoid race conditions, synchronize access to the queue by using the `pthread_mutex` facilities.
Carefully consider all locations where you might need synchronization.

**Important**: The consumer threads should be able to begin processing elements from the queue as soon as the producer has added them, NOT only once all 10,000 elements have been added.

Example output:

```
$ ./task2
Consumer 2 sum: 2165
Consumer 1 sum: 1501
Consumer 4 sum: 2320
Consumer 0 sum: 2219
Consumer 3 sum: 1795
Final sum: 10000
```

Instead of a mutex, could a semaphore be used in this situation?

## Task 3

Continuing with the single producer, multiple consumer example, increase the number of consumers to 500 and the number of values to 100,000.

Now use a pthread _condition variable_ to signal the availability of new elements to the consumers.
What is the advantage of using a condition variable in this case, compared to using plain mutexes?

How does the program behavior differ between the two variants?
Measure the execution time of both using `/usr/bin/time -v` and report your findings.
Make sure to use an optimized build (`-O2` or `-O3`).

---

```
exc06_csXXXXXX.zip
├── task1/
│   ├── Makefile
│   ├── solution.txt
│   ├── task1a.c
│   └── task1b.c
├── task2/
│   ├── Makefile
│   ├── myqueue.h
│   ├── solution.txt
│   └── task2.c
└── task3/
    ├── Makefile
    ├── myqueue.h
    ├── solution.txt
    └── task3.c
```

- [ ] Read and understand [Exercise Sheet 4 Feedback](../exercise04/feedback.md)
- [ ] Check that your solution conforms to the guidelines given in these feedback documents
- [ ] Run solutions using `valgrind` where applicable
- [ ] Auto-format all source files
- [ ] Check your zip archive using [`check_exc06`](check_exc06)¹
- [ ] Check your submission on ZID-GPL
- [ ] Submit zip
- [ ] Submit tally list

¹ If the script reports that your zip is not correct, run it on ZID-GPL.

# Exercise Sheet 7

## Task 1

Write a program that initializes an integer value `X` to 100,000 and subsequently creates 1,000 threads.
Each of these newly created threads should execute a loop of 10,000 iterations.
In each of these iterations, the value `X` should be decreased by one.
The main thread waits for all the other threads to finish and then writes the final value of `X` to the console.

Then, implement two variants:

1. Protect access to `X` using a mutex.
   All threads should be able to advance in the loop concurrently.
2. Use C11 atomics to perform the decrement operation.

How does the program behavior differ between the two variants?
Measure the execution time of both using `/usr/bin/time -v` and report your findings.
Make sure to use an optimized build (`-O2` or `-O3`).

## Task 2

Create a program that spawns two threads.
The first thread pushes 10 million integers with value 1 into a queue (you may reuse last week's implementation), followed by a single 0, then exits.
The other thread pops elements from the queue and adds them to a local sum until it encounters a 0.
It then prints the sum and exits.

Synchronize access to the queue using a mutex.
However this time, instead of using a pthread mutex, implement your own mutex using atomics.
Your solution should at least provide two functions: `my_mutex_lock` and `my_mutex_unlock`.
You may come up with additional functions, if needed.

Think about ways of representing the state of your mutex using an atomic variable.
Instead of manipulating that variable directly however, use the `atomic_flag_test_and_set` and `atomic_flag_clear` functions (why is this necessary?).
_Alternatively_, if you prefer, you can also use `atomic_compare_exchange_weak`.
Make sure you fully understand the behavior of these functions and how they could be used to implement your mutex before proceeding.

We want to measure the performance of our own mutex vs. the mutex provided by the system.
Provide both variants (using your own mutex and `pthread_mutex_t`) in your uploaded solution.
An elegant way of doing this while minimizing code duplication could be to use preprocessor macros, like so:

```c
#if USE_MY_MUTEX
    #define MUTEX_LOCK my_mutex_lock
    // ... other functions
#else
    #define MUTEX_LOCK pthread_mutex_lock
    // ... other functions
#endif
```

You can then use the `MUTEX_LOCK` macro for both, and switch between the two implementations by adding `-DUSE_MY_MUTEX=<0 or 1>` to your compiler call.

Measure the execution time with `/usr/bin/time -v`.
Report your findings and interpret the results.
Make sure to use an optimized build (`-O2` or `-O3`).

**Hints**:

- Locking your mutex will likely require a loop of some sort.
- It may be helpful to `assert` your expectations about the state of the mutex within your lock/unlock functions.
- _Optional_: If you want to improve the performance of your mutex, take a look at the `sched_yield` function.

## Task 3

In this task you'll simulate a simple dice rolling game.
Use only _pthread barriers_ for synchronization.

The game starts with a fixed number of players (5).
Each round all of the players roll a die (random number between 1 and 6).
The player(s) with the smallest roll are eliminated from the game.
If all remaining players roll the same number, all of them lose.
The game continues until either one player wins, or all players have been eliminated.

- The main thread only does setup and cleanup.
- Each player is simulated by one dedicated thread.
- One of the players (not the main thread) determines which player(s) are eliminated.
  Use `pthread_barrier_wait`'s return value for this.

Example output where player 3 wins the game:

```
Player 0 rolled a 4
Player 1 rolled a 1
Player 2 rolled a 5
Player 3 rolled a 3
Player 4 rolled a 2
Eliminating player 1
---------------------
Player 0 rolled a 4
Player 2 rolled a 3
Player 3 rolled a 5
Player 4 rolled a 2
Eliminating player 4
---------------------
Player 0 rolled a 5
Player 2 rolled a 1
Player 3 rolled a 6
Eliminating player 2
---------------------
Player 0 rolled a 1
Player 3 rolled a 3
Eliminating player 0
---------------------
Player 3 has won the game!
```

Example output where all players lose the game:

```
Player 0 rolled a 4
Player 1 rolled a 4
Player 2 rolled a 3
Player 3 rolled a 1
Player 4 rolled a 4
Eliminating player 3
---------------------
Player 0 rolled a 1
Player 1 rolled a 4
Player 2 rolled a 5
Player 4 rolled a 5
Eliminating player 0
---------------------
Player 1 rolled a 3
Player 2 rolled a 3
Player 4 rolled a 2
Eliminating player 4
---------------------
Player 1 rolled a 1
Player 2 rolled a 1
Eliminating player 1
Eliminating player 2
---------------------
All players were eliminated!
```

---

```
exc07_csXXXXXX.zip
├── task1/
│   ├── Makefile
│   ├── solution.txt
│   └── task1.c
├── task2/
│   ├── Makefile
│   ├── solution.txt
│   └── task2.c
└── task3/
    ├── Makefile
    └── task3.c
```

- [ ] Read and understand [Exercise Sheet 5 Feedback](../exercise05/feedback.md)
- [ ] Check that your solution conforms to the guidelines given in these feedback documents
- [ ] Run solutions using `valgrind` where applicable
- [ ] Auto-format all source files
- [ ] Check your submission on ZID-GPL
- [ ] Submit zip
- [ ] Submit tally list

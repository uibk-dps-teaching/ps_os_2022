# Exercise Sheet 3

## Task 1

Write a program which creates `N` child processes using `fork(2)`, each computing an estimate of the number PI using a Monte Carlo simulation with `S` random samples.
Both `N` and `S` can be specified on the command-line.

The parent first **creates all children**, then **waits** until they have exited, and finally prints `Done.`.

Every child:

- computes an estimate of PI using `S` random samples; then
- prints a message of the form `Child <i> PID = <PID>. mc_pi(<S>) = <mc_pi(S)>.` where
  `<i>` is the child number,
  `<PID>` is the child's process id,
  `<S>` is the random samples parameter,
  and `<mc_pi(S)>` is the calculation result.

Use the following implementation for the Monte Carlo simulation:

```c
double mc_pi(int64_t S) {
    int64_t in_count = 0;
    for(int64_t i = 0; i < S; ++i) {
        const double x = rand() / (double)RAND_MAX;
        const double y = rand() / (double)RAND_MAX;
        if(x*x + y*y <= 1.f) {
            in_count++;
        }
    }
    return 4 * in_count / (double)S;
}
```

_Hint_: A parent process can wait for its children using `wait(2)` or `waitpid(2)`.

Example output:

```
$ ./task1 3 1000
Child 0 PID = 27877. mc_pi(1000) = 3.192000.
Child 1 PID = 27878. mc_pi(1000) = 3.136000.
Child 2 PID = 27879. mc_pi(1000) = 3.180000.
Done.
```

Experiment with different values for `N` and `S`.
Analyze the obtained output.
Is the order of the messages consistent?
Can the order of these messages be predicted?
What does it depend on?

Notice that the function `mc_pi` uses `rand(3)` to repeatedly generate pseudo-random numbers.
By default this function returns the same sequence of numbers every time.
To get different estimates from each process, _seed_ the random number generator using `srand(getpid())`.
Does it matter whether you do this before or after the call to `fork()`?
Explain your answer.

## Task 2

Programs frequently use _system calls_ to interact with the operating system, for example when spawning a child process or when performing everyday file operations.
It sometimes can be useful to inspect the system calls performed by a program, e.g. to debug its behavior when no source code is available, or simply to find out more about how it works.
Linux systems provide the `strace` utility to do just that.

Alongside this document you will find a binary file called [`secret`](secret), which has been compiled to run on [ZID-GPL](../zid_gpl.md) (while you can also try to run it on your own computer, your mileage may vary).

Begin by reading `man strace` to familiarize yourself with its output format.
Then, use `strace` to investigate the behavior of the provided binary.

## Task 3

Take your `even` program from the previous exercise sheet and write a shell script that:

- executes your `even` program for every argument given to the shell script;
- sums up the even and odd numbers separately; and
- outputs the results

Here is an example how that could look like:

```
$ bash even_odd_sum 9 4 67 2 3 0 5 6 8 -2
sum of even numbers: 18
sum of odd numbers:  84
```

If an argument is not a number (according to `even`), abort the calculation and print a meaningful error.

You might want to look into the shell's `shift` builtin command.
Please include `even.c` and the corresponding `Makefile` in this submission as well.

---

Submit your solution as a zip archive via [OLAT](https://lms.uibk.ac.at), structured as follows.

```
exc03_csXXXXXX.zip
├── task1/
│   ├── Makefile
│   ├── solution.txt
│   └── task1.c
├── task2/
│   └── solution.txt
└── task3/
    ├── Makefile
    ├── even.c
    └── even_odd_sum
```

This time we've provided a script [`check_exc03`](check_exc03) for you to check whether the structure is correct.
Run it after creating the zip:

```
$ bash check_exc03 exc03_csXXXXXX.zip
```

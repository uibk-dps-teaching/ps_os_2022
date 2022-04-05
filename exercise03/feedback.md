# Feedback — Exercise Sheet 3

## General

### Output Parameters

Some C functions take non-const pointers as parameters to _return_ additional information.
Take for example `pid_t wait(int* wstatus)`: it returns the PID of the child process (on success), but also takes an `int*` as argument.
The man page states:

> If wstatus is not NULL, wait() and waitpid() store status information in the int to which it points.

This technique is used because functions in C can only return one value — here we are already returning the PID.
We commonly refer to such parameters as _output parameters_ as their sole purpose is to output additional values by the function.

Furthermore, the _if is not NULL_ part tells us that it is an _optional_ parameter.
If we are not interested in this particular value, we can just provide `NULL` instead of a valid pointer.

Note that output parameters can sometimes be non-optional, in which case you have to provide a valid pointer.
Always read the documentation carefully.

## Task 1

### Parallel Execution 

One of the main advantages of using child processes is the parallel execution of computations.
The function calls `wait(NULL)` and `waitpid(-1, NULL, 0)` are blocking, which means that execution stops until the the function returns (i.e. is finished).
Using only one loop, like in the following code snippet, results in a _sequential_ execution of the child processes.
This means that for every loop iteration: the (parent) process spawns a child process and waits until the child process is finished.
The next child process will be spawned in the following loop iteration.

```c
for (int i = 0; i < processCount; ++i) {
    pid_t pid = fork();
    if (pid < 0) {
        // error handling
    } else if (pid == 0) {
        // child
        exit(0);
    } else {
        // parent
        wait(NULL);
    }
}
```

The correct solution requires you to use two loops, one for starting child process, the other to wait for them to finish.

```c
for (int i = 0; i < processCount; ++i) {
    pid_t pid = fork();
    if (pid < 0) {
        // error handling
    } else if (pid == 0) {
        // child
        exit(0);
    }
}

while (wait(NULL) > 0) {
}
```

# Exercise Sheet 2

From here on, `Makefile`s with `all` (default) and `clean` rules are required.
Your source code must compile by running `make` in the corresponding folder.

## Task 1

Write a program that checks if the _environment variable_ `MY_ENV_VAR` is set.
If the variable is set, the program should tell that the variable is set and show the value.
If the variable is not set, the program should also reflect that:

```
$ ./read_my_env_var
MY_ENV_VAR is not set

...

$ ./read_my_env_var
MY_ENV_VAR is set to 'Hello World'
```

Find a way to set and unset the environment variable in your shell.
Finally, find a way to set the environment variable only for a specific command (in this case `./read_my_env_var`).

You can inspect environment variables on your shell with `printenv` and print them with `echo $MY_ENV_VAR`.
Investigate what the `PATH` environment variable is about and why it is important.

## Task 2

Create a program that registers handler(s) for the signals `SIGINT`, `SIGUSR1`, `SIGTERM`, and `SIGKILL` using `sigaction` (you must **not** use `signal`).
When a signal is received, it outputs a message indicating which signal was received.
After registering the signal handler(s), call `usleep` in a loop to prevent the process from exiting.

Have a look at the following man pages `sigaction(2)`, `signal(7)`, and `signal-safety(7)`.
Read also [SIG30-C](https://wiki.sei.cmu.edu/confluence/display/c/SIG30-C.+Call+only+asynchronous-safe+functions+within+signal+handlers) and [SIG31-C](https://wiki.sei.cmu.edu/confluence/display/c/SIG31-C.+Do+not+access+shared+objects+in+signal+handlers).

You can use the shell commands `ps` and `pgrep` to find the process ID (PID) of a process.
`kill` can be used to send a signal to a process.

What difference do you notice between `SIGTERM` and `SIGKILL`?

What happens when you press `CTRL + c` while the program is running?

*Note:* In order to use `sigaction` and `usleep` you have to define `_POSIX_C_SOURCE` and `_BSD_SOURCE` feature test macros.
This can be done by adding the compile flag `-D_POSIX_C_SOURCE -D_BSD_SOURCE`.
See the `feature_test_macros(7)` man page for more information.
On modern systems the `_BSD_SOURCE` feature test macro has been deprecated and replaced by `_DEFAULT_SOURCE`, however this is not available on `zid-gpl`.
You may ignore the deprecation warning that is emitted when using `_BSD_SOURCE`, however your program should not generate any other warnings.

## Task 3

Create a program `even.c` where the main function:

- returns 0 if the first argument (`argv[1]`) is an even number
- returns 1 if the first argument is an odd number
- returns 2 if no argument (or more than one) was provided
- returns 3 if the first argument is not a number

How can you retrieve this _exit code_ in your shell after running the program?

In your shell `;`, `&&`, and `||` can be used to execute multiple commands on a single line.
What are the differences between these 3 operators?

Have a look at the two executables `true` and `false` located in `/usr/bin`.
There are dedicated man pages available.
Note that your shell also has _builtins_ for `true` and `false`.

`type true` tells you that `true` is a shell builtin.
`which true` shows you the path to the executable.
How can you run the executable instead of the shell builtin?

---

Submit your solution as a zip archive via [OLAT](https://lms.uibk.ac.at), structured as follows.

`solution.txt` contain the answers to the questions above.
Your answers have to be written in English and the file must be plaintext.

```
exc02_csXXXXXX.zip
├── task1/
│   ├── Makefile
│   ├── read_my_env_var.c
│   └── solution.txt
├── task2/
│   ├── Makefile
│   ├── signal_handler.c
│   └── solution.txt
└── task3/
    ├── Makefile
    ├── even.c
    └── solution.txt
```

**Do not forget to mark the exercises solved after uploading your solution!**

![Tally List](images/tally_list.png)

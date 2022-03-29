# Exercise Sheet 4

## Task 1

When you list the contents of a directory with details you can see the permissions, owning user, and owning group among other details.
For example:

```
$ ls -l
drwxrwxr-x. 2 c7031162 lxusers 4096 Mar 11 19:11 game_of_life
-rw-rw-r--. 1 c7031162 lxusers 1088 Mar 11 00:07 task_2_summary.txt
-rw-rw-r--. 1 c7031162 lxusers   61 Mar 11 00:07 task_3_command.txt

 ↑            ↑        ↑
 Permissions  User     Group
```

- Explain each of the permissions read, write, and execute for both files and directories
- Explain what role the owning user and owning group plays
- Explain how to read these permission strings (e.g. `rw-rw-r--`)
- How do these permission strings relate to octal permissions (e.g. `0644`)?
- How can you change the permissions of a file?
- How can you change the permissions of all files within a directory?
- What is the Bash builtin `umask` about?
- The same permission mechanism is also used for other Unix operating system components (e.g. see the second parameter of `mkfifo(3)`).
  Assuming a big multi-user system, like ZID-GPL, which permissions should you use for an IPC mechanism, like a named pipe, to prevent other users from interfering with your task?
  Looking at named pipes specifically, what else can you do, apart from setting permissions, to prevent conflicts with other users?

_Note:_ You don't have to cover advanced features like _setuid_, _setgid_, sticky bit, or ACLs.

## Task 2

On the shell you can provide the output of command as input to another using a _pipe_ `|`.

```
$ ls | wc -l
8
```

This _forwards_ the standard output of `ls` to the standard input of `wc` (word count).
The result you see is the output of `wc -l`, the number of lines.
Multiple commands can be chained together to build longer _pipelines_.

```
$ ls | grep -v lab | wc -l
6
```

Your task is to create a C program `ls_wc` that uses fork and exec to run the two programs `ls` and `wc` (with the argument `-l`) like just shown.
Have a look at `fork(2)`, `exec(3)`, `pipe(7)`, `pipe(2)`, and `dup(2)`/`dup2(2)` man pages.

Then, create a program `ls_grep_wc` that mimics the second example.
Your Makefile should build both executables.

There are even more helpful operators built into your shell.
Explain what the following operators do: `<`, `>`, `2>`, `2>&1`

## Task 3

In this task you will work with FIFOs, also called **named** pipes.

The goal is to write a simple "math server", where multiple interactive _clients_ can send math expressions to a _server_, which will then evaluate them and print the result to the console.
Your solution should produce two separate executables, `task3_client` and `task3_server`.
There is no need to use `fork()` in this task.

A single **server** can be started with a list of clients that may connect to it, like so: `./task3_server euler gauss`.
In this case, the two clients `euler` and `gauss` can connect, however it should be possible to specify an arbitrary number of clients.

The server then creates a FIFO for each client.
Think about a way of associating each FIFO with a specific client.
**NOTE**: If you have problems creating FIFOs in a certain location, make sure that your filesystem actually supports them (this can be a problem e.g. in WSL, when working on an NTFS drive).
A good location to store FIFOs is somewhere inside `/tmp` (when working on ZID-GPL, be sure to avoid file naming collisions with other students!).

The server then waits for clients to connect by opening each FIFO for reading.
You can assume that clients connect in the same order they are specified on the command line.
Once a client connects, the server prints the message `"<client name> connected"`.

A **client** is started by also providing it with a name, for example `./task3_client euler`.
The client will then attempt to connect to the server by opening one of the FIFOs for writing.
Once connected, the client continuously prompts for a math expression to send.
Each expression is then written to the FIFO.
You can assume a math expression to be at most `PIPE_BUF` long (why is this important?).
If the expression is empty, the client disconnects by closing the FIFO and exiting.

Meanwhile, the server continuously monitors all FIFOs for incoming math expressions using `poll()` (see `man poll`).
Once an expression is received, it is evaluated and printed to the console like so `"<client name>: <expression> = <result>."`.
The server only needs to support very simple math expressions of the form `<a> <op> <b>` where `a` and `b` are real numbers and `op` is one of `+-*/`.
If an expression does not conform to the expected format, the server instead prints `"<client name>: <expression> is malformed."`.
If a client disconnects, the server prints the message `"<client name> disconnected."`.
Once all clients are disconnected, the server cleans up the FIFOs and exits.

Here's what an example session could look like (each line represents a different point in time, `>` indicates user input).

```
TERMINAL 1                      TERMINAL 2                  TERMINAL 3

> ./task3_server euler gauss
                                > ./task3_client euler
euler connected.                Expression:
                                                            > ./task3_client gauss
gauss connected.                                            Expression:
                                > 1.359 * 2
euler: 1.359 * 2 = 2.718        Expression:
                                                            > 1.139 - 0.304
gauss: 1.139 - 0.304 = 0.835                                Expression:
                                > 2 * i
euler: 2 * i is malformed.      Expression:
                                > (empty)
euler disconnected.
```

Additional notes and hints:

- You can assume that all clients are connected before handling their messages.
- Your server doesn't need to support clients re-connecting, i.e., they can (and must) connect exactly once.
- Your solution **must** use `poll()`, NOT `select()`.
- Make sure to use appropriate file access modes (which we discussed in connection to `chmod`) to create and open your FIFOs.
  Justify your choice.
- Your FIFOs should be **blocking**, i.e., you must NOT use `O_NONBLOCK` in your calls to `open()`.
- You can use the `%g` format specifier to let `printf` decide on the number of significant digits to print.
- As always, make sure to properly release all allocated resources (e.g. FIFOs) upon exiting.
- If you want to create multiple processes side by side for testing, there are several different options available to you:
  1. Open multiple terminal windows / tabs
  2. Use shell jobs to switch between different processes (`&`, `fg`, `bg`,
     `jobs` and so on)
  3. Use a terminal multiplexer such as `tmux` or `screen`

---

```
exc04_csXXXXXX.zip
├── task1/
│   └── solution.txt
├── task2/
│   ├── Makefile
│   ├── ls_grep_wc.c
│   ├── ls_wc.c
│   └── solution.txt
└── task3/
    ├── Makefile
    ├── task3_client.c
    ├── task3_server.c
```

- [ ] Read and understand [Exercise Sheet 1 Feedback](../exercise01/feedback/README.md)
- [ ] Read and understand [Exercise Sheet 2 Feedback](../exercise02/feedback.md)
- [ ] Check that your solution conforms to the guidelines given in these feedback documents
- [ ] Run solutions using `valgrind` where applicable
- [ ] Auto-format all source files
- [ ] Check your zip archive using [`check_exc04`](check_exc04)¹
- [ ] Check your submission on ZID-GPL
- [ ] Submit zip
- [ ] Submit tally list

¹ If `check_exc04` reports that your zip is not correct, run it on ZID-GPL.
There have been issues with different implementations of `zipinfo` and `sort`.
Let this also be a lesson for how fragile shell scripts are and that a proper scripting language (e.g. Python or Ruby) is often a better choice.

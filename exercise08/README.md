# Exercise Sheet 8

## Task 1

### Task 1a

Create a (non-global) variable `counter` of type `atomic_int` and initialize it to `50000`.
Now, spawn 50000 POSIX threads.
Each thread decrements the variable by `1` and then exits.
The _main thread_ waits for all threads to finish and prints the final value of `counter`.

**Notes:**

- Make sure to check the return values of `pthread_create` and `pthread_join`.
- It might happen that you cannot spawn `50000` threads on your system, in which case reduce the number of threads to something that works.

### Task 1b

We now want to repeat the same calculation without the overhead of creating 50000 separate threads.
To do this, implement a _thread pool_, which contains a limited number of _worker threads_ that remain alive for the duration of the pool, while processing _jobs_ submitted to a queue (in a first-in-first-out manner).

Your thread pool should provide the following functions:

- The `void pool_create(thread_pool* pool, size_t size)` function initializes a `thread_pool` by starting `size` worker threads.
  Each thread checks for submitted jobs and runs them.
- The `job_id pool_submit(thread_pool* pool, job_function start_routine, job_arg arg)` submits a job to the thread pool and returns a `job_id`.
- The `void pool_await(job_id id)` function waits for the job with the given `job_id` to finish.
- The `void pool_destroy(thread_pool* pool)` shuts down the thread pool and frees all associated resources.
  Worker threads finish the currently running job (if any) and then stop gracefully.

The `job_function` and `job_arg` types are defined as follows, similar to the arguments for `pthread_create`:

```c
typedef void* (*job_function)(void*);
typedef void* job_arg;
```

You may choose the implementation of the `thread_pool` and `job_id` types as you see fit.

Now implement the same `counter` variable as in Task 1a.
Then, create a thread pool and submit 50000 jobs.
Each job again decrements the `counter` by one.
The _main thread_ then waits for all jobs to finish, cleans up the thread pool and prints the final value of `counter`.

**Hints:**

- Note that not all submitted jobs can start immediately.
  You can adjust the `myqueue` implementation from previous exercises to store submitted jobs; you will need to store at least `job_function`, `job_arg` and possibly `job_id` in the queue entry.
  Worker threads can then pop jobs from the queue and execute them.
- You can assume a `job_function` to always returns `NULL`, i.e. the return value is never used.

How does a thread pool relate to the producer-consumer pattern?
Measure the execution time of both using `/usr/bin/time -v` and report your findings.
Make sure to use an optimized build (`-O2` or `-O3`).

## Task 2

In this task we will implement a simple "echo server" using TCP sockets.

Create a program, the server, that receives as its single argument a TCP port to listen on.
After startup, the server opens a TCP socket (see below), prints `"Listening on port <port>."` and continuously listens for incoming connections.
Upon a client connecting, the server attempts to read incoming messages from the client and responds with `"Echo: <original message>"`, either until the client disconnects or the shutdown command (see below) is received.

For this task, you don't need to create a separate client application.
Instead, you can use the `telnet` utility to establish a TCP connection to your server.
The command `telnet localhost <port>` will open a prompt where you can send messages to your server and see its response.
You can exit `telnet` by pressing `CTRL+]` or `CTRL+5`, and then entering `close` in the prompt.

The server should be able to handle multiple clients connecting, but only one at at time (i.e., the current connection has to be closed before a new one can be accepted).

Upon receiving the message `/shutdown`, the server should print `"Shutting down."`, stop accepting new connections, clean up and exit.

Begin by skimming through `man 7 ip`. Here is a quick TLDR:

1. To create a TCP/IP socket, use the `socket()` function with `domain = AF_INET` and `type = SOCK_STREAM`.
2. Next, fill out the `sockaddr_in` struct to inform the socket where to bind to, like so:

   ```c
   struct sockaddr_in addr;
   memset(&addr, 0, sizeof(struct sockaddr_in));
   addr.sin_family = AF_INET;
   addr.sin_port = htons(port);
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   ```

3. Next, `bind()` the socket to the address, and `listen()` for incoming connections.
4. Use `accept()` to accept incoming connections, and to receive a unique communication socket for each client.

**Additional notes & hints:**

- The `dprintf` function may be useful for writing to a socket.
- Make sure to properly `close()` all sockets before exiting.
- `bind()` returning `EADDRINUSE` ("Address already in use"), may indicate that you did not properly close a socket.
  You can use a different port or wait a few seconds to circumvent this.
  Ultimately you should however try to find the root cause of the problem.

## Task 3

In this task we will turn the simple "echo server" from the previous task into a basic _HTTP web server_.

- The server again receives as a single command line argument the port it should listen on.
- After creating and binding the socket as in the previous task, the server should spawn a _listener thread_ for accepting incoming connections.
  - The listener thread should terminate once the `/shutdown` URL has been requested (see below).
    A good way of implementing this is to use the `pthread_cancel()` function.
- For each client that connects, a _connection handler function_ is executed:
  - Each connection handler function should be run in your thread pool from task 1.
  - To simulate a real workload, the handler begins by sleeping for 100 milliseconds.
  - The handler then "parses" the incoming HTTP request and responds accordingly (see below).
- The server waits until the listener thread and all connection handler functions have terminated, cleans up and exits.

If everything works correctly, you should be able to navigate your browser to `http://localhost:<port>` and see your web server's response.

### The HTTP Protocol

To properly communicate with a web browser, the web server needs to be able to "speak" the _HTTP protocol_, or at least a very simple subset thereof.
Most HTTP messages (and all that we care about here) consist of at most two parts, a _header_ and optionally a _message body_.
The _header_ contains meta-information about the current request or response.
It consists of multiple lines of plain text, separated by a newline.
Importantly, the HTTP protocol uses CRLF newlines (`\r\n`) instead of just LF (`\n`).
A final empty newline marks the end of the header section.

A typical incoming request header will look something like this:

```
GET /foo HTTP/1.1\r\n
Host: localhost:<port>\r\n
Accept: */*\r\n
\r\n
```

You do not need to handle any of this, except for the first line.
The `GET /foo` refers to the URL that is being requested from the server (in this case corresponding to a request to `http://localhost:<port>/foo`).
All URLs can be ignored, except if a client requests `GET /shutdown`, which should then cause the server to shut down as in the previous task.

To respond to a client, the server needs to write both a header and a message body.
A typical response will look something like this:

```
HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n
Content-Length: <number of bytes in message body, including newlines>\r\n
\r\n
<message body>
```

You may include a small HTML message body of your own choosing.
Be sure to set the correct size in the `Content-Length` header field.

**Additional notes & hints:**

- Use `curl -v localhost:<port>` to see the request and response headers sent to and from your server.
  Curl will complain if the response is malformed.
- For more information on the HTTP protocol, you may browse through [RFC 7230](https://datatracker.ietf.org/doc/html/rfc7230).
- When working on ZID-GPL, you can still connect to your web server with your local web browser by creating an _SSH tunnel_.
  Simply execute the following command in a shell: `ssh csXXXX@zid-gpl.uibk.ac.at -N -L <port>:localhost:<port>` where `<port>` refers to the port your server is listening on (make sure to choose a unique port to avoid collisions with other students).

---

```
exc08_csXXXXXX.zip
├── task1/
│   ├── Makefile
│   ├── solution.txt
│   ├── task1.c
│   └── [*.h]
├── task2/
│   ├── Makefile
│   ├── task2.c
│   └── [*.h]
└── task3/
    ├── Makefile
    ├── task3.c
    └── [*.h]
```

- [ ] Read and understand [Exercise Sheet 5 Feedback](../exercise05/feedback.md)
- [ ] Read and understand [Exercise Sheet 6 Feedback](../exercise06/feedback.md)
- [ ] Check that your solution conforms to the guidelines given in these feedback documents
- [ ] Run solutions using `valgrind` where applicable
- [ ] Auto-format all source files
- [ ] Check your submission on ZID-GPL
- [ ] Submit zip
- [ ] Submit tally list

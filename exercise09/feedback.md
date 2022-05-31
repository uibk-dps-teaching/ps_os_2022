# Feedback — Exercise Sheet 9

## Streaming Semantics

This is actually not a _new_ concept, but something that should've been explained earlier in the lecture (or another course).

In this course we do a lot of IO operations (i.e. sending and receiving data).
We use the mechanisms provided by the operating system to read from files, pipes, sockets, etc.
All of these mechanisms work by reading and writing _bytes_ as they have to be as generic as possible.
It follows that these mechanisms do not impose a specific _format_ (i.e. protocol) that we have to adhere to.
We can, should, and often must put our own rules in place when working with _streams of bytes_.

More specifically: when we want to exchange _messages_ between two components, these components need to know where one message ends and another beings.
Most students assumed that `read(2)` and `write(2)` takes care of this automatically — but that's not the case.
_We_ have to define a format in order to send and receive messages correctly across a the stream.

Before looking at 3 practical formats let's revisit the `read(2)` function.

> `ssize_t read(int fd, void *buf, size_t count);`
>
> `read()` attempts to read up to `count` bytes from file descriptor `fd` into the buffer starting at `buf`.
>
> […]
>
> On success, the number of bytes read is returned […]
> On error, -1 is returned […]

Note the _attempts_ above.
Even if we call `read(fd, buf, 128)` we are not guaranteed to read exactly 128 bytes — there might be fewer.
The return value tells us how many bytes were _actually_ read.
The same is true, more or less, for `write(2)`; however, to keep this short, we just focus on the reading side.

Now, to the formats hinted above:

1.  **Fixed-length**: all messages have the same length — always!
    If a message is shorter, it needs to be _padded_ (i.e. extended to meet the required length).
    If a message is longer, it cannot be sent (at least not completely).
    Both of these rules also highlight the drawbacks of this naive approach.

    To read a message, you'd use a fixed-size buffer that is exactly the length of one message and then call `read(2)` repeatedly until you have filled that buffer completely (or an error occurred).

    ```c
    char buffer[MESSAGE_LENGTH] = {0};
    ssize_t bytes_read = 0;
    while (bytes_read < MESSAGE_LENGTH) {
        ssize_t n = read(fd, buffer + bytes_read, MESSAGE_LENGTH - bytes_read);
        if (n == -1) {
            // error
        }
        bytes_read += n;
    }
    ```

2.  **Terminator**: a specific byte (or sequence of bytes) is transmitted after the message to indicate the message's end.
    This byte must not be present in the message itself.

    To read a message, you _could_ read from a source byte by byte, checking whether that byte is the _terminator_.
    You continue reading until the terminator is encountered.
    If the buffer is exhausted, you need to grow it as the message isn't complete yet.

    Since reading byte by byte will likely hinder performance, one'd commonly read into a small buffer and then go through that buffer to check whether it contains the terminator.
    However, when the terminator is found, there might also be bytes after it.
    Bytes that you've already read since we are no longer reading byte by byte.
    These bytes belong to the next message and must not be discarded.
    There is usually no way to put bytes back into a source, so you have to store them somewhere and take them into account when reading the next message.

    Depending on the programming language, the standard library often has a form of _buffered reader_ that encapsulates this behavior.
    For instance [Rust's `BufReader`](https://doc.rust-lang.org/std/io/struct.BufReader.html), [Go's `bufio`](https://pkg.go.dev/bufio), [Java's `BufferedReader`](https://docs.oracle.com/javase/8/docs/api/java/io/BufferedReader.html).

3.  **Length prefixed**: before sending a message, you send the length of the message.
    For sending the length you define a fixed number of bytes (typically 4).
    This also imposes a hard, upper limit on the message length — although a relatively high one.

    First you read 4 byte and interpret this as length of the message that follows.
    Using a buffer that is big enough to hold the message, you read until _length_ bytes have been read.

# Feedback — Exercise Sheet 2

## General

### Always initialize variables

The following code does **not** correctly initialize a local variable.

```c
struct sigaction sa;
sa.sa_handler = handler;
```

`sigaction` contains other fields besides `sa_handler` that remain uninitialized even after the second line.
Use _designated initializers_ when initializing structs as they will initialize fields that are not listed to zero.

```c
struct sigaction sa = {
    .sa_handler = handler,
};

// Additionally initialize sa_mask, see sigaction(2) and sigsetops(3).
sigemptyset(&sa.sa_mask);
```

### Makefiles should not run executables automatically

Some students added a command to directly execute the program right after compilation.
Program execution should not happen automatically!
You can add a dedicated _run target_ to the Makefile if you want, but keep it separate from the build target rules.
`all` should not depend on _run targets_, only build the project.

Also, `clean` is the **only** rule that deletes files.

### Do not disregard documentation referred to by the assignment

Especially with the signal handler task, a lot of people did not read/understand the provided references, specifically [SIG30-C] and [SIG31-C].
By ignoring the referenced material, your solution will likely be incorrect.
If the material is unclear, ask in the tutorium Discord for help.

[SIG30-C]: https://wiki.sei.cmu.edu/confluence/display/c/SIG30-C.+Call+only+asynchronous-safe+functions+within+signal+handlers
[SIG31-C]: https://wiki.sei.cmu.edu/confluence/display/c/SIG31-C.+Do+not+access+shared+objects+in+signal+handlers

### Do not blindly copy code from the internet

Some students copied code into their program without carefully examining it.
Here is an example:

```c
int isNumber(char number[]) {
    int chars = 0;
    for (int i = 0; number[i] != 0; i++) {
        if (number[0] == '-') {
            i++;
        }
        if (!isdigit(number[i])) {
            chars++;
        }
    }
    return chars;
}
```

Calling this function with the argument `"-"` will result in an out-of-bounds access when the for-loop's condition is checked on the second iteration.
The implementation is therefore incorrect.

Furthermore, the function is named inappropriately.
The `is` prefix suggests a boolean result; the given input can be either *a number* (`true`) or *not a number* (`false`).
Instead, the function returns some character count.

### Read the documentation of the functions you are using

Many students implemented a `isNumber` function just to check whether a given string is a valid number.
They then call `atoi` on the string to obtain that number.

Taking a look at the man page of `atoi`, it states:

> The `atoi()` function converts the initial portion of the string pointed to by `nptr` to `int`.
> The behavior is the same as
>
>     strtol(nptr, NULL, 10);
>
> except that `atoi()` does not detect errors.

This paragraph tells you that there is a more sophisticated function `strtol` that, contrary to `atoi`, detects whether the string to integer conversion was successful.
Using `strtol` eliminates the need for an `isNumber` function.

### You have to understand the code you submit

It doesn't matter whether your code was copied from the internet (including citation), from a group member, or directly from the documentation.
When you submit your solution, you must be able to explain every single line.
If you can't explain it, don't include it.

This does not mean that you have to know all the flags and fields by heart.
Opening the man page is just a few button presses away.

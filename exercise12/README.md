# Exercise Sheet 12

The following is a _real_ exam from a previous year.
It should give you a general idea of what to expect from this year's exam.

This exercise sheet is optional, and provides up to 3 **bonus points**:
These points are not included in the total number of points reachable this semester (33), but will count towards your personal final score (theoretically, it is thus possible to achieve 36/33 points).

While the exam itself is not split into explicit tasks, we define the following tasks for receiving the bonus points:

- **Task 1:** Implement basic food ordering / preparation / pickup logic.
- **Task 2:** Implement notification app.
- **Task 3:** Provide a fully functional solution that compiles without warnings, executes correctly producing only the expected output and finishes cleanly without leaking any resources.

**As usual, please mark your completed tasks in OLAT**.

**NOTE**: We recommend that you attempt to solve this exercise sheet by simulating the real exam scenario:
Prepare any materials you may need, such as your solutions to previous exercises, for quick access (the real exam will be open book!).
Take an hour and 45 minutes and only then continue reading the rest of this sheet.
Try to solve it in time.

---

# Exam

## General Information

**Please read this description carefully.**

Talking, chatting, texting, or any other form of a live human to human communication is not allowed and may lead to a negative grade.
However, you are allowed to use all other offline and online resources such as previous homework solutions, manpages, StackOverflow, etc.

If you directly copy code from one of these sources, please add a **citation** as a comment before the code fragment.
I.e.

```C
// The following code is from my solution to exercise 5.2
```

or

```C
// Source: https://stackoverflow.com/questions/14888027/mutex-lock-threads
```

If we find the same code in multiple solutions without citation, we have to assume cheating, and you will receive a failing grade.
Note that it does not matter whether you are the person copying from, or providing your solution to another -- both will receive a failing grade.

Your program **must compile** on ZID-GPL (or ZID-Desk) and include a Makefile.
If something does not work as expected, provide comments in the source code explaining what you think the problem is.
It might improve your grade.

**You have 1 hour and 45 minutes to complete this exercise.**

**Please make sure to upload your solution to OLAT in time.**

### General Hints

- Don't forget you can load a newer version of GCC on ZID-GPL using e.g. `module load gcc/9.2.0`.
- Remember that you can use `valgrind` to check your program for memory leaks.
- Try to develop your program incrementally, and focus on the core functionality first.

## Task: Fast Food Restaurant

A fast food restaurant employs multiple cooks to prepare meal orders for guests.
Upon entering the restaurant, guests make their order and then go to a table to wait until they can pick up their meal from the counter.
Meanwhile, cooks receive orders _in-order_, prepare meals and then place them on the counter for pickup.

- Each cook can prepare one meal at a time.
  To prepare a meal, a random amount of time in the interval `[100, 500]` **milliseconds** is required.
- The counter is quite small, so only a single meal can be placed on it at a time.
  A cook that has finished a meal has to wait for the counter to become free before they can continue cooking the next meal.
- Guests are extremely impatient, and walk to the counter to check whether their order is complete every **100 milliseconds**.
  Once at the counter, it takes them another **100 milliseconds** to determine whether the meal is theirs.

To reduce contention around the counter, the restaurant wants to introduce an app that notifies guests once their order is ready for pickup.
This way, guests can remain at their tables (and maybe take a nap) instead of having to constantly check the counter.

Once all guests have been served, the cooks are informed that they can go home, and the restaurant closes for the day.

--

Model each guest and cook as a separate thread.
Use a queue to keep track of placed orders (you may want to use the implementation provided for some of the exercises during the semester).

- Keep track of guests, cooks and orders using numeric ids.
- Cooks should print a message when they start or finish an order, and when they have placed it on the counter.
- Guests should print a message when they made an order and when they pick it up.
  Additionally, print the total time a guest waited for their meal from order to pickup.
- Before exiting, print the average wait time for all guests.
- Use _condition variables_ to simulate the notification app.

Your solution should produce a single executable called `restaurant`, which can be called like so:
`./restaurant <enable notifications> <number of guests> <number of cooks>`.
The first parameter can either be `0` or `1` and indicates whether guests are notified when their order is complete (as opposed to having to check manually).

**Example output:**

```
> ./restaurant 0 5 3
Guest 1 has made meal order 0
Guest 3 has made meal order 3
Cook 0 is preparing order 0
Cook 2 is preparing order 3
Guest 0 has made meal order 1
Guest 2 has made meal order 2
Guest 4 has made meal order 4
Cook 1 is preparing order 2
Cook 1 has finished order 2
Cook 0 has finished order 0
Cook 2 has finished order 3
Cook 0 has placed order 0 on counter
Cook 0 is preparing order 1
Guest 1 has picked up order 0 after 1101 ms
Cook 2 has placed order 3 on counter
Cook 2 is preparing order 4
Cook 0 has finished order 1
Guest 3 has picked up order 3 after 1503 ms
Cook 0 has placed order 1 on counter
Cook 2 has finished order 4
Guest 0 has picked up order 1 after 1805 ms
Cook 1 has placed order 2 on counter
Guest 2 has picked up order 2 after 2104 ms
Cook 2 has placed order 4 on counter
Guest 4 has picked up order 4 after 2204 ms
All guests have been served with an average wait time of 1743 ms
```

Try to match the formatting of output messages as closely as possible.

**Additional Notes:**

- To simulate cooking times, make sure to generate a unique random seed for each cook.
- Use `usleep(ms * 1000)` to sleep for `ms` milliseconds.
- To measure time in milliseconds, you can use `clock_gettime` like so:
  ```c
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  uint64_t ms = ts.tv_sec * 1e3 + ts.tv_nsec / 1e6;
  ```
- As always, ensure proper synchronization and cleanup.

# Feedback — Exercise Sheet 6

## Condition Variables

### Make sure your implementation does not have race conditions

We have seen several solutions where access to the queue was only protected for the consumer, but not the producer.
This is wrong because the implementation of the queue is not thread-safe!

Consider that while the consumer is checking whether the queue is empty,

```c
pthread_mutex_lock(&mutex);
while (myqueue_is_empty(&queue)) {
    pthread_cond_wait(&cond, &mutex);
}
// ...
pthread_mutex_unlock(&mutex);
```

without locking, the producer may modify the queue (by adding an item) at the same time:

```c
myqueue_push(&queue, 1);
pthread_cond_signal(&cond);
```

Since the `myqueue_push` function is not thread-safe, the producer and the consumer can access the queue both at the same time, resulting in a race condition.

Apart from simply crashing, the above program can even cause a deadlock which may be difficult to identify.

Consider the OS scheduler decides to schedule according to this scheme:

```c
Thread A                                Thread B
pthread_mutex_lock(&mutex);
while (myqueue_is_empty(&queue))
                                        myqueue_push(&queue, 1);
                                        pthread_cond_signal(&cond);
pthread_cond_wait(&cond, &mutex);
```

In this case the signal is sent _after_ the queue was found empty, but _before_ blocking on the condition variable. The signal is thus lost and thread A may wait indefinitely.

**Remember to always carefully identify all locations where shared data is being accessed to avoid race conditions.**

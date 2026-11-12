#include "stack.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef IMPL_NAME
#define IMPL_NAME "?"
#endif

static double now_ms(void)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (double)ts.tv_sec * 1e3 + (double)ts.tv_nsec / 1e6;
}

int main(int argc, char **argv)
{
    long count = (argc > 1) ? atol(argv[1]) : 5000000L;
    Item value;
    Stack *stack;
    Queue *queue;
    double start;
    double stack_time;
    double queue_time;

    stack = stack_create();
    start = now_ms();

    for (long index = 0; index < count; ++index) {
        stack_push(stack, (Item)index);
    }

    while (!stack_is_empty(stack)) {
        stack_pop(stack, &value);
    }

    stack_time = now_ms() - start;
    stack_destroy(stack);

    queue = queue_create();
    start = now_ms();

    for (long index = 0; index < count; ++index) {
        queue_enqueue(queue, (Item)index);
    }

    while (!queue_is_empty(queue)) {
        queue_dequeue(queue, &value);
    }

    queue_time = now_ms() - start;
    queue_destroy(queue);

    printf("%-6s  n=%ld  pile: %8.1f ms   file: %8.1f ms\n",
           IMPL_NAME, count, stack_time, queue_time);
    return EXIT_SUCCESS;
}
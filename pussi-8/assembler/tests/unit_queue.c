#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>

#include "err.h"
#include "queue.h"

TestSuite(Queue);

Test(Queue, QueueInit)
{
    struct queue *queue = queue_create();
    cr_expect(ne(ptr, queue, NULL));
    queue_destroy(queue);
}

Test(Queue, QueueEnqueue)
{
    struct queue *queue = queue_create();
    void *data = malloc(1);
    cr_assert(ne(ptr, queue, NULL));
    cr_expect(eq(int, queue->length, 0));
    cr_expect(eq(int, queue_enqueue(queue, data), SUCCESS));
    cr_expect(eq(int, queue->length, 1));
    queue_destroy(queue);
}

Test(Queue, QueueDequeue)
{
    struct queue *queue = queue_create();
    void *data = malloc(1);
    cr_assert(ne(ptr, queue, NULL));
    cr_expect(eq(int, queue_enqueue(queue, data), SUCCESS));
    cr_expect(eq(ptr, queue_dequeue(queue), data));
    free(data);
    cr_expect(eq(int, queue->length, 0));
    queue_destroy(queue);
}

Test(Queue, QueueDequeueEmpty)
{
    struct queue *queue = queue_create();
    cr_assert(ne(ptr, queue, NULL));
    cr_expect(eq(ptr, queue_dequeue(queue), NULL));
    cr_expect(eq(int, queue->length, 0));
    queue_destroy(queue);
}

Test(Queue, QueueComplex)
{
    struct queue *queue = queue_create();
    void *data = malloc(1);
    cr_assert(ne(ptr, queue, NULL));
    for (int i = 0; i < 10; i++)
        cr_expect(eq(int, queue_enqueue(queue, data), SUCCESS));
    for (int i = 0; i < 10; i++)
        cr_expect(eq(ptr, queue_dequeue(queue), data));
    free(data);

    cr_expect(eq(int, queue->length, 0));
    queue_destroy(queue);
}

Test(Queue, QueueIter)
{
    struct queue *queue = queue_create();
    cr_assert(ne(ptr, queue, NULL));
    for (int i = 0; i < 10; i++)
    {
        int *data = malloc(sizeof(int));
        *data = i;
        cr_assert(eq(int, queue_enqueue(queue, data), SUCCESS));
    }

    char *data = queue_iter_start(queue);
    cr_expect(eq(int, queue->iter_index, 0));
    cr_expect(eq(ptr, queue->iter_last, queue->head));

    int i = 0;
    while (data)
    {
        cr_expect(eq(int, *data, i));
        cr_expect(eq(int, queue->iter_index, i));

        data = queue_iter_next(queue);
        i++;
    }

    cr_expect(eq(int, i, 10));
    cr_expect(eq(ptr, queue->iter_last, NULL));

    queue_destroy(queue);
}

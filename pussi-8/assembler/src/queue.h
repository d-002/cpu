#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

struct queue_node
{
    void *data;
    struct queue_node *next;
};

/**
 * Queue of void pointers.
 * These pointers should be NULL or point to heap-allocated data.
 * The data will be deallocated when the queue is destroyed.
 */
struct queue
{
    struct queue_node *head;
    struct queue_node *tail;

    size_t length;
    size_t iter_index;
    struct queue_node *iter_last;
};

struct queue *queue_create(void);
int queue_enqueue(struct queue *queue, void *data);
void *queue_dequeue(struct queue *queue);
void *queue_iter_start(struct queue *queue);
void *queue_iter_next(struct queue *queue);
void queue_destroy(struct queue *queue);

#endif /* ! QUEUE_H */

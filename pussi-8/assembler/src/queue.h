#ifndef QUEUE_H
#define QUEUE_H

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
};

struct queue *queue_create(void);
int queue_enqueue(struct queue *queue, void *data);
void *queue_dequeue(struct queue *queue);
void queue_destroy(struct queue *queue);
int isempty(struct queue *queue);

#endif /* ! QUEUE_H */

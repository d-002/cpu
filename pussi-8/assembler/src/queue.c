#include "queue.h"

#include <stdlib.h>

#include "err.h"

struct queue *queue_create(void)
{
    struct queue *queue = malloc(sizeof(struct queue));
    if (queue == NULL)
        return NULL;

    queue->head = NULL;
    queue->tail = NULL;
    queue->length = 0;
    queue->iter_index = 0;
    queue->iter_last = NULL;

    return queue;
}

int queue_enqueue(struct queue *queue, void *data)
{
    struct queue_node *node = malloc(sizeof(struct queue_node));
    if (node == NULL)
        return ALLOC_ERROR;

    node->data = data;
    node->next = NULL;

    if (queue->tail == NULL)
        queue->head = node;
    else
        queue->tail->next = node;

    queue->tail = node;
    queue->length++;

    return SUCCESS;
}

void *queue_dequeue(struct queue *queue)
{
    if (queue->head == NULL)
        return NULL;

    struct queue_node *node = queue->head;
    queue->head = node->next;
    if (node == queue->tail)
        queue->tail = NULL;

    void *res = node->data;
    free(node);

    queue->length--;
    if (queue->iter_index)
        queue->iter_index--;
    else
        queue->iter_last = NULL;
    return res;
}

void *queue_iter_start(struct queue *queue)
{
    queue->iter_index = 0;
    queue->iter_last = queue->head;

    return queue->head == NULL ? NULL : queue->iter_last->data;
}

void *queue_iter_next(struct queue *queue)
{
    if (queue->iter_last == NULL)
        return NULL;

    queue->iter_index++;
    queue->iter_last = queue->iter_last->next;

    return queue->iter_last == NULL ? NULL : queue->iter_last->data;
}

void queue_update_at_iter_last(struct queue *queue, void *data)
{
    if (queue->iter_last == NULL)
        return; // out of bounds, I'll decide to do nothing here because yes

    queue->iter_last->data = data;
}

void queue_destroy(struct queue *queue)
{
    if (queue == NULL)
        return;

    struct queue_node *node = queue->head;
    while (node)
    {
        struct queue_node *next = node->next;
        free(node->data);
        free(node);
        node = next;
    }

    free(queue);
}

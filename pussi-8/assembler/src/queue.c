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

    return queue;
}

int queue_enqueue(struct queue *queue, void *data)
{
    struct queue_node *node = malloc(sizeof(struct queue_node));
    if (node == NULL)
        return ALLOC_ERROR;

    node->data = data;
    node->next = queue->head;

    queue->head = node;
    if (queue->tail == NULL)
        queue->tail = node;

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
    return res;
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

int isempty(struct queue *queue) {
    return queue->head == NULL;
}

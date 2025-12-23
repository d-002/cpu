#include "hash_map.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"

size_t hash(const char *key)
{
    if (!key)
        return 0;

    uint32_t hash = 2166136261;
    uint32_t prime = 16777619;

    while (*key)
    {
        hash ^= *key;
        hash *= prime;
        key++;
    }

    return hash;
}

struct hash_map *hash_map_create(free_func free_func)
{
    struct hash_map *hash_map = malloc(sizeof(struct hash_map));
    if (hash_map == NULL)
        return NULL;

    // either 0 for no error, or the index of the arr element that caused the
    // error
    size_t err = 0;

    for (size_t i = 0; i < HASH_MAP_SIZE; i++)
    {
        struct queue *ptr = queue_create();
        if (ptr == NULL)
        {
            err = i;
            break;
        }

        hash_map->arr[i] = ptr;
    }

    if (err)
    {
        for (size_t i = 0; i < err; i++)
            queue_destroy(hash_map->arr[i]);
        free(hash_map);
        return NULL;
    }

    hash_map->iter_index = 0;
    hash_map->iter_starting = 0;
    hash_map->free_func = free_func;

    return hash_map;
}

struct pair *get_in_queue(struct queue *queue, char *key)
{
    for (struct queue_node *node = queue->head; node; node = node->next)
    {
        struct pair *elt = node->data;
        if (strcmp(key, elt->key) == 0)
            return elt;
    }

    return NULL;
}

int hash_map_insert(struct hash_map *hash_map, struct pair pair)
{
    size_t h = hash(pair.key) % HASH_MAP_SIZE;
    struct queue *queue = hash_map->arr[h];

    struct pair *found = get_in_queue(queue, pair.key);

    if (found != NULL)
        return HASH_MAP_DUPE_ERROR;

    struct pair *alloc_ed = malloc(sizeof(struct pair));
    if (alloc_ed == NULL)
        return ALLOC_ERROR;

    alloc_ed->key = pair.key;
    alloc_ed->value = pair.value;

    if (queue_enqueue(queue, alloc_ed))
    {
        free(alloc_ed);
        return ALLOC_ERROR;
    }

    return SUCCESS;
}

void *hash_map_get(struct hash_map *hash_map, char *key)
{
    size_t h = hash(key) % HASH_MAP_SIZE;
    struct queue *queue = hash_map->arr[h];

    struct pair *pair = get_in_queue(queue, key);
    if (pair == NULL)
        return NULL;
    return pair->value;
}

int hash_map_update(struct hash_map *hash_map, struct pair pair)
{
    size_t h = hash(pair.key) % HASH_MAP_SIZE;
    struct queue *queue = hash_map->arr[h];

    struct pair *found = get_in_queue(queue, pair.key);

    if (found == NULL)
        return HASH_MAP_CANNOT_UPDATE;

    hash_map->free_func(found->value);
    found->value = pair.value;

    return SUCCESS;
}

char *hash_map_iter_start(struct hash_map *hash_map)
{
    hash_map->iter_index = 0;
    hash_map->iter_starting = 1;

    return hash_map_iter_next(hash_map);
}

char *hash_map_iter_next(struct hash_map *hash_map)
{
    struct pair *pair = hash_map->iter_starting
        ? NULL
        : queue_iter_next(hash_map->arr[hash_map->iter_index]);
    hash_map->iter_starting = 0;
    if (pair)
        return pair->key;

    while (pair == NULL)
    {
        if (++hash_map->iter_index == HASH_MAP_SIZE)
            return NULL;

        pair = queue_iter_start(hash_map->arr[hash_map->iter_index]);
    }

    return pair->key;
}

void hash_map_destroy(struct hash_map *hash_map)
{
    if (hash_map == NULL)
        return;

    for (size_t i = 0; i < HASH_MAP_SIZE; i++)
    {
        struct queue *queue = hash_map->arr[i];
        while (queue->length)
        {
            struct pair *data = queue_dequeue(queue);
            free(data->key);
            hash_map->free_func(data->value);
            free(data);
        }

        queue_destroy(queue);
    }

    free(hash_map);
}

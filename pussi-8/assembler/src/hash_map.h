#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "queue.h"

#define HASH_MAP_SIZE 32

typedef void (*free_func)(void *arg);

/**
 * Hash map of char pointers.
 * These pointers should be NULL or point to heap-allocated strings.
 * The data will be deallocated when the hash map is destroyed.
 */
struct pair
{
    char *key;
    char *value;
};

struct hash_map
{
    struct queue *arr[HASH_MAP_SIZE];
    size_t iter_index;
    int iter_starting;
};

struct hash_map *hash_map_create(void);
int hash_map_insert(struct hash_map *hash_map, struct pair pair);
char *hash_map_get(struct hash_map *hash_map, char *key);
char *hash_map_iter_start(struct hash_map *hash_map);
char *hash_map_iter_next(struct hash_map *hash_map);
void hash_map_destroy(struct hash_map *hash_map);

#endif /* ! HASH_MAP_H */

#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>

#include "hash_map/hash_map.h"
#include "utils/errors.h"

TestSuite(HashMap);

Test(HashMap, HashMapInit)
{
    struct hash_map *hash_map = hash_map_create(free);
    cr_expect(ne(ptr, hash_map, NULL));
    hash_map_destroy(hash_map);
}

Test(HashMap, HashMapInsert)
{
    struct hash_map *hash_map = hash_map_create(free);

    char *key = malloc(6);
    memcpy(key, "hello", 6);
    char *value = malloc(6);
    memcpy(value, "world", 6);
    struct pair pair = {
        .key = key,
        .value = value,
    };

    cr_assert(ne(ptr, hash_map, NULL));
    cr_expect(eq(int, hash_map_insert(hash_map, pair), SUCCESS));
    hash_map_destroy(hash_map);
}

Test(HashMap, HashMapInsertTwice)
{
    struct hash_map *hash_map = hash_map_create(free);

    char *key = malloc(6);
    memcpy(key, "hello", 6);
    char *value = malloc(6);
    memcpy(value, "world", 6);
    struct pair pair = {
        .key = key,
        .value = value,
    };

    cr_assert(ne(ptr, hash_map, NULL));
    cr_expect(eq(int, hash_map_insert(hash_map, pair), SUCCESS));
    cr_expect(eq(int, hash_map_insert(hash_map, pair), HASH_MAP_DUPE_ERROR));
    hash_map_destroy(hash_map);
}

Test(HashMap, HashMapInsertGet)
{
    struct hash_map *hash_map = hash_map_create(free);

    char *key = malloc(6);
    memcpy(key, "hello", 6);
    char *value = malloc(6);
    memcpy(value, "world", 6);
    struct pair pair = {
        .key = key,
        .value = value,
    };

    cr_assert(ne(ptr, hash_map, NULL));
    cr_expect(eq(int, hash_map_insert(hash_map, pair), SUCCESS));
    char *get = hash_map_get(hash_map, pair.key);
    cr_assert(ne(ptr, get, NULL));
    cr_expect(eq(str, get, pair.value));
    hash_map_destroy(hash_map);
}

Test(HashMap, HashMapIter)
{
    struct hash_map *hash_map = hash_map_create(free);

    char *key1 = malloc(2);
    memcpy(key1, "a", 2);
    struct pair pair = {
        .key = key1,
        .value = NULL,
    };

    cr_assert(ne(ptr, hash_map, NULL));
    cr_expect(eq(int, hash_map_insert(hash_map, pair), SUCCESS));

    char *key2 = malloc(2);
    // should have the same hash as 'a', can fail if the hash map size gets
    // changed
    memcpy(key2, "A", 2);
    pair.key = key2;
    pair.value = NULL;
    cr_expect(eq(int, hash_map_insert(hash_map, pair), SUCCESS));

    int i = 0;
    for (char *key = hash_map_iter_start(hash_map); key;
         key = hash_map_iter_next(hash_map))
    {
        if (i == 0)
            cr_expect(eq(str, key, key1));
        if (i == 1)
            cr_expect(eq(str, key, key2));
        i++;
    }

    cr_expect(eq(int, i, 2));
    hash_map_destroy(hash_map);
}

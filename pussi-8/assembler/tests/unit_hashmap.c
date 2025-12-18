#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>

#include "err.h"
#include "hash_map.h"

TestSuite(HashMap);

Test(HashMap, HashMapInit)
{
    struct hash_map *hash_map = hash_map_create();
    cr_expect(ne(ptr, hash_map, NULL));
    hash_map_destroy(hash_map);
}

Test(HashMap, HashMapInsert)
{
    struct hash_map *hash_map = hash_map_create();

    struct pair pair = {
        .key = "hello",
        .value = "world",
    };

    cr_expect(ne(ptr, hash_map, NULL));
    cr_expect(eq(int, hash_map_insert(hash_map, pair), SUCCESS));
    hash_map_destroy(hash_map);
}

Test(HashMap, HashMapInsertTwice)
{
    struct hash_map *hash_map = hash_map_create();

    struct pair pair = {
        .key = "hello",
        .value = "world",
    };

    cr_expect(ne(ptr, hash_map, NULL));
    cr_expect(eq(int, hash_map_insert(hash_map, pair), SUCCESS));
    cr_expect(eq(int, hash_map_insert(hash_map, pair), HASHMAP_DUPE_ERROR));
    hash_map_destroy(hash_map);
}

Test(HashMap, HashMapInsertGet)
{
    struct hash_map *hash_map = hash_map_create();

    struct pair pair = {
        .key = "hello",
        .value = "world",
    };

    cr_expect(ne(ptr, hash_map, NULL));
    cr_expect(eq(int, hash_map_insert(hash_map, pair), SUCCESS));
    char *get = hash_map_get(hash_map, pair.key);
    cr_expect(ne(ptr, get, NULL));
    cr_expect(eq(int, strcmp(get, pair.value), 0));
    hash_map_destroy(hash_map);
}

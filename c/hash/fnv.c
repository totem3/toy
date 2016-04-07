#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define OFFSET_BASIS 2166136261
#define FNV_PRIME 16777619
#define TABLE_SIZE 1000000

int collision_count = 0;
unsigned int fnv_hash(const char *key)
{
    unsigned int hash = OFFSET_BASIS;
    for (int i = 0; i < sizeof(key); i++) {
        hash = hash * FNV_PRIME;
        hash = hash ^ (key[i]);
    }
    return hash;
}

/* from redis src/dict.c {{{ */
static uint32_t dict_hash_function_seed = 5381;
unsigned int dictGenHashFunction(const void *key, int len) {
    /* 'm' and 'r' are mixing constants generated offline.
     They're not really 'magic', they just happen to work well.  */
    uint32_t seed = dict_hash_function_seed;
    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    /* Initialize the hash to a 'random' value */
    uint32_t h = seed ^ len;

    /* Mix 4 bytes at a time into the hash */
    const unsigned char *data = (const unsigned char *)key;

    while(len >= 4) {
        uint32_t k = *(uint32_t*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    /* Handle the last few bytes of the input array  */
    switch(len) {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0]; h *= m;
    };

    /* Do a few final mixes of the hash to ensure the last few
     * bytes are well-incorporated. */
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return (unsigned int)h;
}
/* }}} */

typedef struct entry {
    const char *key;
    int value;
} Entry;

typedef struct hash_table {
    Entry *entries;
} Map;

Map *new_map()
{
    Entry *entries =calloc(sizeof(Entry), TABLE_SIZE);
    Map *map = calloc(sizeof(Map), 1);
    map->entries = entries;
    return map;
}

void set(Map *m, const char *key, int value)
{
    int index = fnv_hash(key) % TABLE_SIZE;
    /* int index = dictGenHashFunction(key, sizeof(key)) % TABLE_SIZE; */
    if (m->entries[index].key != NULL) {
        collision_count++;
        /* printf("key collision: %s & %s\n", key, m->entries[index].key); */
    }
    m->entries[index].key = key;
    m->entries[index].value = value;
}

Entry *get(Map *m, const char *key)
{
    int index = fnv_hash(key) % TABLE_SIZE;
    /* int index = dictGenHashFunction(key, sizeof(key)) % TABLE_SIZE; */
    return &m->entries[index];
}

int main(int argc, char const* argv[])
{
    Map *map = new_map();
    int TIMES = 1000000;
    char keys[TIMES][8];
    memset(keys, 0, sizeof(keys));
    for (int i = 0; i < TIMES; i++) {
        sprintf(keys[i], "%d", i);
        set(map, keys[i], i);
    }
    for (int i = 0; i < TIMES; i++) {
        Entry *e = get(map, keys[i]);
        /* printf("%s => %d\n", e->key, e->value); */
    }

    printf("collision %d\n", collision_count);
    return 0;
}

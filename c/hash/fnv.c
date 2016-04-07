#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define OFFSET_BASIS 2166136261
#define FNV_PRIME 16777619
#define TABLE_SIZE 1000000

/* int collision_count = 0; */
unsigned int fnv_hash(const char *key)
{
    unsigned int hash = OFFSET_BASIS;
    for (int i = 0; i < sizeof(key); i++) {
        hash = hash * FNV_PRIME;
        hash = hash ^ (key[i]);
    }
    return hash;
}

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
    /* if (m->entries[index].key != NULL) { */
    /*     collision_count++; */
    /*     printf("key collision: %s & %s\n", key, m->entries[index].key); */
    /* } */
    m->entries[index].key = key;
    m->entries[index].value = value;
}

Entry *get(Map *m, const char *key)
{
    int index = fnv_hash(key) % TABLE_SIZE;
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

    return 0;
}

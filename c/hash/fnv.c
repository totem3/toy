#include <stdio.h>
#include <stdlib.h>
#define OFFSET_BASIS 2166136261
#define FNV_PRIME 16777619
unsigned int fnv_hash(const char *key)
{
    unsigned int hash = OFFSET_BASIS;
    for (int i = 0; i < sizeof(key); i++) {
        hash = hash * FNV_PRIME;
        hash = hash ^ (key[i]);
    }
    return hash;
}

void usage(const char *program)
{
    printf("usage: %s <int>\n", program);
}

int main(int argc, char const* argv[])
{

    const char *keys[3] = {"foo", "bar", "baz"};
    for (int i = 0; i < 3; i++) {
        printf("%s => %u\n", keys[i], fnv_hash(keys[i]));
    }
    return 0;
}

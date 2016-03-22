#include <stdio.h>

#define MAX_LENGTH 100
int table[100];

int hash(char *str)
{
    int n = 0;
    for (int l = 0; l < strlen(str); l++) {
        n += str[l] << l;
    }
    return n % MAX_LENGTH;
}

int get(int *table, char *key)
{
    int key_hash = hash(key);
    return table[key_hash];
}

void set(int *table, char *key, int value)
{
    int key_hash = hash(key);
    table[key_hash] = value;
}

int main(int argc, char const* argv[])
{
    set(table, "foo", 100);

    for (int i = 0; i < 10; i++) {
        int foo = get(table, "foo");
        foo++;
        set(table, "foo", foo);
    }
    int foo = get(table, "foo");
    printf("%d\n", foo);
    return 0;
}

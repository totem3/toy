#include <stdio.h>
#include "./tree.c"
#include "./dict.h"

void f(tree_t *t)
{
    t->value = t->value * 2;
}

int main(int argc, char const* argv[])
{
    dict_entry_t entry;
    entry.key = "test";
    entry.value = 77;
    printf("key: %s, value: %d\n", entry.key, entry.value);

    printf("create first tree\n");
    tree_t t = {
        .left = NULL,
        .right = NULL,
        .value = 100
    };
    printf("created first tree\n");
    printf("t.value: %d\n", t.value);
    printf("create second tree\n");
    tree_t *t2;
    t2->left = NULL;
    t2->right = NULL;
    t2->value = 200;
    printf("t2->value: %d\n", t2->value);
    insert_node(&t, t2);
    tree_t t3 = {
        .left = NULL,
        .right = NULL,
        .value = 50
    };
    printf("t3->value: %d\n", t3.value);
    insert_node(&t, &t3);
    print_tree(&t);
    traverse(&t, f);
    print_tree(&t);
    return 0;
}

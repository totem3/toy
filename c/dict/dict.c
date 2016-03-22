#include <stdio.h>
#include "./tree.c"
#include "./dict.h"

void f(tree_t *t)
{
    t->value = t->value * 2;
}

int main()
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
    tree_t t2;
    t2.left = NULL;
    t2.right = NULL;
    t2.value = 200;
    printf("t2->value: %d\n", t2.value);
    tree_t t3 = {
        .left = NULL,
        .right = NULL,
        .value = 50
    };
    tree_t t4 = {
        .left = NULL,
        .right = NULL,
        .value = 300
    };
    printf("t3->value: %d\n", t3.value);
    insert_node(&t, &t3);
    insert_node(&t3, &t2);
    insert_node(&t, &t4);
    traverse(&t, f);
    printf("t2 height: %d\n", tree_height(&t2));
    printf("t1 height: %d\n", tree_height(&t));
    print_tree(&t, 0);
    return 0;
}

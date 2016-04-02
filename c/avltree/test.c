#include "./avltree.h"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

int is_avl(tree_t*);
void assert_avl(tree_t *t)
{
    CU_ASSERT(is_avl(t));
}

void print_bias(tree_t *t)
{
  printf("bias(t(%s=>%d)) = %d\n", t->entry->key, t->entry->value, bias(t));
}

void test_valid_avl()
{
    tree_t root = new_tree("b", 5);
    tree_t l    = new_tree("a", 1);
    tree_t r    = new_tree("f", 50);
    tree_t rl   = new_tree("d", 30);
    tree_t rr   = new_tree("g", 100);
    tree_t rll  = new_tree("c", 20);
    tree_t rlr  = new_tree("e", 40);
    insert_tree(&root, &l);
    insert_tree(&root, &r);
    insert_tree(&root, &rl);
    insert_tree(&root, &rr);
    insert_tree(&root, &rll);
    insert_tree(&root, &rlr);

    print_tree(&rl);
    traverse_tree(&rl, print_bias);
    traverse_tree(&rl, assert_avl);
    traverse_tree(&rr, assert_avl);
    traverse_tree(&rll, assert_avl);
    traverse_tree(&rlr, assert_avl);
}

void test_valid_avl2()
{
    tree_t root = new_tree("10", 10);
    tree_t l    = new_tree("5", 5);
    tree_t r    = new_tree("4", 4);
    tree_t rl   = new_tree("3", 3);
    tree_t rll  = new_tree("2", 2);
    tree_t rlr  = new_tree("1", 1);
    tree_t rr   = new_tree("0", 0);
    insert_tree(&root, &l);
    insert_tree(&root, &r);
    insert_tree(&root, &rl);
    insert_tree(&root, &rr);
    insert_tree(&root, &rll);
    insert_tree(&root, &rlr);
    traverse_tree(&root, assert_avl);
}

void print_value(tree_t *t)
{
    printf("value: %d\n", t->entry->value);
}

int is_avl(tree_t *t)
{
    int b = bias(t);
    return (-1 <= b) && (b <= 1);
}

void test_height()
{
    tree_t t = new_tree("b", 10);
    tree_t t2 = new_tree("a", 5);
    tree_t t3 = new_tree("c", 15);
    CU_ASSERT(1 == tree_height(&t));
    insert_tree(&t, &t2);
    CU_ASSERT(2 == tree_height(&t));
    insert_tree(&t, &t3);
    CU_ASSERT(2 == tree_height(&t));
}

void test_insert_tree()
{
    tree_t t1 = new_tree("g", 10);
    tree_t t2 = new_tree("c", 5);
    tree_t t3 = new_tree("a", 4);
    insert_tree(&t1, &t2);
    insert_tree(&t1, &t3);
    CU_ASSERT(t1.parent == &t2);
    CU_ASSERT(t1.left == NULL);
    CU_ASSERT(t1.right == NULL);
    CU_ASSERT(t2.parent == NULL);
    CU_ASSERT(t2.left == &t3);
    CU_ASSERT(t2.right == &t1);
    CU_ASSERT(t3.parent == &t2);
    CU_ASSERT(t3.left == NULL);
    CU_ASSERT(t3.right == NULL);
}

void test_get_set()
{
    dict_t dict = new_dict();
    entry_t *e1 = get(&dict, "foo");
    CU_ASSERT(e1 == NULL);
    CU_ASSERT(set(&dict, "foo", 70) == 70);
    entry_t *e2 = get(&dict, "foo");
    CU_ASSERT(e2->value == 70);
}

char* rand_string(char *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

void test_many_set()
{
    dict_t d = new_dict();
    int i;
    size_t key_length = 16;
    char keys[16][10];
    for (i = 0; i < 10; i++) {
        /* struct timeval tp; */
        /* gettimeofday(&tp, NULL); */
        /* srandom(tp.tv_usec); */
        int value = i;
        sprintf(keys[i], "%08d", i);
        /* rand_string(key, key_length); */
        printf("key %s %d\n", keys[i], value);
        set(&d, keys[i], value);
    }
    printf("set done\n");
}

int main()
{
    /* CU_pSuite suite; */
    /* CU_initialize_registry(); */
    /* suite = CU_add_suite("avl tree suite", NULL, NULL); */
    /* CU_add_test(suite, "valid_val_tree", test_valid_avl); */
    /* CU_add_test(suite, "valid_val_tree2", test_valid_avl2); */
    /* CU_add_test(suite, "test tree_height", test_height); */
    /* CU_add_test(suite, "test insert_tree", test_insert_tree); */

    /* CU_add_test(suite, "test dict get set", test_get_set); */
    test_many_set();
    /* CU_add_test(suite, "test dict many set", test_many_set); */

    /* CU_basic_run_tests(); */
    /* CU_cleanup_registry(); */
    return 0;
}

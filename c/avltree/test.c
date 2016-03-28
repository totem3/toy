#include "./avltree.h"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

int is_avl(tree_t*);
void assert_avl(tree_t *t)
{
    CU_ASSERT(is_avl(t));
}

void test_valid_avl()
{
    tree_t root = { .value = 10 };
    tree_t l    = { .value = 5 };
    tree_t r    = { .value = 50 };
    tree_t rl   = { .value = 30 };
    tree_t rll  = { .value = 20 };
    tree_t rlr  = { .value = 40 };
    tree_t rr   = { .value = 100 };
    insert(&root, &l);
    insert(&root, &r);
    insert(&root, &rl);
    insert(&root, &rr);
    insert(&root, &rll);
    insert(&root, &rlr);
    traverse_tree(&root, assert_avl);
}

void test_valid_avl2()
{
    tree_t root = { .value = 10 };
    tree_t l    = { .value = 5 };
    tree_t r    = { .value = 4 };
    tree_t rl   = { .value = 3 };
    tree_t rll  = { .value = 2 };
    tree_t rlr  = { .value = 1 };
    tree_t rr   = { .value = 0 };
    insert(&root, &l);
    insert(&root, &r);
    insert(&root, &rl);
    insert(&root, &rr);
    insert(&root, &rll);
    insert(&root, &rlr);
    traverse_tree(&root, assert_avl);
}

void print_value(tree_t *t)
{
    printf("value: %d\n", t->value);
}

int is_avl(tree_t *t)
{
    int b = bias(t);
    return (-1 <= b) && (b <= 1);
}

void test_height()
{
    tree_t t = { .value = 10 };
    tree_t t2 = { .value = 5 };
    tree_t t3 = { .value = 15 };
    CU_ASSERT(1 == tree_height(&t));
    insert(&t, &t2);
    CU_ASSERT(2 == tree_height(&t));
    insert(&t, &t3);
    CU_ASSERT(2 == tree_height(&t));
}

void test_insert()
{
    tree_t t1 = { .value = 10 };
    tree_t t2 = { .value = 5 };
    tree_t t3 = { .value = 4 };
    insert(&t1, &t2);
    insert(&t1, &t3);
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

int main()
{
    CU_pSuite suite;
    CU_initialize_registry();
    suite = CU_add_suite("avl tree suite", NULL, NULL);
    CU_add_test(suite, "valid_val_tree", test_valid_avl);
    CU_add_test(suite, "valid_val_tree2", test_valid_avl2);
    CU_add_test(suite, "test tree_height", test_height);
    CU_add_test(suite, "test insert", test_insert);

    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}

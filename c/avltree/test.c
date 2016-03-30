#include "./avltree.h"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

int is_avl(tree_t*);
void assert_avl(tree_t *t)
{
    CU_ASSERT(is_avl(t));
}

void print_bias(tree_t *t)
{
  printf("bias(t(%s=>%d)) = %d\n", t->entry.key, t->entry.value, bias(t));
}

void test_valid_avl()
{
    tree_t root = { .entry = {.key = "b", .value = 5} };
    tree_t l    = { .entry = {.key = "a", .value = 1} };
    tree_t r    = { .entry = {.key = "f", .value = 50 } };
    tree_t rl   = { .entry = {.key = "d", .value = 30 } };
    tree_t rr   = { .entry = {.key = "g", .value = 100 } };
    tree_t rll  = { .entry = {.key = "c", .value = 20 } };
    tree_t rlr  = { .entry = {.key = "e", .value = 40 } };
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
    tree_t root = { .entry = {.key = "10", .value = 10 } };
    tree_t l    = { .entry = {.key = "5", .value = 5 } };
    tree_t r    = { .entry = {.key = "4", .value = 4 } };
    tree_t rl   = { .entry = {.key = "3", .value = 3 } };
    tree_t rll  = { .entry = {.key = "2", .value = 2 } };
    tree_t rlr  = { .entry = {.key = "1", .value = 1 } };
    tree_t rr   = { .entry = {.key = "0", .value = 0 } };
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
    printf("value: %d\n", t->entry.value);
}

int is_avl(tree_t *t)
{
    int b = bias(t);
    return (-1 <= b) && (b <= 1);
}

void test_height()
{
    tree_t t = { .entry = {.key = "b", .value = 10 } };
    tree_t t2 = { .entry = {.key = "a", .value = 5 } };
    tree_t t3 = { .entry = {.key = "c", .value = 15 } };
    CU_ASSERT(1 == tree_height(&t));
    insert_tree(&t, &t2);
    CU_ASSERT(2 == tree_height(&t));
    insert_tree(&t, &t3);
    CU_ASSERT(2 == tree_height(&t));
}

void test_insert_tree()
{
    tree_t t1 = { .entry = {.key = "g", .value = 10 } };
    tree_t t2 = { .entry = {.key = "c", .value = 5 } };
    tree_t t3 = { .entry = {.key = "a", .value = 4 } };
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

int main()
{
    CU_pSuite suite;
    CU_initialize_registry();
    suite = CU_add_suite("avl tree suite", NULL, NULL);
    CU_add_test(suite, "valid_val_tree", test_valid_avl);
    CU_add_test(suite, "valid_val_tree2", test_valid_avl2);
    CU_add_test(suite, "test tree_height", test_height);
    CU_add_test(suite, "test insert_tree", test_insert_tree);

    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}

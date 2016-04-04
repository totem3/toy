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
  printf("bias(t(%s=>%d)) = %d\n", t->entry->key, t->entry->value, bias(t));
}

void test_valid_avl()
{
    tree_t *root = new_tree("b", 5);
    tree_t *l    = new_tree("a", 1);
    tree_t *r    = new_tree("f", 50);
    tree_t *rl   = new_tree("d", 30);
    tree_t *rr   = new_tree("g", 100);
    tree_t *rll  = new_tree("c", 20);
    tree_t *rlr  = new_tree("e", 40);
    insert_tree(root, l);
    insert_tree(root, r);
    insert_tree(root, rl);
    insert_tree(root, rr);
    insert_tree(root, rll);
    insert_tree(root, rlr);

    traverse_tree(rl, assert_avl);
    traverse_tree(rr, assert_avl);
    traverse_tree(rll, assert_avl);
    traverse_tree(rlr, assert_avl);
}

void test_valid_avl2()
{
    tree_t *root = new_tree("10", 10);
    tree_t *l    = new_tree("5", 5);
    tree_t *r    = new_tree("4", 4);
    tree_t *rl   = new_tree("3", 3);
    tree_t *rll  = new_tree("2", 2);
    tree_t *rlr  = new_tree("1", 1);
    tree_t *rr   = new_tree("0", 0);
    insert_tree(root, l);
    insert_tree(root, r);
    insert_tree(root, rl);
    insert_tree(root, rr);
    insert_tree(root, rll);
    insert_tree(root, rlr);
    traverse_tree(root, assert_avl);
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
    tree_t *t = new_tree("b", 10);
    tree_t *t2 = new_tree("a", 5);
    tree_t *t3 = new_tree("c", 15);
    CU_ASSERT(1 == tree_height(t));
    insert_tree(t, t2);
    CU_ASSERT(2 == tree_height(t));
    insert_tree(t, t3);
    CU_ASSERT(2 == tree_height(t));
}

void test_insert_tree()
{
    tree_t *t1 = new_tree("g", 10);
    tree_t *t2 = new_tree("c", 5);
    tree_t *t3 = new_tree("a", 4);
    insert_tree(t1, t2);
    insert_tree(t1, t3);
    CU_ASSERT(t1->parent == t2);
    CU_ASSERT(t1->left == NULL);
    CU_ASSERT(t1->right == NULL);
    CU_ASSERT(t2->parent == NULL);
    CU_ASSERT(t2->left == t3);
    CU_ASSERT(t2->right == t1);
    CU_ASSERT(t3->parent == t2);
    CU_ASSERT(t3->left == NULL);
    CU_ASSERT(t3->right == NULL);
}

void test_bias_insert()
{
    dict_t *d = new_dict();
    set(d, "1", 1);
    CU_ASSERT(d->tree->height == 1);
    set(d, "2", 2);
    CU_ASSERT(d->tree->height == 2);
    set(d, "3", 3);
    CU_ASSERT(d->tree->height == 2);
    set(d, "4", 4);
    CU_ASSERT(d->tree->height == 3);
    set(d, "5", 5);
    CU_ASSERT(d->tree->height == 3);
    set(d, "6", 6);
    CU_ASSERT(d->tree->height == 3);
    set(d, "7", 7);
    CU_ASSERT(d->tree->height == 3);
    set(d, "8", 8);
    CU_ASSERT(d->tree->height == 4);
    set(d, "9", 9);
    CU_ASSERT(d->tree->height == 4);
    set(d, "10", 10);
    CU_ASSERT(d->tree->height == 4);
}

void test_get_set()
{
    dict_t *dict = new_dict();
    entry_t *e1 = get(dict, "foo");
    CU_ASSERT(e1 == NULL);
    CU_ASSERT(set(dict, "foo", 70) == 70);
    CU_ASSERT(set(dict, "bar", 74) == 74);
    CU_ASSERT(set(dict, "baz", 24) == 24);
    entry_t *e2 = get(dict, "foo");
    CU_ASSERT(e2 != NULL);
    if (e2 != NULL) {
        CU_ASSERT(e2->value == 70);
    }
    entry_t *e3 = get(dict, "bar");
    CU_ASSERT(e3 != NULL);
    if (e3 != NULL)
        CU_ASSERT(e3->value == 74);
    entry_t *e4 = get(dict, "baz");
    CU_ASSERT(e4 != NULL);
    if (e4 != NULL)
        CU_ASSERT(e4->value == 24);
}

void test_large_set_get()
{
    dict_t *d = new_dict();
    int TIMES = 100000;
    char keys[TIMES][8];
    memset(keys, 0, sizeof(keys));
    for (int i = 0; i < TIMES; i++) {
        /* printf("i %d\n", i); */
        sprintf(keys[i], "%d", i);
        set(d, keys[i], i);
        /* printf("inserted\n"); */
    }
    tree_t *root = d->tree;
    for (int i = 0; i < TIMES; i++) {
        entry_t *e = get(d, keys[i]);
        if (e == NULL) {
            /* printf("e = null\n"); */
        } else {
            CU_ASSERT(e->value == i);
        }
    }

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
    CU_add_test(suite, "test dict get set", test_get_set);
    CU_add_test(suite, "test biased insert", test_bias_insert);
    CU_add_test(suite, "test large dict get set", test_large_set_get);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}

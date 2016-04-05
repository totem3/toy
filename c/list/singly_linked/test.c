#include "./list.h"
#include <CUnit/CUnit.h>
#include <Cunit/Basic.h>

void test_insert()
{
    List list = { .head = NULL };
    Node node = { .value = 1, .next = NULL};
    insert(&list, &node);
    Node *n = get(&list, 0);
    CU_ASSERT(n->value == 1);

    Node node2 = { .value = 2, .next = NULL};
    Node node3 = { .value = 3, .next = NULL};

    insert(&list, &node2);
    insert(&list, &node3);
    Node *n1 = get(&list, 0);
    CU_ASSERT(n1->value == 3);
    Node *n2 = get(&list, 1);
    CU_ASSERT(n2->value == 2);
    Node *n3 = get(&list, 2);
    CU_ASSERT(n3->value == 1);
    Node *n4 = get(&list, 3);
    CU_ASSERT(n4 == NULL);
}

void test_remove()
{
    List *list = new_list();
    Node *n1 = new_node(1);
    Node *n2 = new_node(2);
    Node *n3 = new_node(3);

    insert(list, n1);
    insert(list, n2);
    insert(list, n3);

    Node *n = get(list, 1);
    CU_ASSERT(n->value == 2);
    remove_node(list, n2);
    Node *nn = get(list, 1);
    CU_ASSERT(nn->value == 1);
}

int main()
{
    CU_pSuite suite;
    CU_initialize_registry();
    suite = CU_add_suite("singly linked list test", NULL, NULL);
    CU_add_test(suite, "test_insert", test_insert);
    CU_add_test(suite, "test_remove", test_remove);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}

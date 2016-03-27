#include "./main.c"
#include <Cunit/Cunit.h>
#include <CUnit/Basic.h>

void test_sort_001(void)
{
    int array[] = {3,2,1};
    sort(array, 3);
    CU_ASSERT(array[0] == 3);
    CU_ASSERT(array[1] == 2);
    CU_ASSERT(array[2] == 1);
}

int main()
{
    CU_pSuite sort_suite;
    CU_initialize_registry();
    sort_suite = CU_add_suite("sort suite", NULL, NULL);
    CU_add_test(sort_suite, "test_001", test_sort_001);

    /* CU_add_test(sort_suite, "test_002", test_sort_002); */
    /* CU_add_test(sort_suite, "test_003", test_sort_003); */
    /* CU_add_test(sort_suite, "test_004", test_sort_004); */
    /* CU_add_test(sort_suite, "test_005", test_sort_005); */

    /* CU_console_run_tests(); */
    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}


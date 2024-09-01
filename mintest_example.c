#include "mintest.h"

static int test_int_value = 0;
static const char *test_string = "a test string";

/* 待测试函数1，计算两个int型数字和全局变量test_int_value的和，并更新test_int_value的值加1 */
int my_add_int(int a, int b)
{
    int ret = (a + b + test_int_value);
    test_int_value += 1;
    return ret;
}
/* 待测试函数2，计算两个double型数字和全局变量test_int_value的和，并更新test_int_value的值加1 */
double my_add_double(double a, double b)
{
    double ret = (a + b + (double)test_int_value);
    test_int_value += 1;
    return ret;
}

/* 测试准备函数，重置test_int_value值为1，这应该在每一个用例测试前被执行 */
void test_setup(void)
{
    test_int_value = 1;
}
/* 测试清理函数，重置test_int_value值为0，在每个用例测试结束后执行 */
void test_teardown(void)
{
    test_int_value = 0;
}

/* 定义测试用例test_assert_pass，验证mt_assert测试成功的场景 */
MT_TESTCASE(test_assert_pass)
{
    /* 用例开始执行前test_setup先被执行，因此这里test_int_value先被初始化为1 */
    /* 同一个用例不同测试项之间test_int_value的值不会被重置，这里每次my_add_int调用都会使test_int_value加1 */
    mt_assert(my_add_int(1, 1) == 3, "my_add_int(1, 1) calc result should be 3");
    mt_assert(my_add_int(1, 1) == 4, "my_add_int(1, 1) calc result should be 4");
    mt_assert(my_add_int(1, 1) == 5, "my_add_int(1, 1) calc result should be 5");
}

/* 定义测试用例test_assert_fail，验证mt_assert测试失败的场景 */
MT_TESTCASE(test_assert_fail)
{
    /* 同上，test_int_value先被初始化为1，后续每次my_add_int调用会使其加1，因此预期第一个断言通过，后续失败 */
    /* 同一测试用例中有任一断言失败则判定整个测试用例失败，后续逻辑不会再执行 */
    mt_assert(my_add_int(1, 1) == 3, "my_add_int(1, 1) calc result should be 3"); /* PASS */
    mt_assert(my_add_int(1, 1) == 3, "my_add_int(1, 1) calc result should be 4"); /* FAIL */
    mt_assert(my_add_int(1, 1) == 3, "my_add_int(1, 1) calc result should be 5"); /* NOT RUN */
}

/* 定义测试用例test_assert_int_eq_pass，验证mt_assert_int_eq断言测试成功的场景 */
MT_TESTCASE(test_assert_int_eq_pass)
{
    mt_assert_int_eq(3, my_add_int(1, 1));
    mt_assert_int_eq(4, my_add_int(1, 1));
    mt_assert_int_eq(5, my_add_int(1, 1));
}

/* 定义测试用例test_assert_int_eq_fail，验证mt_assert_int_eq断言测试失败的场景 */
MT_TESTCASE(test_assert_int_eq_fail)
{
    mt_assert_int_eq(3, my_add_int(1, 1)); /* PASS */
    mt_assert_int_eq(3, my_add_int(1, 1)); /* FAIL */
    mt_assert_int_eq(3, my_add_int(1, 1)); /* NOT RUN */
}

/* 定义测试用例test_assert_double_eq_pass，验证mt_assert_double_eq断言测试成功的场景 */
MT_TESTCASE(test_assert_double_eq_pass)
{
    mt_assert_double_eq(3.2, my_add_double(1.1, 1.1));
    mt_assert_double_eq(4.2, my_add_double(1.1, 1.1));
    mt_assert_double_eq(5.2, my_add_double(1.1, 1.1));
}

/* 定义测试用例test_assert_double_eq_fail，验证mt_assert_double_eq断言测试失败的场景 */
MT_TESTCASE(test_assert_double_eq_fail)
{
    mt_assert_double_eq(3.2, my_add_double(1.1, 1.1)); /* PASS */
    mt_assert_double_eq(3.2, my_add_double(1.1, 1.1)); /* FAIL */
    mt_assert_double_eq(3.2, my_add_double(1.1, 1.1)); /* NOT RUN */
}

/* 定义测试用例test_assert_string_eq_pass，验证mt_assert_string_eq断言测试成功的场景 */
MT_TESTCASE(test_assert_string_eq_pass)
{
    mt_assert_string_eq("a test string", test_string);
}

/* 定义测试用例test_assert_string_eq_fail，验证mt_assert_string_eq断言测试失败的场景 */
MT_TESTCASE(test_assert_string_eq_fail)
{
    mt_assert_string_eq("a test string", test_string); /* PASS */
    mt_assert_string_eq("hello world", test_string);   /* FAIL */
}

/* 组合上述一系列测试用例，定义基础测试套件test_suite1 */
MT_TESTSUITE(test_suite1)
{
    /* 配置用例的准备和清理函数 */
    MT_TESTSUITE_CONFIGURE(&test_setup, &test_teardown);

    /* 依次运行每个测试用例 */
    MT_RUN_TESTCASE(test_assert_pass);
    MT_RUN_TESTCASE(test_assert_fail);
    MT_RUN_TESTCASE(test_assert_int_eq_pass);
    MT_RUN_TESTCASE(test_assert_int_eq_fail);
    MT_RUN_TESTCASE(test_assert_double_eq_pass);
    MT_RUN_TESTCASE(test_assert_double_eq_fail);
    MT_RUN_TESTCASE(test_assert_string_eq_pass);
    MT_RUN_TESTCASE(test_assert_string_eq_fail);
}

/* 不配置用例准备和清理函数，组合部分测试用例定义测试套件test_suite2 */
MT_TESTSUITE(test_suite2)
{
    /* 在测试开始前手动初始化一次test_int_value值为1 */
    test_int_value = 1;

    /* 每个用例执行前test_int_value的值不会再被重置，因此用例之间会互相影响 */
    MT_RUN_TESTCASE(test_assert_int_eq_pass); /* 预期成功，因为test_int_value初始值为1 */
    MT_RUN_TESTCASE(test_assert_int_eq_pass); /* 预期失败，因为test_int_value初始值为4 */
}

/* 定义测试用例test_assert_int_array_eq_pass，验证mt_assert_int_array_eq断言测试成功的场景 */
MT_TESTCASE(test_assert_int_array_eq_pass)
{
    int a[] = {1, 2, 3};
    int b[] = {1, 2, 3};
    mt_assert_int_array_eq(a, 3, b, 3); /* PASS */
}

/* 定义测试用例test_assert_int_array_eq_fail1，验证mt_assert_int_array_eq断言数组长度不匹配测试失败的场景 */
MT_TESTCASE(test_assert_int_array_eq_fail1)
{
    int a[] = {1, 2, 3};
    int b[] = {1, 2};
    mt_assert_int_array_eq(a, 3, b, 2); /*FAIL*/
}

/* 定义测试用例test_assert_int_array_eq_fail2，验证mt_assert_int_array_eq断言数组元素不匹配测试失败的场景 */
MT_TESTCASE(test_assert_int_array_eq_fail2)
{
    int a[] = {1, 2, 3};
    int b[] = {1, 2, 4};
    mt_assert_int_array_eq(a, 3, b, 3); /*FAIL*/
}

/* 组合上述一系列测试用例，定义测试套件test_suite3，验证数组比较相关断言接口使用 */
MT_TESTSUITE(test_suite3)
{
    MT_RUN_TESTCASE(test_assert_int_array_eq_pass);
    MT_RUN_TESTCASE(test_assert_int_array_eq_fail1);
    MT_RUN_TESTCASE(test_assert_int_array_eq_fail2);
}

/* 定义测试用例test_assert_not_null_pass，验证mt_assert_not_null断言测试成功的场景 */
MT_TESTCASE(test_assert_not_null_pass)
{
    int *p = (int *)malloc(sizeof(int));
    mt_assert_not_null(p);
    free(p);
}

/* 定义测试用例test_assert_not_null_fail，验证mt_assert_not_null断言测试失败的场景 */
MT_TESTCASE(test_assert_not_null_fail)
{
    int *p = NULL;
    mt_assert_not_null(p);
    *p = 1; /* NOT RUN */
}

/* 定义测试用例test_assert_pointer_eq_pass，验证mt_assert_pointer_eq断言测试成功的场景 */
MT_TESTCASE(test_assert_pointer_eq_pass)
{
    int *p1 = (int *)malloc(sizeof(int));
    int *p2 = p1;
    mt_assert_pointer_eq(p1, p2);
    free(p1);
}

/* 定义测试用例test_assert_pointer_eq_fail，验证mt_assert_pointer_eq断言测试失败的场景 */
MT_TESTCASE(test_assert_pointer_eq_fail)
{
    int *p1 = (int *)malloc(sizeof(int));
    int *p2 = (int *)malloc(sizeof(int));
    mt_assert_pointer_eq(p1, p2);
    free(p1);
    free(p2);
}

/* 组合上述一系列测试用例，定义测试套件test_suite4，验证指针比较相关断言接口使用 */
MT_TESTSUITE(test_suite4)
{
    MT_RUN_TESTCASE(test_assert_not_null_pass);
    MT_RUN_TESTCASE(test_assert_not_null_fail);
    MT_RUN_TESTCASE(test_assert_pointer_eq_pass);
    MT_RUN_TESTCASE(test_assert_pointer_eq_fail);
}

int main(void)
{
    MT_RUN_TESTSUITE(test_suite1); /* 运行测试套件test_suite1 */
    MT_RUN_TESTSUITE(test_suite2); /* 运行测试套件test_suite2 */
    MT_RUN_TESTSUITE(test_suite3); /* 运行测试套件test_suite3 */
    MT_RUN_TESTSUITE(test_suite4); /* 运行测试套件test_suite4 */
    MT_REPORT_COUNT();             /* 打印所有用例测试结果的计数统计 */
    return MT_EXIT_CODE;
}

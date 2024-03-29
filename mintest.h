/*
 * mintest.h - 一个简单轻量化、在单个头文件内实现的C语言测试框架
 *
 * 复制本文件到项目中即可快速使用，具体使用示例可参考 mintest_example.c
 * 本文件中所有以 __ 开头的变量/宏定义均为框架内部使用定义，用户无需关注
 */

#ifndef __MINTEST_H__
#define __MINTEST_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/* 定义单个测试用例可缓存的消息数量（每个assert检查失败会缓存一条消息）和单条消息最大长度 */
/* 用户可通过在引用该头文件前先定义对应宏来覆盖当前的默认值 */
#ifndef MT_MESSAGE_MAX_NUM
#define MT_MESSAGE_MAX_NUM (8)
#endif
#ifndef MT_MESSAGE_MAX_LEN
#define MT_MESSAGE_MAX_LEN (256)
#endif

/* 浮点数比较精度，差值小于该值的两个浮点数认为相等，用户可自定义覆盖当前默认值 */
#ifndef MT_FLOAT_EPSILON
#define MT_FLOAT_EPSILON 1E-12
#endif

/* 测试用例执行的相关统计和状态记录 */
static int __mt_test_count = 0;  /* 已执行的测试用例数 */
static int __mt_fail_count = 0;  /* 失败的测试用例数 */
static int __mt_test_status = 0; /* 用于临时记录测试执行状态 */

/* 测试用例的前置准备工作和清理工作接口的函数指针，分别在测试逻辑执行前和执行后执行 */
/* 同一测试套件内的所有测试用例共用同一套准备和清理接口 */
static void (*__mt_setup_testcase)(void) = NULL;
static void (*__mt_teardown_testcase)(void) = NULL;

/* 记录测试过程assert检查失败时输出的信息 */
static struct __mt_message_cache__
{
    int msg_count;
    int overcount;
    char msg[MT_MESSAGE_MAX_NUM][MT_MESSAGE_MAX_LEN];
} __mt_message_cache;

/* 定义测试用例和测试套件，基于输入的用例/套件名构造测试框架内部使用的测试接口名 */
#define MT_TESTCASE(case_name) \
    static void __mt_testcase_##case_name(void)
#define MT_TESTSUITE(suite_name) \
    static void __mt_testsuite_##suite_name(void)

/* 使用失败的测试用例数作为测试结束的返回码 */
#define MT_EXIT_CODE __mt_fail_count

/* 配置测试套件中用例的的setup和teardown接口函数，二者分别在每个用例开始前和结束后执行 */
#define MT_TESTSUITE_CONFIGURE(setup_fn, teardown_fn) \
    do                                                \
    {                                                 \
        __mt_setup_testcase = setup_fn;               \
        __mt_teardown_testcase = teardown_fn;         \
    } while (0)

/* 运行指定测试套件，并在完成后置空setup和teardown函数指针避免干扰后续测试套件运行 */
#define MT_RUN_TESTSUITE(testsuite)                              \
    do                                                           \
    {                                                            \
        printf("=== TEST SUITE - %s is running:\n", #testsuite); \
        __mt_testsuite_##testsuite();                            \
        __mt_setup_testcase = NULL;                              \
        __mt_teardown_testcase = NULL;                           \
        printf("\n");                                            \
    } while (0)

/* 运行指定测试用例，包括用例的准备和清理工作、相关统计计数、结果格式化打印及刷新等 */
#define MT_RUN_TESTCASE(testcase)                                             \
    do                                                                        \
    {                                                                         \
        int i = 0;                                                            \
        if (__mt_setup_testcase)                                              \
        {                                                                     \
            (*__mt_setup_testcase)();                                         \
        }                                                                     \
        memset(&__mt_message_cache, 0, sizeof(struct __mt_message_cache__));  \
        __mt_test_status = 0;                                                 \
        __mt_testcase_##testcase();                                           \
        __mt_test_count++;                                                    \
        if (__mt_test_status > 0)                                             \
        {                                                                     \
            __mt_fail_count++;                                                \
            printf("[F] %s failed:\n", #testcase);                            \
            for (i = 0; i < __mt_message_cache.msg_count; i++)                \
            {                                                                 \
                printf("    %s\n", __mt_message_cache.msg[i]);                \
            }                                                                 \
            if (__mt_message_cache.overcount)                                 \
            {                                                                 \
                printf("    ...... %d messages have been truncated ......\n", \
                       __mt_message_cache.overcount);                         \
            }                                                                 \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            printf("[P] %s passed\n", #testcase);                             \
        }                                                                     \
        (void)fflush(stdout);                                                 \
        if (__mt_teardown_testcase)                                           \
        {                                                                     \
            (*__mt_teardown_testcase)();                                      \
        }                                                                     \
    } while (0)

/* 打印测试相关统计计数，包括测试用例的执行总数、通过数、失败数 */
#define MT_REPORT_COUNT()                                                               \
    do                                                                                  \
    {                                                                                   \
        printf("!!! TEST RESULT COUNT: total %d test cases, %d passed, %d failed!\n\n", \
               __mt_test_count, __mt_test_count - __mt_fail_count, __mt_fail_count);    \
    } while (0)

/* 格式化指定消息并存储到__mt_message_cache中，自动处理消息数/消息长度达到上限的情况 */
#define __MT_MESSAGE_CACHE_PUSH(fmt, ...)                                        \
    do                                                                           \
    {                                                                            \
        if (__mt_message_cache.msg_count < MT_MESSAGE_MAX_NUM)                   \
        {                                                                        \
            (void)snprintf(__mt_message_cache.msg[__mt_message_cache.msg_count], \
                           MT_MESSAGE_MAX_LEN, fmt, ##__VA_ARGS__);              \
            __mt_message_cache.msg_count++;                                      \
        }                                                                        \
        else                                                                     \
        {                                                                        \
            __mt_message_cache.overcount++;                                      \
        }                                                                        \
    } while (0)

/* 基础条件检查断言，判断condition条件是否为真，不为真时测试失败并格式化输出message信息 */
#define mt_assert(condition, message)                                          \
    do                                                                         \
    {                                                                          \
        if (!(condition))                                                      \
        {                                                                      \
            __MT_MESSAGE_CACHE_PUSH("%s:%d: %s", __FILE__, __LINE__, message); \
            __mt_test_status += 1;                                             \
        }                                                                      \
    } while (0)

/* 整数类型结果检查，判断期望结果和实际结果是否相等，不相等时测试失败并输出提示信息 */
#define mt_assert_int_eq(expected, result)                                           \
    do                                                                               \
    {                                                                                \
        int tmp_e = (expected);                                                      \
        int tmp_r = (result);                                                        \
        if (tmp_e != tmp_r)                                                          \
        {                                                                            \
            __MT_MESSAGE_CACHE_PUSH("%s:%d: expected result: %d, actual result: %d", \
                                    __FILE__, __LINE__, tmp_e, tmp_r);               \
            __mt_test_status += 1;                                                   \
        }                                                                            \
    } while (0)

/* 浮点数类型结果检查，判断期望结果和实际结果差值是否小于判断精度，不符合时测试失败并输出提示信息 */
#define mt_assert_double_eq(expected, result)                                                \
    do                                                                                       \
    {                                                                                        \
        double tmp_e = (expected);                                                           \
        double tmp_r = (result);                                                             \
        int significant_figures = 1 - log10(MT_FLOAT_EPSILON);                               \
        if (fabs(tmp_e - tmp_r) > MT_FLOAT_EPSILON)                                          \
        {                                                                                    \
            __MT_MESSAGE_CACHE_PUSH("%s:%d: expected result: %.*g, actual result: %.*g",     \
                                    __FILE__, __LINE__,                                      \
                                    significant_figures, tmp_e, significant_figures, tmp_r); \
            __mt_test_status += 1;                                                           \
        }                                                                                    \
    } while (0)

/* 字符串类型结果检查，判断期望结果和实际结果是否相等，不相等时测试失败并输出提示信息 */
#define mt_assert_string_eq(expected, result)                                                \
    do                                                                                       \
    {                                                                                        \
        const char *tmp_e = expected ? expected : "<null pointer>";                          \
        const char *tmp_r = result ? result : "<null pointer>";                              \
        if (0 != strcmp(tmp_e, tmp_r))                                                       \
        {                                                                                    \
            __MT_MESSAGE_CACHE_PUSH("%s:%d: expected result: \"%s\", actual result: \"%s\"", \
                                    __FILE__, __LINE__, tmp_e, tmp_r);                       \
            __mt_test_status += 1;                                                           \
        }                                                                                    \
    } while (0)

#endif /* __MINTEST_H__ */

/**
 * MIT License
 *
 * Copyright (c) 2024 Ming Kuang <ming@imkuang.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * mintest.h - 一个简单、轻量化、在单个头文件内实现的 C 语言测试框架
 *
 * 项目地址：https://git.imkuang.com/imkuang/mintest
 * 当前版本：v1.0.0
 *
 * 复制本文件到项目中并引用即可使用框架支持的全部测试定义和运行接口，具体使用
 * 示例可参考 mintest_example.c 及源码注释（本文件中所有以 __ 开头的变量/宏
 * 定义均为框架内部使用定义，用户无需关注）
 */

#ifndef __MINTEST_H__
#define __MINTEST_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/* 测试检查失败提示消息的最大长度，可通过在引用该头文件前定义对应宏来覆盖当前默认值 */
#ifndef MT_MESSAGE_MAX_LEN
#define MT_MESSAGE_MAX_LEN (512)
#endif

/* 浮点数比较精度，差值小于该精度的两个浮点数会被认为相等，支持自定义覆盖当前默认值 */
#ifndef MT_FLOAT_EPSILON
#define MT_FLOAT_EPSILON (1E-12)
#endif

/* 测试用例执行的相关统计和状态记录 */
static int __mt_testcase_total_count = 0; /* 已执行的测试用例总数 */
static int __mt_testcase_fail_count = 0;  /* 执行失败的测试用例数 */
static int __mt_testcase_run_status = 0;  /* 临时记录当前测试用例执行状态（0:成功 / 1:失败） */

/* 测试用例的前置准备工作和清理工作接口函数指针，分别在测试逻辑执行前和执行后被调用 */
/* 同一测试套件内的所有测试用例共用同一套准备和清理接口 */
static void (*__mt_setup_testcase)(void) = NULL;
static void (*__mt_teardown_testcase)(void) = NULL;

/* 缓存测试过程assert检查失败时输出的消息用于后续格式化打印输出 */
static char __mt_message_cache[MT_MESSAGE_MAX_LEN] = {0};

/* 用于定义测试用例和测试套件，基于输入的用例/套件名构造测试框架内部使用的测试接口函数 */
#define MT_TESTCASE(case_name) \
    static void __mt_testcase_##case_name(void)
#define MT_TESTSUITE(suite_name) \
    static void __mt_testsuite_##suite_name(void)

/* 使用测试失败的用例数作为测试结束的退出状态码，没有失败用例时为0，即测试成功 */
#define MT_EXIT_CODE __mt_testcase_fail_count

/* 配置测试套件中用例的的setup和teardown接口函数，二者分别在每个用例开始前和结束后执行 */
#define MT_TESTSUITE_CONFIGURE(setup_fn, teardown_fn) \
    do                                                \
    {                                                 \
        __mt_setup_testcase = setup_fn;               \
        __mt_teardown_testcase = teardown_fn;         \
    } while (0)

/* 运行指定测试套件，并在完成后置空setup和teardown函数指针避免干扰后续测试套件的运行 */
#define MT_RUN_TESTSUITE(testsuite)                            \
    do                                                         \
    {                                                          \
        printf("=== TEST SUITE - %s start:\n", #testsuite);    \
        __mt_testsuite_##testsuite();                          \
        __mt_setup_testcase = NULL;                            \
        __mt_teardown_testcase = NULL;                         \
        printf("=== TEST SUITE - %s finish!\n\n", #testsuite); \
    } while (0)

/* 运行指定测试用例，包括用例的准备和清理工作、相关统计计数、结果格式化打印及刷新等 */
#define MT_RUN_TESTCASE(testcase)                          \
    do                                                     \
    {                                                      \
        if (__mt_setup_testcase)                           \
        {                                                  \
            (*__mt_setup_testcase)();                      \
        }                                                  \
        memset(__mt_message_cache, 0, MT_MESSAGE_MAX_LEN); \
        __mt_testcase_run_status = 0;                      \
        __mt_testcase_##testcase();                        \
        __mt_testcase_total_count++;                       \
        if (__mt_testcase_run_status)                      \
        {                                                  \
            __mt_testcase_fail_count++;                    \
            printf("[F] %s failed:\n", #testcase);         \
            printf("    %s\n", __mt_message_cache);        \
        }                                                  \
        else                                               \
        {                                                  \
            printf("[P] %s passed\n", #testcase);          \
        }                                                  \
        (void)fflush(stdout);                              \
        if (__mt_teardown_testcase)                        \
        {                                                  \
            (*__mt_teardown_testcase)();                   \
        }                                                  \
    } while (0)

/* 打印测试相关统计计数，包括测试用例的执行总数、通过数、失败数 */
#define MT_REPORT_COUNT()                                                             \
    do                                                                                \
    {                                                                                 \
        printf("!!! ==========================================================\n");   \
        printf("!!! TEST COUNT: total %d test cases, %d passed, %d failed!\n",        \
               __mt_testcase_total_count,                                             \
               __mt_testcase_total_count - __mt_testcase_fail_count,                  \
               __mt_testcase_fail_count);                                             \
        printf("!!! ==========================================================\n\n"); \
    } while (0)

/* 条件检查断言，在condition条件不为真时测试失败，结束当前测试用例并基于message信息生成提示消息 */
#define mt_assert(condition, message)                              \
    do                                                             \
    {                                                              \
        if (!(condition))                                          \
        {                                                          \
            (void)snprintf(__mt_message_cache, MT_MESSAGE_MAX_LEN, \
                           "%s:%d: %s",                            \
                           __FILE__, __LINE__, message);           \
            __mt_testcase_run_status = 1;                          \
            return;                                                \
        }                                                          \
    } while (0)

/* 指针判空断言，在pointer指针为NULL时测试失败，结束当前测试用例并生成固定格式的提示消息 */
#define mt_assert_not_null(pointer)                                \
    do                                                             \
    {                                                              \
        if (NULL == (pointer))                                     \
        {                                                          \
            (void)snprintf(__mt_message_cache, MT_MESSAGE_MAX_LEN, \
                           "%s:%d: %s should not be NULL",         \
                           __FILE__, __LINE__, #pointer);          \
            __mt_testcase_run_status = 1;                          \
            return;                                                \
        }                                                          \
    } while (0)

/* 指针类型结果检查，期望结果与实际结果不相等时测试失败，结束当前测试用例并并生成固定格式的提示消息 */
#define mt_assert_pointer_eq(expected, result)                              \
    do                                                                      \
    {                                                                       \
        void *tmp_e = (expected);                                           \
        void *tmp_r = (result);                                             \
        if (tmp_e != tmp_r)                                                 \
        {                                                                   \
            (void)snprintf(__mt_message_cache, MT_MESSAGE_MAX_LEN,          \
                           "%s:%d: expected result: %p, actual result: %p", \
                           __FILE__, __LINE__, tmp_e, tmp_r);               \
            __mt_testcase_run_status = 1;                                   \
            return;                                                         \
        }                                                                   \
    } while (0)

/* 整数类型结果检查，期望结果与实际结果不相等时测试失败，结束当前测试用例并并生成固定格式的提示消息 */
#define mt_assert_int_eq(expected, result)                                  \
    do                                                                      \
    {                                                                       \
        int tmp_e = (expected);                                             \
        int tmp_r = (result);                                               \
        if (tmp_e != tmp_r)                                                 \
        {                                                                   \
            (void)snprintf(__mt_message_cache, MT_MESSAGE_MAX_LEN,          \
                           "%s:%d: expected result: %d, actual result: %d", \
                           __FILE__, __LINE__, tmp_e, tmp_r);               \
            __mt_testcase_run_status = 1;                                   \
            return;                                                         \
        }                                                                   \
    } while (0)

/* 浮点数类型结果检查，期望结果与实际结果的差值大于判定精度时测试失败 */
#define mt_assert_double_eq(expected, result)                                       \
    do                                                                              \
    {                                                                               \
        double tmp_e = (expected);                                                  \
        double tmp_r = (result);                                                    \
        int significant_figures = 1 - log10(MT_FLOAT_EPSILON);                      \
        if (fabs(tmp_e - tmp_r) > MT_FLOAT_EPSILON)                                 \
        {                                                                           \
            (void)snprintf(__mt_message_cache, MT_MESSAGE_MAX_LEN,                  \
                           "%s:%d: expected result: %.*g, actual result: %.*g",     \
                           __FILE__, __LINE__,                                      \
                           significant_figures, tmp_e, significant_figures, tmp_r); \
            __mt_testcase_run_status = 1;                                           \
            return;                                                                 \
        }                                                                           \
    } while (0)

/* 字符串类型结果检查，期望结果与实际结果不相等时测试失败 */
#define mt_assert_string_eq(expected, result)                                       \
    do                                                                              \
    {                                                                               \
        const char *tmp_e = (expected) ? (expected) : "<null pointer>";             \
        const char *tmp_r = (result) ? (result) : "<null pointer>";                 \
        if (0 != strcmp(tmp_e, tmp_r))                                              \
        {                                                                           \
            (void)snprintf(__mt_message_cache, MT_MESSAGE_MAX_LEN,                  \
                           "%s:%d: expected result: \"%s\", actual result: \"%s\"", \
                           __FILE__, __LINE__, tmp_e, tmp_r);                       \
            __mt_testcase_run_status = 1;                                           \
            return;                                                                 \
        }                                                                           \
    } while (0)

/* 整型数组结果检查，期望结果与实际结果的长度不相等或者对应元素不相等时测试失败 */
#define mt_assert_int_array_eq(expected, expected_len, result, result_len)              \
    do                                                                                  \
    {                                                                                   \
        int *tmp_e = (expected);                                                        \
        int *tmp_r = (result);                                                          \
        int tmp_e_len = (expected_len);                                                 \
        int tmp_r_len = (result_len);                                                   \
        int i = 0;                                                                      \
        if (tmp_e_len != tmp_r_len)                                                     \
        {                                                                               \
            (void)snprintf(__mt_message_cache, MT_MESSAGE_MAX_LEN,                      \
                           "%s:%d: expected array length: %d, actual array length: %d", \
                           __FILE__, __LINE__, tmp_e_len, tmp_r_len);                   \
            __mt_testcase_run_status = 1;                                               \
            return;                                                                     \
        }                                                                               \
        for (i = 0; i < tmp_e_len; i++)                                                 \
        {                                                                               \
            if (tmp_e[i] != tmp_r[i])                                                   \
            {                                                                           \
                (void)snprintf(__mt_message_cache, MT_MESSAGE_MAX_LEN,                  \
                               "%s:%d: expected array[%d]: %d, actual array[%d]: %d",   \
                               __FILE__, __LINE__, i, tmp_e[i], i, tmp_r[i]);           \
                __mt_testcase_run_status = 1;                                           \
                return;                                                                 \
            }                                                                           \
        }                                                                               \
    } while (0)

#endif /* __MINTEST_H__ */

/*
 * check.h - C 与 C++ 测试共用的极简断言宏。
 * 与 assert 不同：失败时不会中止程序，并且在 Release 构建下依然生效。
 * 每个测试可执行文件只能有一个源文件包含本头文件（计数器是 static 的）。
 */
#ifndef DSA_COMMON_CHECK_H
#define DSA_COMMON_CHECK_H

#include <stdio.h>

static int g_check_total = 0;
static int g_check_failed = 0;

/* 可变参数宏：条件中含有逗号（如 std::pair<int, int>）也能正常使用 */
#define CHECK(...)                                                         \
    do {                                                                   \
        ++g_check_total;                                                   \
        if (!(__VA_ARGS__)) {                                              \
            ++g_check_failed;                                              \
            printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #__VA_ARGS__); \
        }                                                                  \
    } while (0)

#define TEST_SECTION(name) printf("[%s]\n", name)

/* 在 main 的最后 return CHECK_REPORT(); */
#define CHECK_REPORT()                                                     \
    (printf("%s: %d/%d checks passed\n", g_check_failed ? "FAILED" : "OK", \
            g_check_total - g_check_failed, g_check_total),                \
     g_check_failed ? 1 : 0)

#endif

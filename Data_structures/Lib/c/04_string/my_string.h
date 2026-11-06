/*
 * my_string.h - 手写 C 标准库字符串函数，以及字符串匹配算法。
 *
 * 匹配算法：
 *   - 朴素匹配（暴力）  O(n*m)
 *   - KMP               O(n+m)
 *
 * next 数组定义：next[i] = pat[0..i] 的最长"相等真前缀与真后缀"的长度。
 * 例如 pat = "ababaca"，next = {0,0,1,2,3,0,1}。
 */
#ifndef DSA_C_MY_STRING_H
#define DSA_C_MY_STRING_H

#include <stddef.h>

size_t my_strlen(const char *s);
char *my_strcpy(char *dst, const char *src);
char *my_strncpy(char *dst, const char *src, size_t n);
char *my_strcat(char *dst, const char *src);
int my_strcmp(const char *a, const char *b);
char *my_strchr(const char *s, int c);
char *my_strdup(const char *s); /* 调用者负责 free */
void my_str_reverse(char *s);

/* 找不到返回 -1，pat 为空串时返回 0 */
long bf_search(const char *text, const char *pat);

void kmp_build_next(const char *pat, size_t m, int *next);
long kmp_search(const char *text, const char *pat);
/* 找出所有出现位置（允许重叠），最多写入 max 个，返回总出现次数 */
size_t kmp_search_all(const char *text, const char *pat, size_t *positions, size_t max);

#endif

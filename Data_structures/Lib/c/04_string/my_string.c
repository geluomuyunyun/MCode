#include "my_string.h"

#include <stdlib.h>

size_t my_strlen(const char *s)
{
    const char *p = s;
    while (*p)
        ++p;
    return (size_t)(p - s);
}

char *my_strcpy(char *dst, const char *src)
{
    char *d = dst;
    while ((*d++ = *src++) != '\0') {
    }
    return dst;
}

char *my_strncpy(char *dst, const char *src, size_t n)
{
    /* 与标准库一致：src 不足 n 个字符时用 '\0' 补齐；src 过长时不会补 '\0' */
    size_t i = 0;
    for (; i < n && src[i]; ++i)
        dst[i] = src[i];
    for (; i < n; ++i)
        dst[i] = '\0';
    return dst;
}

char *my_strcat(char *dst, const char *src)
{
    my_strcpy(dst + my_strlen(dst), src);
    return dst;
}

int my_strcmp(const char *a, const char *b)
{
    while (*a && *a == *b) {
        ++a;
        ++b;
    }
    /* 按 unsigned char 比较，和标准库行为一致 */
    return (int)(unsigned char)*a - (int)(unsigned char)*b;
}

char *my_strchr(const char *s, int c)
{
    for (;; ++s) {
        if (*s == (char)c)
            return (char *)s;
        if (*s == '\0')
            return NULL;
    }
}

char *my_strdup(const char *s)
{
    size_t n = my_strlen(s) + 1;
    char *p = (char *)malloc(n);
    if (p)
        my_strcpy(p, s);
    return p;
}

void my_str_reverse(char *s)
{
    size_t n = my_strlen(s);
    if (n < 2)
        return;
    for (size_t i = 0, j = n - 1; i < j; ++i, --j) {
        char t = s[i];
        s[i] = s[j];
        s[j] = t;
    }
}

long bf_search(const char *text, const char *pat)
{
    size_t n = my_strlen(text), m = my_strlen(pat);
    if (m == 0)
        return 0;
    for (size_t i = 0; i + m <= n; ++i) {
        size_t j = 0;
        while (j < m && text[i + j] == pat[j])
            ++j;
        if (j == m)
            return (long)i;
    }
    return -1;
}

void kmp_build_next(const char *pat, size_t m, int *next)
{
    if (m == 0)
        return;
    next[0] = 0;
    int k = 0; /* 当前最长相等前后缀的长度 */
    for (size_t i = 1; i < m; ++i) {
        while (k > 0 && pat[i] != pat[k])
            k = next[k - 1]; /* 失配时回退到更短的相等前后缀 */
        if (pat[i] == pat[k])
            ++k;
        next[i] = k;
    }
}

size_t kmp_search_all(const char *text, const char *pat, size_t *positions, size_t max)
{
    size_t n = my_strlen(text), m = my_strlen(pat);
    if (m == 0 || m > n)
        return 0;
    int *next = (int *)malloc(m * sizeof(int));
    if (!next)
        return 0;
    kmp_build_next(pat, m, next);

    size_t count = 0;
    size_t j = 0; /* 已匹配的长度 */
    for (size_t i = 0; i < n; ++i) {
        while (j > 0 && text[i] != pat[j])
            j = (size_t)next[j - 1];
        if (text[i] == pat[j])
            ++j;
        if (j == m) {
            if (positions && count < max)
                positions[count] = i + 1 - m;
            ++count;
            j = (size_t)next[j - 1]; /* 继续寻找下一个（允许重叠） */
        }
    }
    free(next);
    return count;
}

long kmp_search(const char *text, const char *pat)
{
    if (*pat == '\0')
        return 0;
    size_t pos;
    return kmp_search_all(text, pat, &pos, 1) > 0 ? (long)pos : -1;
}

/*
 * dynamic_array.h - 动态数组（可自动扩容的顺序表），元素类型为 int。
 *
 * 学习要点：
 *   - malloc / realloc / free 的配对使用
 *   - 容量翻倍扩容，均摊 O(1) 的尾部插入
 *   - 元素数量降到容量 1/4 时缩容，避免抖动
 *
 * 约定：返回 int 的修改类函数，0 表示成功，-1 表示失败（越界或内存不足）。
 */
#ifndef DSA_C_DYNAMIC_ARRAY_H
#define DSA_C_DYNAMIC_ARRAY_H

#include <stddef.h>

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} DynArray;

DynArray *da_create(size_t init_capacity);
void da_destroy(DynArray *a);

int da_push_back(DynArray *a, int value);
int da_pop_back(DynArray *a, int *out);
int da_insert(DynArray *a, size_t index, int value); /* index 可以等于 size */
int da_erase(DynArray *a, size_t index);

int da_get(const DynArray *a, size_t index, int *out);
int da_set(DynArray *a, size_t index, int value);

long da_find(const DynArray *a, int value); /* 找不到返回 -1 */
void da_reverse(DynArray *a);
void da_clear(DynArray *a);

size_t da_size(const DynArray *a);
size_t da_capacity(const DynArray *a);
void da_print(const DynArray *a);

#endif

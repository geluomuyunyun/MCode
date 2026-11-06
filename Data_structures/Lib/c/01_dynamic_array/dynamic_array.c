#include "dynamic_array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DA_MIN_CAPACITY 4

static int da_resize(DynArray *a, size_t new_capacity)
{
    if (new_capacity < DA_MIN_CAPACITY)
        new_capacity = DA_MIN_CAPACITY;
    /* realloc 失败时原内存仍然有效，所以先用临时指针接收 */
    int *p = (int *)realloc(a->data, new_capacity * sizeof(int));
    if (!p)
        return -1;
    a->data = p;
    a->capacity = new_capacity;
    return 0;
}

DynArray *da_create(size_t init_capacity)
{
    DynArray *a = (DynArray *)malloc(sizeof(DynArray));
    if (!a)
        return NULL;
    a->data = NULL;
    a->size = 0;
    a->capacity = 0;
    if (da_resize(a, init_capacity) != 0) {
        free(a);
        return NULL;
    }
    return a;
}

void da_destroy(DynArray *a)
{
    if (!a)
        return;
    free(a->data);
    free(a);
}

int da_push_back(DynArray *a, int value)
{
    return da_insert(a, a->size, value);
}

int da_pop_back(DynArray *a, int *out)
{
    if (a->size == 0)
        return -1;
    if (out)
        *out = a->data[a->size - 1];
    return da_erase(a, a->size - 1);
}

int da_insert(DynArray *a, size_t index, int value)
{
    if (index > a->size)
        return -1;
    if (a->size == a->capacity && da_resize(a, a->capacity * 2) != 0)
        return -1;
    /* 内存区域重叠，必须用 memmove 而不是 memcpy */
    memmove(a->data + index + 1, a->data + index, (a->size - index) * sizeof(int));
    a->data[index] = value;
    a->size++;
    return 0;
}

int da_erase(DynArray *a, size_t index)
{
    if (index >= a->size)
        return -1;
    memmove(a->data + index, a->data + index + 1, (a->size - index - 1) * sizeof(int));
    a->size--;
    if (a->capacity > DA_MIN_CAPACITY && a->size <= a->capacity / 4)
        da_resize(a, a->capacity / 2); /* 缩容失败不影响正确性，忽略返回值 */
    return 0;
}

int da_get(const DynArray *a, size_t index, int *out)
{
    if (index >= a->size)
        return -1;
    *out = a->data[index];
    return 0;
}

int da_set(DynArray *a, size_t index, int value)
{
    if (index >= a->size)
        return -1;
    a->data[index] = value;
    return 0;
}

long da_find(const DynArray *a, int value)
{
    for (size_t i = 0; i < a->size; ++i)
        if (a->data[i] == value)
            return (long)i;
    return -1;
}

void da_reverse(DynArray *a)
{
    if (a->size < 2)
        return;
    for (size_t i = 0, j = a->size - 1; i < j; ++i, --j) {
        int t = a->data[i];
        a->data[i] = a->data[j];
        a->data[j] = t;
    }
}

void da_clear(DynArray *a)
{
    a->size = 0;
}

size_t da_size(const DynArray *a)
{
    return a->size;
}

size_t da_capacity(const DynArray *a)
{
    return a->capacity;
}

void da_print(const DynArray *a)
{
    printf("[");
    for (size_t i = 0; i < a->size; ++i)
        printf(i ? ", %d" : "%d", a->data[i]);
    printf("] (size=%zu, capacity=%zu)\n", a->size, a->capacity);
}

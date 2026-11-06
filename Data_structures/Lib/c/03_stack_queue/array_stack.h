/*
 * array_stack.h - 顺序栈（基于可扩容数组）。
 * 栈顶在数组末尾，push/pop 均摊 O(1)。
 */
#ifndef DSA_C_ARRAY_STACK_H
#define DSA_C_ARRAY_STACK_H

#include <stddef.h>

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} ArrayStack;

ArrayStack *as_create(size_t init_capacity);
void as_destroy(ArrayStack *s);
int as_push(ArrayStack *s, int val);
int as_pop(ArrayStack *s, int *out);
int as_peek(const ArrayStack *s, int *out);
int as_empty(const ArrayStack *s);
size_t as_size(const ArrayStack *s);

#endif

#include "array_stack.h"

#include <stdlib.h>

ArrayStack *as_create(size_t init_capacity)
{
    ArrayStack *s = (ArrayStack *)malloc(sizeof(ArrayStack));
    if (!s)
        return NULL;
    s->capacity = init_capacity ? init_capacity : 4;
    s->size = 0;
    s->data = (int *)malloc(s->capacity * sizeof(int));
    if (!s->data) {
        free(s);
        return NULL;
    }
    return s;
}

void as_destroy(ArrayStack *s)
{
    if (!s)
        return;
    free(s->data);
    free(s);
}

int as_push(ArrayStack *s, int val)
{
    if (s->size == s->capacity) {
        int *p = (int *)realloc(s->data, s->capacity * 2 * sizeof(int));
        if (!p)
            return -1;
        s->data = p;
        s->capacity *= 2;
    }
    s->data[s->size++] = val;
    return 0;
}

int as_pop(ArrayStack *s, int *out)
{
    if (s->size == 0)
        return -1;
    s->size--;
    if (out)
        *out = s->data[s->size];
    return 0;
}

int as_peek(const ArrayStack *s, int *out)
{
    if (s->size == 0)
        return -1;
    *out = s->data[s->size - 1];
    return 0;
}

int as_empty(const ArrayStack *s)
{
    return s->size == 0;
}

size_t as_size(const ArrayStack *s)
{
    return s->size;
}

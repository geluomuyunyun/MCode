#include "link_stack.h"

#include <stdlib.h>

LinkStack *ls_create(void)
{
    LinkStack *s = (LinkStack *)malloc(sizeof(LinkStack));
    if (!s)
        return NULL;
    s->top = NULL;
    s->size = 0;
    return s;
}

void ls_destroy(LinkStack *s)
{
    if (!s)
        return;
    while (ls_pop(s, NULL) == 0) {
    }
    free(s);
}

int ls_push(LinkStack *s, int val)
{
    LSNode *n = (LSNode *)malloc(sizeof(LSNode));
    if (!n)
        return -1;
    n->val = val;
    n->next = s->top;
    s->top = n;
    s->size++;
    return 0;
}

int ls_pop(LinkStack *s, int *out)
{
    if (!s->top)
        return -1;
    LSNode *n = s->top;
    if (out)
        *out = n->val;
    s->top = n->next;
    free(n);
    s->size--;
    return 0;
}

int ls_peek(const LinkStack *s, int *out)
{
    if (!s->top)
        return -1;
    *out = s->top->val;
    return 0;
}

int ls_empty(const LinkStack *s)
{
    return s->top == NULL;
}

size_t ls_size(const LinkStack *s)
{
    return s->size;
}

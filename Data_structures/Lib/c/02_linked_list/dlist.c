#include "dlist.h"

#include <stdio.h>
#include <stdlib.h>

DList *dl_create(void)
{
    DList *l = (DList *)malloc(sizeof(DList));
    if (!l)
        return NULL;
    l->head.prev = l->head.next = &l->head;
    l->size = 0;
    return l;
}

void dl_destroy(DList *l)
{
    if (!l)
        return;
    DNode *p = l->head.next;
    while (p != &l->head) {
        DNode *next = p->next;
        free(p);
        p = next;
    }
    free(l);
}

/* 在 pos 之前插入新结点 */
static int dl_link_before(DList *l, DNode *pos, int val)
{
    DNode *n = (DNode *)malloc(sizeof(DNode));
    if (!n)
        return -1;
    n->val = val;
    n->prev = pos->prev;
    n->next = pos;
    pos->prev->next = n;
    pos->prev = n;
    l->size++;
    return 0;
}

static int dl_unlink(DList *l, DNode *n)
{
    n->prev->next = n->next;
    n->next->prev = n->prev;
    int val = n->val;
    free(n);
    l->size--;
    return val;
}

/* 根据下标选择从前或从后走，最多走 size/2 步 */
static DNode *dl_node_at(const DList *l, size_t index)
{
    DNode *p;
    if (index < l->size / 2) {
        p = l->head.next;
        for (size_t i = 0; i < index; ++i)
            p = p->next;
    } else {
        p = (DNode *)&l->head;
        for (size_t i = l->size; i > index; --i)
            p = p->prev;
    }
    return p;
}

int dl_push_front(DList *l, int val)
{
    return dl_link_before(l, l->head.next, val);
}

int dl_push_back(DList *l, int val)
{
    return dl_link_before(l, &l->head, val);
}

int dl_pop_front(DList *l, int *out)
{
    if (l->size == 0)
        return -1;
    int v = dl_unlink(l, l->head.next);
    if (out)
        *out = v;
    return 0;
}

int dl_pop_back(DList *l, int *out)
{
    if (l->size == 0)
        return -1;
    int v = dl_unlink(l, l->head.prev);
    if (out)
        *out = v;
    return 0;
}

int dl_insert(DList *l, size_t index, int val)
{
    if (index > l->size)
        return -1;
    /* index == size 时 dl_node_at 返回哨兵，正好插在末尾 */
    return dl_link_before(l, dl_node_at(l, index), val);
}

int dl_erase(DList *l, size_t index)
{
    if (index >= l->size)
        return -1;
    dl_unlink(l, dl_node_at(l, index));
    return 0;
}

int dl_get(const DList *l, size_t index, int *out)
{
    if (index >= l->size)
        return -1;
    *out = dl_node_at(l, index)->val;
    return 0;
}

long dl_find(const DList *l, int val)
{
    long i = 0;
    for (const DNode *p = l->head.next; p != &l->head; p = p->next, ++i)
        if (p->val == val)
            return i;
    return -1;
}

void dl_reverse(DList *l)
{
    /* 交换每个结点（包括哨兵）的 prev 与 next */
    DNode *p = &l->head;
    do {
        DNode *t = p->next;
        p->next = p->prev;
        p->prev = t;
        p = t;
    } while (p != &l->head);
}

size_t dl_size(const DList *l)
{
    return l->size;
}

void dl_print(const DList *l)
{
    printf("HEAD <-> ");
    for (const DNode *p = l->head.next; p != &l->head; p = p->next)
        printf("%d <-> ", p->val);
    printf("HEAD\n");
}

void dl_print_reverse(const DList *l)
{
    printf("HEAD <-> ");
    for (const DNode *p = l->head.prev; p != &l->head; p = p->prev)
        printf("%d <-> ", p->val);
    printf("HEAD\n");
}

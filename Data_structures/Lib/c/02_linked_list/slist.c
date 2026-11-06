#include "slist.h"

#include <stdio.h>
#include <stdlib.h>

static SNode *snode_new(int val, SNode *next)
{
    SNode *n = (SNode *)malloc(sizeof(SNode));
    if (n) {
        n->val = val;
        n->next = next;
    }
    return n;
}

SList *sl_create(void)
{
    SList *l = (SList *)malloc(sizeof(SList));
    if (!l)
        return NULL;
    l->head.next = NULL;
    l->size = 0;
    return l;
}

void sl_destroy(SList *l)
{
    if (!l)
        return;
    snode_free_all(&l->head.next);
    free(l);
}

/* 返回第 index 个结点的前驱；index == 0 时返回哨兵 */
static SNode *sl_prev_of(SList *l, size_t index)
{
    SNode *p = &l->head;
    for (size_t i = 0; i < index; ++i)
        p = p->next;
    return p;
}

int sl_insert(SList *l, size_t index, int val)
{
    if (index > l->size)
        return -1;
    SNode *prev = sl_prev_of(l, index);
    SNode *n = snode_new(val, prev->next);
    if (!n)
        return -1;
    prev->next = n;
    l->size++;
    return 0;
}

int sl_push_front(SList *l, int val)
{
    return sl_insert(l, 0, val);
}

int sl_push_back(SList *l, int val)
{
    return sl_insert(l, l->size, val);
}

int sl_erase(SList *l, size_t index)
{
    if (index >= l->size)
        return -1;
    SNode *prev = sl_prev_of(l, index);
    SNode *victim = prev->next;
    prev->next = victim->next;
    free(victim);
    l->size--;
    return 0;
}

int sl_remove_value(SList *l, int val)
{
    for (SNode *prev = &l->head; prev->next; prev = prev->next) {
        if (prev->next->val == val) {
            SNode *victim = prev->next;
            prev->next = victim->next;
            free(victim);
            l->size--;
            return 1;
        }
    }
    return 0;
}

int sl_get(const SList *l, size_t index, int *out)
{
    if (index >= l->size)
        return -1;
    const SNode *p = l->head.next;
    for (size_t i = 0; i < index; ++i)
        p = p->next;
    *out = p->val;
    return 0;
}

long sl_find(const SList *l, int val)
{
    long i = 0;
    for (const SNode *p = l->head.next; p; p = p->next, ++i)
        if (p->val == val)
            return i;
    return -1;
}

size_t sl_size(const SList *l)
{
    return l->size;
}

void sl_print(const SList *l)
{
    for (const SNode *p = l->head.next; p; p = p->next)
        printf("%d -> ", p->val);
    printf("NULL\n");
}

void sl_reverse(SList *l)
{
    SNode *prev = NULL, *cur = l->head.next;
    while (cur) {
        SNode *next = cur->next;
        cur->next = prev;
        prev = cur;
        cur = next;
    }
    l->head.next = prev;
}

SNode *sl_middle(SList *l)
{
    SNode *slow = l->head.next, *fast = l->head.next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

void sl_merge_sorted(SList *dst, SList *src)
{
    SNode dummy;
    SNode *tail = &dummy;
    SNode *a = dst->head.next, *b = src->head.next;
    while (a && b) {
        if (a->val <= b->val) {
            tail->next = a;
            a = a->next;
        } else {
            tail->next = b;
            b = b->next;
        }
        tail = tail->next;
    }
    tail->next = a ? a : b;
    dst->head.next = dummy.next;
    dst->size += src->size;
    src->head.next = NULL;
    src->size = 0;
}

/* ---------- 二级指针版本 ---------- */

int snode_push_front(SNode **head, int val)
{
    SNode *n = snode_new(val, *head);
    if (!n)
        return -1;
    *head = n;
    return 0;
}

int snode_remove(SNode **head, int val)
{
    /* link 指向"指向当前结点的那个指针"，删除头结点也无需特判 */
    for (SNode **link = head; *link; link = &(*link)->next) {
        if ((*link)->val == val) {
            SNode *victim = *link;
            *link = victim->next;
            free(victim);
            return 1;
        }
    }
    return 0;
}

void snode_free_all(SNode **head)
{
    SNode *p = *head;
    while (p) {
        SNode *next = p->next;
        free(p);
        p = next;
    }
    *head = NULL;
}

int snode_has_cycle(const SNode *head)
{
    const SNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast)
            return 1;
    }
    return 0;
}

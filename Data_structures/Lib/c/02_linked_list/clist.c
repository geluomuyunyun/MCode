#include "clist.h"

#include <stdio.h>
#include <stdlib.h>

CList *cl_create(void)
{
    CList *l = (CList *)malloc(sizeof(CList));
    if (!l)
        return NULL;
    l->tail = NULL;
    l->size = 0;
    return l;
}

void cl_destroy(CList *l)
{
    if (!l)
        return;
    while (l->size > 0)
        cl_pop_front(l, NULL);
    free(l);
}

int cl_push_front(CList *l, int val)
{
    CNode *n = (CNode *)malloc(sizeof(CNode));
    if (!n)
        return -1;
    n->val = val;
    if (!l->tail) {
        n->next = n;
        l->tail = n;
    } else {
        n->next = l->tail->next;
        l->tail->next = n;
    }
    l->size++;
    return 0;
}

int cl_push_back(CList *l, int val)
{
    if (cl_push_front(l, val) != 0)
        return -1;
    /* 先插到头部，再把尾指针前移一格，新结点就成了尾 */
    l->tail = l->tail->next;
    return 0;
}

int cl_pop_front(CList *l, int *out)
{
    if (!l->tail)
        return -1;
    CNode *head = l->tail->next;
    if (out)
        *out = head->val;
    if (head == l->tail)
        l->tail = NULL;
    else
        l->tail->next = head->next;
    free(head);
    l->size--;
    return 0;
}

size_t cl_size(const CList *l)
{
    return l->size;
}

void cl_print(const CList *l)
{
    if (!l->tail) {
        printf("(empty)\n");
        return;
    }
    const CNode *p = l->tail->next;
    do {
        printf("%d -> ", p->val);
        p = p->next;
    } while (p != l->tail->next);
    printf("(back to %d)\n", l->tail->next->val);
}

int cl_josephus(int n, int k, int *order)
{
    if (n <= 0 || k <= 0)
        return -1;
    CList *l = cl_create();
    if (!l)
        return -1;
    for (int i = 1; i <= n; ++i) {
        if (cl_push_back(l, i) != 0) {
            cl_destroy(l);
            return -1;
        }
    }
    /* prev 始终指向"当前报数者"的前一个结点，初始为尾 */
    CNode *prev = l->tail;
    int out_idx = 0;
    while (l->size > 1) {
        for (int step = 1; step < k; ++step)
            prev = prev->next;
        CNode *victim = prev->next;
        if (order)
            order[out_idx++] = victim->val;
        prev->next = victim->next;
        if (victim == l->tail)
            l->tail = prev;
        free(victim);
        l->size--;
    }
    int last = l->tail->val;
    if (order)
        order[out_idx] = last;
    cl_destroy(l);
    return last;
}

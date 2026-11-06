#include "link_queue.h"

#include <stdlib.h>

LinkQueue *lq_create(void)
{
    LinkQueue *q = (LinkQueue *)malloc(sizeof(LinkQueue));
    if (!q)
        return NULL;
    q->front = q->rear = NULL;
    q->size = 0;
    return q;
}

void lq_destroy(LinkQueue *q)
{
    if (!q)
        return;
    while (lq_dequeue(q, NULL) == 0) {
    }
    free(q);
}

int lq_enqueue(LinkQueue *q, int val)
{
    LQNode *n = (LQNode *)malloc(sizeof(LQNode));
    if (!n)
        return -1;
    n->val = val;
    n->next = NULL;
    if (q->rear)
        q->rear->next = n;
    else
        q->front = n;
    q->rear = n;
    q->size++;
    return 0;
}

int lq_dequeue(LinkQueue *q, int *out)
{
    if (!q->front)
        return -1;
    LQNode *n = q->front;
    if (out)
        *out = n->val;
    q->front = n->next;
    if (!q->front)
        q->rear = NULL; /* 出队最后一个元素时 rear 也要置空 */
    free(n);
    q->size--;
    return 0;
}

int lq_front(const LinkQueue *q, int *out)
{
    if (!q->front)
        return -1;
    *out = q->front->val;
    return 0;
}

int lq_empty(const LinkQueue *q)
{
    return q->front == NULL;
}

size_t lq_size(const LinkQueue *q)
{
    return q->size;
}

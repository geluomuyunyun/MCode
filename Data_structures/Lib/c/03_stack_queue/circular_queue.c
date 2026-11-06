#include "circular_queue.h"

#include <stdlib.h>

CircularQueue *cq_create(size_t capacity)
{
    if (capacity == 0)
        return NULL;
    CircularQueue *q = (CircularQueue *)malloc(sizeof(CircularQueue));
    if (!q)
        return NULL;
    q->slots = capacity + 1;
    q->data = (int *)malloc(q->slots * sizeof(int));
    if (!q->data) {
        free(q);
        return NULL;
    }
    q->front = q->rear = 0;
    return q;
}

void cq_destroy(CircularQueue *q)
{
    if (!q)
        return;
    free(q->data);
    free(q);
}

int cq_enqueue(CircularQueue *q, int val)
{
    if (cq_full(q))
        return -1;
    q->data[q->rear] = val;
    q->rear = (q->rear + 1) % q->slots;
    return 0;
}

int cq_dequeue(CircularQueue *q, int *out)
{
    if (cq_empty(q))
        return -1;
    if (out)
        *out = q->data[q->front];
    q->front = (q->front + 1) % q->slots;
    return 0;
}

int cq_front(const CircularQueue *q, int *out)
{
    if (cq_empty(q))
        return -1;
    *out = q->data[q->front];
    return 0;
}

int cq_empty(const CircularQueue *q)
{
    return q->front == q->rear;
}

int cq_full(const CircularQueue *q)
{
    return (q->rear + 1) % q->slots == q->front;
}

size_t cq_size(const CircularQueue *q)
{
    return (q->rear + q->slots - q->front) % q->slots;
}

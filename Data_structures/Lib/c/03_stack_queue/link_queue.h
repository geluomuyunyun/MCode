/*
 * link_queue.h - 链式队列。队头出队、队尾入队，保存 front 与 rear 两个指针。
 */
#ifndef DSA_C_LINK_QUEUE_H
#define DSA_C_LINK_QUEUE_H

#include <stddef.h>

typedef struct LQNode {
    int val;
    struct LQNode *next;
} LQNode;

typedef struct {
    LQNode *front;
    LQNode *rear;
    size_t size;
} LinkQueue;

LinkQueue *lq_create(void);
void lq_destroy(LinkQueue *q);
int lq_enqueue(LinkQueue *q, int val);
int lq_dequeue(LinkQueue *q, int *out);
int lq_front(const LinkQueue *q, int *out);
int lq_empty(const LinkQueue *q);
size_t lq_size(const LinkQueue *q);

#endif

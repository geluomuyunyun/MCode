/*
 * circular_queue.h - 固定容量的循环队列。
 *
 * 用"牺牲一个空位"的方法区分队空与队满：
 *   队空：front == rear
 *   队满：(rear + 1) % slots == front
 * 因此内部数组长度 slots = capacity + 1。
 */
#ifndef DSA_C_CIRCULAR_QUEUE_H
#define DSA_C_CIRCULAR_QUEUE_H

#include <stddef.h>

typedef struct {
    int *data;
    size_t slots; /* capacity + 1 */
    size_t front; /* 队头元素下标 */
    size_t rear;  /* 下一个入队位置 */
} CircularQueue;

CircularQueue *cq_create(size_t capacity);
void cq_destroy(CircularQueue *q);
int cq_enqueue(CircularQueue *q, int val); /* 队满返回 -1 */
int cq_dequeue(CircularQueue *q, int *out);
int cq_front(const CircularQueue *q, int *out);
int cq_empty(const CircularQueue *q);
int cq_full(const CircularQueue *q);
size_t cq_size(const CircularQueue *q);

#endif

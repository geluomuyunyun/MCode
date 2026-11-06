/*
 * clist.h - 单向循环链表，只保存尾指针。
 *
 * tail->next 就是头结点，所以只用一个指针就能 O(1) 地在头尾插入。
 * 经典应用：约瑟夫环问题。
 */
#ifndef DSA_C_CLIST_H
#define DSA_C_CLIST_H

#include <stddef.h>

typedef struct CNode {
    int val;
    struct CNode *next;
} CNode;

typedef struct {
    CNode *tail; /* 空表为 NULL */
    size_t size;
} CList;

CList *cl_create(void);
void cl_destroy(CList *l);

int cl_push_front(CList *l, int val);
int cl_push_back(CList *l, int val);
int cl_pop_front(CList *l, int *out);
size_t cl_size(const CList *l);
void cl_print(const CList *l);

/*
 * 约瑟夫环：n 个人编号 1..n 围成一圈，从 1 号开始报数，报到 k 的人出列。
 * 返回最后剩下的人的编号；order 不为 NULL 时依次写入出列顺序（长度 n）。
 * 出错返回 -1。
 */
int cl_josephus(int n, int k, int *order);

#endif

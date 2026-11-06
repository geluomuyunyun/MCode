/*
 * slist.h - 单链表。
 *
 * 两套接口：
 *   1. SList：带头结点（哨兵）的单链表。头结点让"在第一个位置插入/删除"
 *      不再是特殊情况。
 *   2. snode_*：不带头结点的裸链表，用二级指针 SNode ** 修改头指针，
 *      专门用来练习二级指针。
 *
 * 附带算法：反转、快慢指针找中点、Floyd 判环、合并两个有序链表。
 */
#ifndef DSA_C_SLIST_H
#define DSA_C_SLIST_H

#include <stddef.h>

typedef struct SNode {
    int val;
    struct SNode *next;
} SNode;

typedef struct {
    SNode head; /* 哨兵，head.next 才是第一个元素 */
    size_t size;
} SList;

SList *sl_create(void);
void sl_destroy(SList *l);

int sl_push_front(SList *l, int val);
int sl_push_back(SList *l, int val);
int sl_insert(SList *l, size_t index, int val);
int sl_erase(SList *l, size_t index);
int sl_remove_value(SList *l, int val); /* 删除第一个等于 val 的结点，删除了返回 1 */
int sl_get(const SList *l, size_t index, int *out);
long sl_find(const SList *l, int val);
size_t sl_size(const SList *l);
void sl_print(const SList *l);

void sl_reverse(SList *l);
SNode *sl_middle(SList *l); /* 偶数个结点时返回后半段的第一个 */
/* 把有序链表 src 的所有结点并入有序链表 dst，src 变为空 */
void sl_merge_sorted(SList *dst, SList *src);

/* ---------- 不带头结点的裸链表（二级指针练习） ---------- */
int snode_push_front(SNode **head, int val);
int snode_remove(SNode **head, int val); /* 删除第一个等于 val 的结点 */
void snode_free_all(SNode **head);
int snode_has_cycle(const SNode *head);  /* Floyd 龟兔赛跑判环 */

#endif

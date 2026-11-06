/*
 * dlist.h - 带哨兵的双向循环链表。
 *
 * 空表时 head.next == head.prev == &head。
 * 哨兵 + 循环 使得任何位置的插入/删除都是同一套代码，没有 NULL 判断。
 */
#ifndef DSA_C_DLIST_H
#define DSA_C_DLIST_H

#include <stddef.h>

typedef struct DNode {
    int val;
    struct DNode *prev;
    struct DNode *next;
} DNode;

typedef struct {
    DNode head;
    size_t size;
} DList;

DList *dl_create(void);
void dl_destroy(DList *l);

int dl_push_front(DList *l, int val);
int dl_push_back(DList *l, int val);
int dl_pop_front(DList *l, int *out);
int dl_pop_back(DList *l, int *out);
int dl_insert(DList *l, size_t index, int val);
int dl_erase(DList *l, size_t index);
int dl_get(const DList *l, size_t index, int *out);
long dl_find(const DList *l, int val);
void dl_reverse(DList *l);
size_t dl_size(const DList *l);
void dl_print(const DList *l);
void dl_print_reverse(const DList *l);

#endif

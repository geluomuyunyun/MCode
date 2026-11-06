/*
 * link_stack.h - 链栈。栈顶就是链表头，push/pop 都是头插/头删。
 */
#ifndef DSA_C_LINK_STACK_H
#define DSA_C_LINK_STACK_H

#include <stddef.h>

typedef struct LSNode {
    int val;
    struct LSNode *next;
} LSNode;

typedef struct {
    LSNode *top;
    size_t size;
} LinkStack;

LinkStack *ls_create(void);
void ls_destroy(LinkStack *s);
int ls_push(LinkStack *s, int val);
int ls_pop(LinkStack *s, int *out);
int ls_peek(const LinkStack *s, int *out);
int ls_empty(const LinkStack *s);
size_t ls_size(const LinkStack *s);

#endif

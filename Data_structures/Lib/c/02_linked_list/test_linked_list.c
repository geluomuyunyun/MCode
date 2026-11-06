#include "check.h"
#include "clist.h"
#include "dlist.h"
#include "slist.h"

static void test_slist(void)
{
    TEST_SECTION("slist basic");
    SList *l = sl_create();
    for (int i = 1; i <= 5; ++i)
        sl_push_back(l, i);   /* 1 2 3 4 5 */
    sl_push_front(l, 0);      /* 0 1 2 3 4 5 */
    sl_insert(l, 3, 100);     /* 0 1 2 100 3 4 5 */
    int v;
    CHECK(sl_size(l) == 7);
    CHECK(sl_get(l, 3, &v) == 0 && v == 100);
    CHECK(sl_find(l, 5) == 6);
    CHECK(sl_remove_value(l, 100) == 1);
    CHECK(sl_remove_value(l, 100) == 0);
    CHECK(sl_erase(l, 0) == 0); /* 1 2 3 4 5 */
    CHECK(sl_middle(l)->val == 3);
    sl_print(l);

    TEST_SECTION("slist reverse");
    sl_reverse(l); /* 5 4 3 2 1 */
    CHECK(sl_get(l, 0, &v) == 0 && v == 5);
    CHECK(sl_get(l, 4, &v) == 0 && v == 1);
    sl_reverse(l);

    TEST_SECTION("slist merge sorted");
    SList *other = sl_create();
    sl_push_back(other, 0);
    sl_push_back(other, 3);
    sl_push_back(other, 9);
    sl_merge_sorted(l, other); /* 0 1 2 3 3 4 5 9 */
    CHECK(sl_size(l) == 8 && sl_size(other) == 0);
    int expect[] = {0, 1, 2, 3, 3, 4, 5, 9};
    for (size_t i = 0; i < 8; ++i)
        CHECK(sl_get(l, i, &v) == 0 && v == expect[i]);
    sl_destroy(other);
    sl_destroy(l);

    TEST_SECTION("raw list with double pointer");
    SNode *head = NULL;
    for (int i = 0; i < 4; ++i)
        snode_push_front(&head, i); /* 3 2 1 0 */
    CHECK(head->val == 3);
    CHECK(snode_remove(&head, 3) == 1); /* 删除头结点 */
    CHECK(head->val == 2);
    CHECK(snode_remove(&head, 0) == 1); /* 删除尾结点 */
    CHECK(snode_has_cycle(head) == 0);
    head->next->next = head; /* 人为制造环: 2 -> 1 -> 2 */
    CHECK(snode_has_cycle(head) == 1);
    head->next->next = NULL;
    snode_free_all(&head);
    CHECK(head == NULL);
}

static void test_dlist(void)
{
    TEST_SECTION("dlist");
    DList *l = dl_create();
    for (int i = 0; i < 6; ++i)
        dl_push_back(l, i); /* 0..5 */
    dl_push_front(l, -1);   /* -1 0 1 2 3 4 5 */
    int v;
    CHECK(dl_get(l, 5, &v) == 0 && v == 4); /* 走后半段 */
    CHECK(dl_insert(l, 7, 6) == 0);         /* 插在末尾 */
    CHECK(dl_pop_back(l, &v) == 0 && v == 6);
    CHECK(dl_pop_front(l, &v) == 0 && v == -1);
    CHECK(dl_erase(l, 2) == 0); /* 0 1 3 4 5 */
    CHECK(dl_find(l, 3) == 2);
    dl_reverse(l);              /* 5 4 3 1 0 */
    CHECK(dl_get(l, 0, &v) == 0 && v == 5);
    CHECK(dl_get(l, 4, &v) == 0 && v == 0);
    CHECK(dl_size(l) == 5);
    dl_print(l);
    dl_print_reverse(l);
    while (dl_pop_front(l, NULL) == 0) {
    }
    CHECK(dl_size(l) == 0);
    dl_destroy(l);
}

static void test_clist(void)
{
    TEST_SECTION("circular list / josephus");
    CList *l = cl_create();
    cl_push_back(l, 2);
    cl_push_back(l, 3);
    cl_push_front(l, 1);
    int v;
    CHECK(cl_size(l) == 3);
    cl_print(l);
    CHECK(cl_pop_front(l, &v) == 0 && v == 1);
    cl_destroy(l);

    int order[7];
    CHECK(cl_josephus(7, 3, order) == 4);
    int expect[] = {3, 6, 2, 7, 5, 1, 4};
    for (int i = 0; i < 7; ++i)
        CHECK(order[i] == expect[i]);
    CHECK(cl_josephus(1, 5, NULL) == 1);
    CHECK(cl_josephus(41, 3, NULL) == 31);
}

int main(void)
{
    test_slist();
    test_dlist();
    test_clist();
    return CHECK_REPORT();
}

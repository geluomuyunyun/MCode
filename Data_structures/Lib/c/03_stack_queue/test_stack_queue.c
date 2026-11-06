#include <string.h>

#include "array_stack.h"
#include "check.h"
#include "circular_queue.h"
#include "link_queue.h"
#include "link_stack.h"
#include "stack_apps.h"

static void test_stacks(void)
{
    TEST_SECTION("array stack");
    ArrayStack *as = as_create(1);
    for (int i = 0; i < 50; ++i)
        as_push(as, i);
    int v;
    CHECK(as_size(as) == 50);
    CHECK(as_peek(as, &v) == 0 && v == 49);
    CHECK(as_pop(as, &v) == 0 && v == 49);
    while (as_pop(as, NULL) == 0) {
    }
    CHECK(as_empty(as));
    CHECK(as_pop(as, &v) == -1);
    as_destroy(as);

    TEST_SECTION("link stack");
    LinkStack *ls = ls_create();
    ls_push(ls, 1);
    ls_push(ls, 2);
    ls_push(ls, 3);
    CHECK(ls_size(ls) == 3);
    CHECK(ls_pop(ls, &v) == 0 && v == 3);
    CHECK(ls_peek(ls, &v) == 0 && v == 2);
    ls_destroy(ls);
}

static void test_queues(void)
{
    TEST_SECTION("circular queue");
    CircularQueue *cq = cq_create(3);
    int v;
    CHECK(cq_empty(cq));
    CHECK(cq_enqueue(cq, 1) == 0);
    CHECK(cq_enqueue(cq, 2) == 0);
    CHECK(cq_enqueue(cq, 3) == 0);
    CHECK(cq_full(cq));
    CHECK(cq_enqueue(cq, 4) == -1);
    CHECK(cq_dequeue(cq, &v) == 0 && v == 1);
    CHECK(cq_enqueue(cq, 4) == 0); /* rear 绕回数组开头 */
    CHECK(cq_size(cq) == 3);
    int expect[] = {2, 3, 4};
    for (int i = 0; i < 3; ++i)
        CHECK(cq_dequeue(cq, &v) == 0 && v == expect[i]);
    CHECK(cq_empty(cq));
    cq_destroy(cq);

    TEST_SECTION("link queue");
    LinkQueue *lq = lq_create();
    for (int i = 0; i < 5; ++i)
        lq_enqueue(lq, i);
    CHECK(lq_front(lq, &v) == 0 && v == 0);
    for (int i = 0; i < 5; ++i)
        CHECK(lq_dequeue(lq, &v) == 0 && v == i);
    CHECK(lq_empty(lq));
    lq_enqueue(lq, 9); /* 清空后再入队，检验 rear 的处理 */
    CHECK(lq_size(lq) == 1 && lq_front(lq, &v) == 0 && v == 9);
    lq_destroy(lq);
}

static void test_apps(void)
{
    TEST_SECTION("bracket match");
    CHECK(bracket_match("{[()()]}"));
    CHECK(bracket_match("a(b)c[d]{e}"));
    CHECK(bracket_match(""));
    CHECK(!bracket_match("(]"));
    CHECK(!bracket_match("(()"));
    CHECK(!bracket_match("())"));

    TEST_SECTION("infix -> postfix");
    char buf[128];
    CHECK(infix_to_postfix("3 + 4 * 2", buf, sizeof buf) == 0 && strcmp(buf, "3 4 2 * +") == 0);
    CHECK(infix_to_postfix("(1+2)*(3-4)", buf, sizeof buf) == 0 && strcmp(buf, "1 2 + 3 4 - *") == 0);
    CHECK(infix_to_postfix("10 - 4 - 3", buf, sizeof buf) == 0 && strcmp(buf, "10 4 - 3 -") == 0);
    CHECK(infix_to_postfix("(1+2", buf, sizeof buf) == -1);
    CHECK(infix_to_postfix("1+", buf, sizeof buf) == -1);
    CHECK(infix_to_postfix("1 2", buf, sizeof buf) == -1);

    TEST_SECTION("evaluate");
    int r;
    CHECK(eval_postfix("3 4 2 * +", &r) == 0 && r == 11);
    CHECK(eval_infix("2 * (3 + 4) - 10 / 3", &r) == 0 && r == 11);
    CHECK(eval_infix("((15))", &r) == 0 && r == 15);
    CHECK(eval_infix("100 / (5 - 5)", &r) == -1);
    CHECK(eval_postfix("1 +", &r) == -1);
}

int main(void)
{
    test_stacks();
    test_queues();
    test_apps();
    return CHECK_REPORT();
}

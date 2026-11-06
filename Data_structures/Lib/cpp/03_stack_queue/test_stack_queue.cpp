#include <string>
#include <vector>

#include "../02_list/List.h"
#include "MonotonicStack.h"
#include "Queue.h"
#include "Stack.h"
#include "check.h"

int main()
{
    TEST_SECTION("Stack over Vector");
    Stack<int> s;
    for (int i = 0; i < 10; ++i)
        s.push(i);
    CHECK(s.size() == 10 && s.top() == 9);
    s.pop();
    CHECK(s.top() == 8);
    bool threw = false;
    try {
        Stack<int> e;
        e.pop();
    } catch (const std::underflow_error&) {
        threw = true;
    }
    CHECK(threw);

    TEST_SECTION("Stack over List (swap the container)");
    Stack<std::string, List<std::string>> ls;
    ls.push("a");
    ls.emplace(3, 'b');
    CHECK(ls.top() == "bbb" && ls.size() == 2);
    ls.pop();
    CHECK(ls.top() == "a");

    TEST_SECTION("Queue ring buffer with growth");
    Queue<int> q;
    for (int i = 0; i < 3; ++i)
        q.push(i);
    q.pop();       // head 前移，制造"绕回"
    q.push(3);
    q.push(4);     // 此时 size == cap == 4
    q.push(5);     // 触发扩容，元素需要按逻辑顺序搬移
    CHECK(q.size() == 5 && q.front() == 1 && q.back() == 5);
    std::vector<int> out;
    Queue<int> copy = q;
    while (!q.empty()) {
        out.push_back(q.front());
        q.pop();
    }
    CHECK(out == (std::vector<int>{1, 2, 3, 4, 5}));
    CHECK(copy.size() == 5 && copy.front() == 1);
    Queue<int> moved = std::move(copy);
    CHECK(moved.size() == 5 && copy.empty());

    TEST_SECTION("monotonic stack");
    CHECK(mono::next_greater_index({2, 1, 2, 4, 3}) == (std::vector<int>{3, 2, 3, -1, -1}));
    CHECK(mono::largest_rectangle({2, 1, 5, 6, 2, 3}) == 10);
    CHECK(mono::largest_rectangle({}) == 0);
    CHECK(mono::largest_rectangle({4, 4, 4}) == 12);

    return CHECK_REPORT();
}

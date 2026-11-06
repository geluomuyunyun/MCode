#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <string>
#include <vector>

#include "ForwardList.h"
#include "List.h"
#include "check.h"

template <typename C>
static std::vector<typename C::iterator::value_type> to_vec(const C& c)
{
    return std::vector<typename C::iterator::value_type>(c.begin(), c.end());
}

static void test_list()
{
    TEST_SECTION("List push / pop / iterate");
    List<int> l{1, 2, 3};
    l.push_front(0);
    l.push_back(4);
    CHECK(l.size() == 5 && l.front() == 0 && l.back() == 4);
    CHECK(to_vec(l) == (std::vector<int>{0, 1, 2, 3, 4}));
    l.pop_front();
    l.pop_back();
    CHECK(to_vec(l) == (std::vector<int>{1, 2, 3}));

    TEST_SECTION("List insert / erase with iterators");
    auto it = std::find(l.begin(), l.end(), 2);
    CHECK(it != l.end());
    l.insert(it, 100); // 1 100 2 3
    it = l.erase(it);  // 1 100 3, it 指向 3
    CHECK(*it == 3);
    CHECK(to_vec(l) == (std::vector<int>{1, 100, 3}));
    CHECK(std::distance(l.begin(), l.end()) == 3);
    std::vector<int> backwards;
    for (auto r = l.end(); r != l.begin();)
        backwards.push_back(*--r);
    CHECK(backwards == (std::vector<int>{3, 100, 1}));

    TEST_SECTION("List reverse / remove / unique / sort");
    l.reverse();
    CHECK(to_vec(l) == (std::vector<int>{3, 100, 1}));
    List<int> d{5, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    CHECK(d.remove(5) == 4);
    d.sort();
    CHECK(to_vec(d) == (std::vector<int>{1, 1, 2, 3, 4, 6, 9}));
    d.unique();
    CHECK(to_vec(d) == (std::vector<int>{1, 2, 3, 4, 6, 9}));
    d.sort(std::greater<int>());
    CHECK(to_vec(d) == (std::vector<int>{9, 6, 4, 3, 2, 1}));

    List<int> big;
    std::srand(7);
    for (int i = 0; i < 2000; ++i)
        big.push_back(std::rand() % 1000);
    big.sort();
    CHECK(std::is_sorted(big.begin(), big.end()) && big.size() == 2000);

    TEST_SECTION("List stable sort");
    List<std::pair<int, int>> st{{2, 0}, {1, 1}, {2, 2}, {1, 3}};
    st.sort([](const auto& a, const auto& b) { return a.first < b.first; });
    auto sv = to_vec(st);
    CHECK(sv[0].second == 1 && sv[1].second == 3 && sv[2].second == 0 && sv[3].second == 2);

    TEST_SECTION("List copy / move");
    List<std::string> a{"x", "y"};
    List<std::string> b = a;
    b.push_back("z");
    CHECK(a.size() == 2 && b.size() == 3);
    List<std::string> c = std::move(b);
    CHECK(b.empty() && c.size() == 3 && c.back() == "z");
    b = c;
    CHECK(to_vec(b) == to_vec(c));
    a = std::move(c);
    CHECK(a.size() == 3 && c.empty());
    c.push_back("still usable");
    CHECK(c.size() == 1);
}

static void test_forward_list()
{
    TEST_SECTION("ForwardList (unique_ptr nodes)");
    ForwardList<int> f{1, 2, 3};
    f.push_front(0);
    CHECK(f.size() == 4 && f.front() == 0);
    f.reverse();
    std::vector<int> out;
    f.for_each([&](int v) { out.push_back(v); });
    CHECK(out == (std::vector<int>{3, 2, 1, 0}));
    CHECK(f.remove_first(3) && f.front() == 2);
    CHECK(f.remove_first(0) && !f.remove_first(42));
    CHECK(f.size() == 2);

    ForwardList<int> g = f; // 深拷贝
    g.pop_front();
    CHECK(f.size() == 2 && g.size() == 1 && g.front() == 1);

    // 很长的链表也能安全析构（析构函数是循环而不是递归）
    ForwardList<int> longList;
    for (int i = 0; i < 1000000; ++i)
        longList.push_front(i);
    CHECK(longList.size() == 1000000);
}

int main()
{
    test_list();
    test_forward_list();
    return CHECK_REPORT();
}

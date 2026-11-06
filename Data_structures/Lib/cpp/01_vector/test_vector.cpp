#include <algorithm>
#include <numeric>
#include <string>
#include <vector>

#include "Vector.h"
#include "check.h"

// 统计构造/析构次数，用来验证没有泄漏对象，也没有重复析构
struct Tracker {
    static int alive;
    int v;
    Tracker(int x = 0) : v(x) { ++alive; }
    Tracker(const Tracker& o) : v(o.v) { ++alive; }
    Tracker(Tracker&& o) noexcept : v(o.v) { ++alive; }
    Tracker& operator=(const Tracker&) = default;
    Tracker& operator=(Tracker&&) noexcept = default;
    ~Tracker() { --alive; }
};
int Tracker::alive = 0;

int main()
{
    TEST_SECTION("push_back / grow / access");
    Vector<int> v;
    for (int i = 0; i < 100; ++i)
        v.push_back(i);
    CHECK(v.size() == 100 && v.capacity() >= 100);
    CHECK(v[42] == 42 && v.front() == 0 && v.back() == 99);
    bool threw = false;
    try {
        v.at(100);
    } catch (const std::out_of_range&) {
        threw = true;
    }
    CHECK(threw);

    TEST_SECTION("insert / erase");
    v.insert(0, -1);
    v.insert(v.size(), 100);
    v.insert(50, 777);
    CHECK(v.size() == 103 && v[0] == -1 && v[50] == 777 && v.back() == 100);
    v.erase(50);
    v.erase(0);
    CHECK(v.size() == 101 && v[0] == 0 && v[50] == 50);
    v.push_back(v[0]); // 引用自身元素
    CHECK(v.back() == 0);

    TEST_SECTION("iterators work with <algorithm>");
    std::reverse(v.begin(), v.end());
    CHECK(v.front() == 0 && v[1] == 100);
    std::sort(v.begin(), v.end());
    CHECK(std::is_sorted(v.begin(), v.end()));
    long long sum = 0;
    for (int x : v)
        sum += x;
    CHECK(sum == std::accumulate(v.begin(), v.end(), 0LL));

    TEST_SECTION("copy / move semantics");
    Vector<std::string> a{"alpha", "beta", "gamma"};
    Vector<std::string> b = a; // 拷贝构造
    CHECK(a == b);
    b[0] = "changed";
    CHECK(a[0] == "alpha"); // 深拷贝
    Vector<std::string> c = std::move(b); // 移动构造
    CHECK(b.empty() && c.size() == 3 && c[0] == "changed");
    a = c; // 拷贝赋值
    CHECK(a == c);
    a = std::move(c); // 移动赋值
    CHECK(c.empty() && a[0] == "changed");
    a.emplace_back(3, 'x');
    CHECK(a.back() == "xxx");

    TEST_SECTION("resize / clear");
    Vector<int> r(3, 7);
    r.resize(5);
    CHECK(r.size() == 5 && r[2] == 7 && r[4] == 0);
    r.resize(1);
    CHECK(r.size() == 1);
    r.clear();
    CHECK(r.empty());

    TEST_SECTION("object lifetime");
    {
        Vector<Tracker> t;
        for (int i = 0; i < 50; ++i)
            t.emplace_back(i);
        t.insert(10, Tracker(-1));
        t.erase(0);
        t.pop_back();
        CHECK(Tracker::alive == 49);
        Vector<Tracker> copy = t;
        CHECK(Tracker::alive == 98);
    }
    CHECK(Tracker::alive == 0);

    return CHECK_REPORT();
}

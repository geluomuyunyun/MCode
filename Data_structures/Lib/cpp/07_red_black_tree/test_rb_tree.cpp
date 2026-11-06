#include <cmath>
#include <map>
#include <random>
#include <string>
#include <vector>

#include "RBTree.h"
#include "check.h"

int main()
{
    TEST_SECTION("insert / find / update");
    RBTree<int, std::string> t;
    CHECK(t.insert(2, "two"));
    CHECK(t.insert(1, "one"));
    CHECK(t.insert(3, "three"));
    CHECK(!t.insert(2, "TWO")); // 更新
    CHECK(t.size() == 3 && *t.find(2) == "TWO" && t.find(4) == nullptr);
    CHECK(t.is_valid());

    TEST_SECTION("sorted insert keeps height <= 2*log2(n+1)");
    RBTree<int, int> s;
    const int n = 10000;
    for (int i = 0; i < n; ++i)
        s.insert(i, i * i);
    CHECK(s.is_valid());
    CHECK(s.height() <= static_cast<int>(2 * std::log2(n + 1.0)));
    CHECK(*s.find(99) == 99 * 99);

    TEST_SECTION("erase");
    for (int i = 0; i < n; i += 2)
        CHECK(s.erase(i));
    CHECK(!s.erase(0));
    CHECK(s.size() == n / 2 && s.is_valid());
    CHECK(!s.contains(100) && s.contains(101));
    for (int i = 1; i < n; i += 2)
        s.erase(i);
    CHECK(s.empty() && s.is_valid());

    TEST_SECTION("randomized vs std::map");
    RBTree<int, int> r;
    std::map<int, int> ref;
    std::mt19937 rng(7);
    std::uniform_int_distribution<int> key(0, 3000);
    bool ok = true, valid = true;
    for (int i = 0; i < 30000; ++i) {
        int k = key(rng);
        if (rng() % 3 != 0) {
            bool fresh = ref.find(k) == ref.end();
            ref[k] = i;
            if (r.insert(k, i) != fresh) ok = false;
        } else {
            if (r.erase(k) != (ref.erase(k) == 1)) ok = false;
        }
        if (i % 1000 == 0 && !r.is_valid()) valid = false;
    }
    CHECK(ok && valid && r.is_valid());
    auto got = r.to_sorted_vector();
    CHECK(got == std::vector<std::pair<int, int>>(ref.begin(), ref.end()));

    return CHECK_REPORT();
}

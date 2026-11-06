#include <cmath>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "AVLTree.h"
#include "check.h"

int main()
{
    TEST_SECTION("sorted insert stays balanced");
    AVLTree<int> t;
    for (int i = 0; i < 1000; ++i)
        CHECK(t.insert(i));
    CHECK(t.size() == 1000 && t.is_valid());
    // AVL 高度上界约为 1.44 * log2(n)
    CHECK(t.height() <= static_cast<int>(1.45 * std::log2(1000.0)) + 1);
    CHECK(!t.insert(500));
    CHECK(t.contains(999) && !t.contains(1000));

    TEST_SECTION("all four rotation cases");
    {
        AVLTree<int> ll, rr, lr, rl;
        for (int k : {3, 2, 1}) ll.insert(k);
        for (int k : {1, 2, 3}) rr.insert(k);
        for (int k : {3, 1, 2}) lr.insert(k);
        for (int k : {1, 3, 2}) rl.insert(k);
        CHECK(ll.height() == 2 && rr.height() == 2 && lr.height() == 2 && rl.height() == 2);
        CHECK(ll.is_valid() && rr.is_valid() && lr.is_valid() && rl.is_valid());
    }

    TEST_SECTION("erase");
    for (int i = 0; i < 1000; i += 3)
        CHECK(t.erase(i));
    CHECK(!t.erase(0));
    CHECK(t.is_valid() && t.size() == 1000 - 334);

    TEST_SECTION("randomized vs std::set");
    AVLTree<int> a;
    std::set<int> ref;
    std::mt19937 rng(99);
    std::uniform_int_distribution<int> key(0, 2000);
    bool ok = true;
    for (int i = 0; i < 20000; ++i) {
        int k = key(rng);
        if (rng() % 2) {
            if (a.insert(k) != ref.insert(k).second) ok = false;
        } else {
            if (a.erase(k) != (ref.erase(k) == 1)) ok = false;
        }
    }
    CHECK(ok);
    CHECK(a.is_valid());
    CHECK(a.to_sorted_vector() == std::vector<int>(ref.begin(), ref.end()));

    TEST_SECTION("custom comparator");
    AVLTree<std::string, std::greater<std::string>> desc;
    for (const char* s : {"banana", "apple", "cherry"})
        desc.insert(s);
    CHECK(desc.to_sorted_vector() == (std::vector<std::string>{"cherry", "banana", "apple"}));

    return CHECK_REPORT();
}

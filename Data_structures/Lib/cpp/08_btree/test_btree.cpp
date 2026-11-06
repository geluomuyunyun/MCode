#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "BPlusTree.h"
#include "BTree.h"
#include "check.h"

static void test_btree()
{
    TEST_SECTION("B-tree insert / search");
    BTree<int, 2> t; // t=2 即 2-3-4 树，结点最多 3 个键，分裂最频繁，便于测试
    for (int i = 1; i <= 100; ++i)
        CHECK(t.insert(i));
    CHECK(!t.insert(50));
    CHECK(t.size() == 100 && t.is_valid());
    CHECK(t.contains(1) && t.contains(100) && !t.contains(0));
    CHECK(t.height() <= 7);

    TEST_SECTION("B-tree erase (borrow / merge / shrink)");
    for (int i = 1; i <= 100; i += 2)
        CHECK(t.erase(i));
    CHECK(!t.erase(1));
    CHECK(t.size() == 50 && t.is_valid());
    for (int i = 2; i <= 100; i += 2)
        CHECK(t.erase(i));
    CHECK(t.empty() && t.is_valid() && t.height() == 0);

    TEST_SECTION("B-tree randomized vs std::set");
    BTree<int, 3> r;
    std::set<int> ref;
    std::mt19937 rng(11);
    std::uniform_int_distribution<int> key(0, 2000);
    bool ok = true, valid = true;
    for (int i = 0; i < 20000; ++i) {
        int k = key(rng);
        if (rng() % 2) {
            if (r.insert(k) != ref.insert(k).second) ok = false;
        } else {
            if (r.erase(k) != (ref.erase(k) == 1)) ok = false;
        }
        if (i % 500 == 0 && !r.is_valid()) valid = false;
    }
    CHECK(ok && valid && r.is_valid());
    CHECK(r.to_sorted_vector() == std::vector<int>(ref.begin(), ref.end()));
}

static void test_bplus()
{
    TEST_SECTION("B+ tree insert / find");
    BPlusTree<int, std::string, 4> t;
    for (int i = 0; i < 50; ++i)
        CHECK(t.insert(i * 10, "v" + std::to_string(i * 10)));
    CHECK(t.size() == 50 && t.is_valid());
    CHECK(t.find(120) == std::string("v120"));
    CHECK(!t.find(125).has_value());
    CHECK(!t.insert(120, "updated") && *t.find(120) == "updated");

    TEST_SECTION("B+ tree range query via leaf links");
    auto r = t.range(95, 151);
    CHECK(r.size() == 6 && r.front().first == 100 && r.back().first == 150);
    CHECK(t.range(1000, 2000).empty());
    CHECK(t.range(-100, 5).size() == 1);
    CHECK(t.range(0, 490).size() == 50);

    TEST_SECTION("B+ tree erase");
    for (int i = 0; i < 50; i += 2)
        CHECK(t.erase(i * 10));
    CHECK(!t.erase(0));
    CHECK(t.size() == 25 && t.is_valid());
    CHECK(t.range(0, 100).size() == 5); // 10 30 50 70 90
    for (int i = 1; i < 50; i += 2)
        CHECK(t.erase(i * 10));
    CHECK(t.empty() && t.is_valid());

    TEST_SECTION("B+ tree randomized vs std::map");
    BPlusTree<int, int, 5> b;
    std::map<int, int> ref;
    std::mt19937 rng(5);
    std::uniform_int_distribution<int> key(0, 3000);
    bool ok = true, valid = true;
    for (int i = 0; i < 30000; ++i) {
        int k = key(rng);
        if (rng() % 3 != 0) {
            bool fresh = ref.find(k) == ref.end();
            ref[k] = i;
            if (b.insert(k, i) != fresh) ok = false;
        } else {
            if (b.erase(k) != (ref.erase(k) == 1)) ok = false;
        }
        if (i % 500 == 0 && !b.is_valid()) valid = false;
    }
    CHECK(ok && valid && b.is_valid());
    auto all = b.range(0, 3000);
    CHECK(all == std::vector<std::pair<int, int>>(ref.begin(), ref.end()));
    auto it = ref.lower_bound(1000);
    auto end = ref.upper_bound(1200);
    CHECK(b.range(1000, 1200) == std::vector<std::pair<int, int>>(it, end));
}

int main()
{
    test_btree();
    test_bplus();
    return CHECK_REPORT();
}

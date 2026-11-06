#include <random>
#include <string>
#include <unordered_map>

#include "HashMap.h"
#include "check.h"

struct Point {
    int x, y;
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }
};

struct PointHash {
    std::size_t operator()(const Point& p) const
    {
        return std::hash<long long>()((static_cast<long long>(p.x) << 32) ^ static_cast<unsigned>(p.y));
    }
};

int main()
{
    TEST_SECTION("basic operations");
    HashMap<std::string, int> m;
    CHECK(m.insert("one", 1));
    CHECK(!m.insert("one", 100));
    CHECK(m.at("one") == 1);
    CHECK(m.insert_or_assign("one", 11) == false && m.at("one") == 11);
    m["two"] = 2;
    m["three"] += 3; // operator[] 对不存在的键先插入 V()
    CHECK(m.size() == 3 && m["three"] == 3);
    CHECK(m.contains("two") && !m.contains("four"));
    CHECK(m.erase("two") && !m.erase("two"));
    CHECK(m.size() == 2 && m.find("two") == nullptr);

    TEST_SECTION("word count");
    HashMap<std::string, int> wc;
    const char* words[] = {"a", "b", "a", "c", "b", "a"};
    for (const char* w : words)
        ++wc[w];
    CHECK(wc["a"] == 3 && wc["b"] == 2 && wc["c"] == 1);
    int total = 0;
    wc.for_each([&](const std::string&, int c) { total += c; });
    CHECK(total == 6);

    TEST_SECTION("custom key type");
    HashMap<Point, std::string, PointHash> pm;
    pm[{1, 2}] = "a";
    pm[{2, 1}] = "b";
    CHECK(pm.size() == 2 && pm[{1, 2}] == "a" && pm[{2, 1}] == "b");

    TEST_SECTION("randomized vs std::unordered_map");
    HashMap<int, int> h;
    std::unordered_map<int, int> ref;
    std::mt19937 rng(2024);
    std::uniform_int_distribution<int> key(0, 5000), op(0, 2);
    bool consistent = true;
    for (int i = 0; i < 50000; ++i) {
        int k = key(rng);
        switch (op(rng)) {
        case 0:
            h.insert_or_assign(k, i);
            ref[k] = i;
            break;
        case 1:
            if (h.erase(k) != (ref.erase(k) == 1))
                consistent = false;
            break;
        default: {
            const int* v = h.find(k);
            auto it = ref.find(k);
            if ((v == nullptr) != (it == ref.end()) || (v && *v != it->second))
                consistent = false;
        }
        }
    }
    CHECK(consistent);
    CHECK(h.size() == ref.size());
    CHECK(h.load_factor() <= 1.0);
    CHECK(h.longest_chain() < 12);

    return CHECK_REPORT();
}

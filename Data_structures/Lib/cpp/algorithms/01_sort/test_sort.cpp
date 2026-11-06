#include <algorithm>
#include <climits>
#include <functional>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "Sort.h"
#include "check.h"

using Ints = std::vector<int>;
using SortFn = void (*)(Ints&);

static std::vector<Ints> make_cases()
{
    std::mt19937 rng(42);
    std::vector<Ints> cases = {
        {}, {1}, {2, 1}, {3, 3, 3}, {5, 4, 3, 2, 1}, {1, 2, 3, 4, 5},
        {0, -1, INT_MAX, INT_MIN, 7, -7},
    };
    for (int n : {17, 100, 1000}) {
        Ints r(n), dup(n);
        for (int i = 0; i < n; ++i) {
            r[i] = static_cast<int>(rng() % 20001) - 10000;
            dup[i] = static_cast<int>(rng() % 5); // 大量重复
        }
        cases.push_back(r);
        cases.push_back(dup);
        Ints sorted = r;
        std::sort(sorted.begin(), sorted.end());
        cases.push_back(sorted);
        std::reverse(sorted.begin(), sorted.end());
        cases.push_back(sorted);
    }
    return cases;
}

int main()
{
    const std::pair<const char*, SortFn> algos[] = {
        {"bubble", [](Ints& a) { sorting::bubble_sort(a); }},
        {"selection", [](Ints& a) { sorting::selection_sort(a); }},
        {"insertion", [](Ints& a) { sorting::insertion_sort(a); }},
        {"shell", [](Ints& a) { sorting::shell_sort(a); }},
        {"merge", [](Ints& a) { sorting::merge_sort(a); }},
        {"quick", [](Ints& a) { sorting::quick_sort(a); }},
        {"heap", [](Ints& a) { sorting::heap_sort(a); }},
        {"radix", sorting::radix_sort},
    };
    const auto cases = make_cases();

    for (const auto& [name, fn] : algos) {
        TEST_SECTION(name);
        for (const Ints& c : cases) {
            Ints got = c, want = c;
            fn(got);
            std::sort(want.begin(), want.end());
            CHECK(got == want);
        }
    }

    TEST_SECTION("counting (small value range)");
    Ints c{4, -2, 2, 8, 3, 3, 1, -2};
    sorting::counting_sort(c);
    CHECK(c == (Ints{-2, -2, 1, 2, 3, 3, 4, 8}));

    TEST_SECTION("custom comparator / strings");
    std::vector<std::string> s{"pear", "apple", "fig", "banana"};
    sorting::quick_sort(s, std::greater<std::string>());
    CHECK(s == (std::vector<std::string>{"pear", "fig", "banana", "apple"}));
    sorting::merge_sort(s);
    CHECK(std::is_sorted(s.begin(), s.end()));

    TEST_SECTION("stability (merge / insertion / bubble)");
    using P = std::pair<int, int>;
    std::vector<P> base{{3, 0}, {1, 1}, {3, 2}, {2, 3}, {1, 4}, {3, 5}};
    auto by_first = [](const P& a, const P& b) { return a.first < b.first; };
    std::vector<P> want{{1, 1}, {1, 4}, {2, 3}, {3, 0}, {3, 2}, {3, 5}};
    auto m = base, ins = base, bub = base;
    sorting::merge_sort(m, by_first);
    sorting::insertion_sort(ins, by_first);
    sorting::bubble_sort(bub, by_first);
    CHECK(m == want && ins == want && bub == want);

    return CHECK_REPORT();
}

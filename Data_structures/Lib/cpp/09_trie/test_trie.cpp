#include <string>
#include <vector>

#include "Trie.h"
#include "check.h"

using S = std::vector<std::string>;

int main()
{
    TEST_SECTION("insert / contains / starts_with");
    Trie t;
    for (const char* w : {"apple", "app", "application", "apt", "bat", "bath"})
        CHECK(t.insert(w));
    CHECK(!t.insert("app"));
    CHECK(t.size() == 6);
    CHECK(t.contains("app") && t.contains("apple") && !t.contains("ap"));
    CHECK(t.starts_with("ap") && t.starts_with("bat") && !t.starts_with("c"));

    TEST_SECTION("prefix queries");
    CHECK(t.count_prefix("ap") == 4);
    CHECK(t.count_prefix("app") == 3);
    CHECK(t.count_prefix("") == 6);
    CHECK(t.words_with_prefix("app") == (S{"app", "apple", "application"}));
    CHECK(t.words_with_prefix("ba") == (S{"bat", "bath"}));
    CHECK(t.words_with_prefix("zzz").empty());
    CHECK(t.longest_prefix_of("applesauce") == "apple");
    CHECK(t.longest_prefix_of("bathroom") == "bath");
    CHECK(t.longest_prefix_of("cat").empty());

    TEST_SECTION("longest common prefix");
    Trie f;
    for (const char* w : {"flower", "flow", "flight"})
        f.insert(w);
    CHECK(f.longest_common_prefix() == "fl");
    f.erase("flight");
    CHECK(f.longest_common_prefix() == "flow");

    TEST_SECTION("erase with pruning");
    CHECK(t.erase("application"));
    CHECK(!t.erase("application"));
    CHECK(!t.contains("application") && t.contains("apple"));
    CHECK(!t.starts_with("appl" "i"));
    CHECK(t.erase("app"));
    CHECK(!t.contains("app") && t.contains("apple") && t.starts_with("app"));
    CHECK(t.count_prefix("ap") == 2);
    CHECK(t.erase("bat") && t.contains("bath"));
    CHECK(t.size() == 3);

    return CHECK_REPORT();
}

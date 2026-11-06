#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "my_string.h"

int main(void)
{
    TEST_SECTION("basic string functions");
    CHECK(my_strlen("") == 0);
    CHECK(my_strlen("hello") == 5);

    char buf[32];
    my_strcpy(buf, "abc");
    CHECK(strcmp(buf, "abc") == 0);
    my_strcat(buf, "def");
    CHECK(strcmp(buf, "abcdef") == 0);

    char nbuf[6];
    my_strncpy(nbuf, "xy", 6);
    CHECK(nbuf[0] == 'x' && nbuf[1] == 'y' && nbuf[2] == '\0' && nbuf[5] == '\0');

    CHECK(my_strcmp("abc", "abc") == 0);
    CHECK(my_strcmp("abc", "abd") < 0);
    CHECK(my_strcmp("abcd", "abc") > 0);
    CHECK(my_strcmp("", "a") < 0);

    CHECK(strcmp(my_strchr("hello", 'l'), "llo") == 0);
    CHECK(my_strchr("hello", 'z') == NULL);

    char *d = my_strdup("copy me");
    CHECK(d && strcmp(d, "copy me") == 0);
    my_str_reverse(d);
    CHECK(strcmp(d, "em ypoc") == 0);
    free(d);

    TEST_SECTION("KMP next array");
    int next[7];
    kmp_build_next("ababaca", 7, next);
    int expect[] = {0, 0, 1, 2, 3, 0, 1};
    for (int i = 0; i < 7; ++i)
        CHECK(next[i] == expect[i]);

    TEST_SECTION("matching");
    const char *text = "bacbababaabcbab";
    CHECK(bf_search(text, "ababaa") == 4);
    CHECK(kmp_search(text, "ababaa") == 4);
    CHECK(kmp_search(text, "xyz") == -1);
    CHECK(bf_search(text, "xyz") == -1);
    CHECK(kmp_search("abc", "") == 0);

    size_t pos[8];
    size_t cnt = kmp_search_all("aaaaa", "aa", pos, 8);
    CHECK(cnt == 4);
    CHECK(pos[0] == 0 && pos[3] == 3);
    CHECK(kmp_search_all("abcabcabc", "abc", pos, 8) == 3 && pos[2] == 6);

    return CHECK_REPORT();
}

#include "dynamic_array_cpp.h"

#include <initializer_list>
#include <iostream>

/*
滑动窗口和双指针是算法，不是动态数组的增删改查。
动态数组提供的是连续内存，以及 O(1) 的 a[i]。
这两类技巧靠“下标只前进、每个位置最多被看常数次”把二重循环收成 O(n)。

滑动窗口是同向双指针的一种：
窗口是闭区间 [left, right]，两个端点都只往右走。
固定窗口每次右边进一个、左边出一个。
可变窗口先扩展 right，条件满足后再收缩 left。
收缩必须有单调性。元素全是正数时，窗口变短，和一定变小。

对向双指针从两端往中间走，常见前提是序列有序，或者两端具有对称含义。
同向双指针一个负责扫描，一个负责写入，用来原地压缩或分区。
*/

DynArray<int> ints(std::initializer_list<int> values) {
    DynArray<int> array(values.size());
    for (int value : values) {
        array.push_back(value);
    }
    return array;
}

DynArray<char> chars_of(const char *text) {
    DynArray<char> array;
    for (const char *p = text; *p != '\0'; ++p) {
        array.push_back(*p);
    }
    return array;
}

bool is_non_decreasing(const DynArray<int> &array) {
    for (std::size_t i = 1; i < array.size(); ++i) {
        if (array[i] < array[i - 1]) {
            return false;
        }
    }
    return true;
}

/* 固定窗口：求长度恰好为 k 的连续段的最大和。
   新窗口和 = 旧窗口和 + 新进入的 a[right] - 离开的 a[right - k]。 */
bool max_sum_fixed_window(const DynArray<int> &array, std::size_t k, long long &best) {
    if (k == 0 || k > array.size()) {
        return false;
    }

    long long window = 0;
    for (std::size_t i = 0; i < k; ++i) {
        window += array[i];
    }
    best = window;

    for (std::size_t right = k; right < array.size(); ++right) {
        window += array[right];
        window -= array[right - k];
        if (window > best) {
            best = window;
        }
    }
    return true;
}

/* 可变窗口：元素必须全是正数。求和至少为 target 的最短连续段长度。
   right 每前进一步，和变大；和已经够了，就尽量把 left 往右收。 */
bool min_positive_subarray_len(const DynArray<int> &array, long long target,
                                std::size_t &best_len) {
    if (target <= 0) {
        return false;
    }
    for (std::size_t i = 0; i < array.size(); ++i) {
        if (array[i] <= 0) {
            return false;
        }
    }

    std::size_t left = 0;
    long long sum = 0;
    bool found = false;
    best_len = 0;

    for (std::size_t right = 0; right < array.size(); ++right) {
        sum += array[right];
        while (sum >= target) {
            std::size_t len = right - left + 1;
            if (!found || len < best_len) {
                best_len = len;
                found = true;
            }
            sum -= array[left];
            ++left;
        }
    }
    return found;
}

/* 对向双指针：升序数组里找两个不同位置，值的和等于 target。
   和偏小就移动左端，和偏大就移动右端。两端相向移动，最多 n 步。 */
bool two_sum_sorted(const DynArray<int> &array, int target,
                    std::size_t &left_out, std::size_t &right_out) {
    if (array.size() < 2 || !is_non_decreasing(array)) {
        return false;
    }

    std::size_t left = 0;
    std::size_t right = array.size() - 1;
    while (left < right) {
        long long sum = static_cast<long long>(array[left]) + array[right];
        if (sum == target) {
            left_out = left;
            right_out = right;
            return true;
        }
        if (sum < target) {
            ++left;
        } else {
            --right;
        }
    }
    return false;
}

/* 同向双指针：slow 是下一个非零要放的位置，fast 负责扫描。
   非零的相对顺序保持不变。 */
void move_zeros(DynArray<int> &array) {
    std::size_t slow = 0;
    for (std::size_t fast = 0; fast < array.size(); ++fast) {
        if (array[fast] != 0) {
            if (slow != fast) {
                int tmp = array[slow];
                array[slow] = array[fast];
                array[fast] = tmp;
            }
            ++slow;
        }
    }
}

/* 同向双指针：有序数组原地去重。slow 指向当前保留段的最后一个。
   fast 看到新值时，slow 前进一步并写入。返回 false 时数组不会被修改。 */
bool remove_duplicates_sorted(DynArray<int> &array, std::size_t &new_size) {
    if (!is_non_decreasing(array)) {
        return false;
    }
    if (array.size() == 0) {
        new_size = 0;
        return true;
    }

    std::size_t slow = 0;
    for (std::size_t fast = 1; fast < array.size(); ++fast) {
        if (array[fast] != array[slow]) {
            ++slow;
            array[slow] = array[fast];
        }
    }
    array.truncate(slow + 1);
    new_size = array.size();
    return true;
}

/* 带状态的滑动窗口：最长无重复字符子串。
   不变量是 s[left, right] 里的字符互不相同。
   新字符如果已经出现在窗口内，left 直接跳到上次出现位置的后面。
   left 只增大，所以仍然是 O(n)。 */
bool longest_unique_substring(const DynArray<char> &text, std::size_t &best_len,
                              std::size_t &best_left, std::size_t &best_right) {
    if (text.size() == 0) {
        return false;
    }

    bool used[256] = {};
    std::size_t last[256] = {};
    std::size_t left = 0;
    best_len = 0;

    for (std::size_t right = 0; right < text.size(); ++right) {
        unsigned char ch = static_cast<unsigned char>(text[right]);
        if (used[ch] && last[ch] >= left) {
            left = last[ch] + 1;
        }
        used[ch] = true;
        last[ch] = right;

        std::size_t len = right - left + 1;
        if (len > best_len) {
            best_len = len;
            best_left = left;
            best_right = right;
        }
    }
    return true;
}

static int check(bool condition, const char *name) {
    if (!condition) {
        std::cerr << "FAILED " << name << "\n";
        return 1;
    }
    std::cout << "ok " << name << "\n";
    return 0;
}

int main() {
    int failed = 0;

    DynArray<int> window_data = ints({2, 1, 5, 1, 3, 2});
    long long best_sum = 0;
    failed += check(max_sum_fixed_window(window_data, 3, best_sum) && best_sum == 9,
                    "fixed window [2,1,5,1,3,2] k=3 -> 9");

    DynArray<int> positive = ints({2, 3, 1, 2, 4, 3});
    std::size_t best_len = 0;
    failed += check(min_positive_subarray_len(positive, 7, best_len) && best_len == 2,
                    "variable window sum>=7 -> length 2");

    DynArray<int> sorted = ints({1, 2, 4, 6, 10});
    std::size_t left = 0;
    std::size_t right = 0;
    failed += check(two_sum_sorted(sorted, 8, left, right) && left == 1 && right == 3,
                    "two pointers 2+6=8");

    DynArray<int> zeros = ints({0, 1, 0, 3, 12});
    move_zeros(zeros);
    failed += check(zeros.size() == 5 && zeros[0] == 1 && zeros[1] == 3 &&
                        zeros[2] == 12 && zeros[3] == 0 && zeros[4] == 0,
                    "move zeros -> [1,3,12,0,0]");

    DynArray<int> duplicates = ints({0, 0, 1, 1, 1, 2, 2, 3, 3, 4});
    std::size_t new_size = 0;
    bool deduped = remove_duplicates_sorted(duplicates, new_size);
    failed += check(deduped && new_size == 5 && duplicates[0] == 0 &&
                        duplicates[1] == 1 && duplicates[2] == 2 &&
                        duplicates[3] == 3 && duplicates[4] == 4,
                    "dedup sorted -> [0,1,2,3,4]");

    DynArray<char> text = chars_of("abcabcbb");
    std::size_t unique_len = 0;
    std::size_t unique_left = 0;
    std::size_t unique_right = 0;
    bool found = longest_unique_substring(text, unique_len, unique_left, unique_right);
    failed += check(found && unique_len == 3 && unique_left == 0 && unique_right == 2,
                    "longest unique substring abcabcbb -> abc");

    return failed == 0 ? 0 : 1;
}

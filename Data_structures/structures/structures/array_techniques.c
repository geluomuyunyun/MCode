#include "dynamic_array_c.h"

#include <stdio.h>

/*
和 array_techniques.cpp 是同一组下标运动。
C 的动态数组元素是 int，所以这里没有字符窗口。
下标直接走 arr->data[i]，因为连续内存让相邻下标就是相邻元素。
*/

static int load_ints(DynArray *array, const int *values, size_t count) {
    size_t i;

    if (dyn_init(array, count) != DYN_OK) {
        return DYN_ERR;
    }
    for (i = 0; i < count; ++i) {
        if (dyn_push(array, values[i]) != DYN_OK) {
            return DYN_ERR;
        }
    }
    return DYN_OK;
}

static int is_non_decreasing(const DynArray *array) {
    size_t i;

    if (array == NULL) {
        return 0;
    }
    for (i = 1; i < array->size; ++i) {
        if (array->data[i] < array->data[i - 1]) {
            return 0;
        }
    }
    return 1;
}

/* 固定窗口。新和 = 旧和 + data[right] - data[right - k]。 */
int max_sum_fixed_window(const DynArray *array, size_t k, long long *best) {
    size_t i;
    long long window;

    if (array == NULL || best == NULL || k == 0 || k > array->size) {
        return DYN_ERR;
    }

    window = 0;
    for (i = 0; i < k; ++i) {
        window += array->data[i];
    }
    *best = window;

    for (i = k; i < array->size; ++i) {
        window += array->data[i];
        window -= array->data[i - k];
        if (window > *best) {
            *best = window;
        }
    }
    return DYN_OK;
}

/* 可变窗口。调用前元素必须全是正数，target 必须是正数。 */
int min_positive_subarray_len(const DynArray *array, long long target, size_t *best_len) {
    size_t left;
    size_t right;
    long long sum;
    int found;

    if (array == NULL || best_len == NULL || target <= 0) {
        return DYN_ERR;
    }
    for (right = 0; right < array->size; ++right) {
        if (array->data[right] <= 0) {
            return DYN_ERR;
        }
    }

    left = 0;
    sum = 0;
    found = 0;
    *best_len = 0;
    for (right = 0; right < array->size; ++right) {
        sum += array->data[right];
        while (sum >= target) {
            size_t len = right - left + 1;
            if (!found || len < *best_len) {
                *best_len = len;
                found = 1;
            }
            sum -= array->data[left];
            ++left;
        }
    }
    return found ? DYN_OK : DYN_ERR;
}

/* 对向双指针。数组必须升序。找到则把两个下标写回。 */
int two_sum_sorted(const DynArray *array, int target, size_t *left_out, size_t *right_out) {
    size_t left;
    size_t right;

    if (array == NULL || left_out == NULL || right_out == NULL ||
        array->size < 2 || !is_non_decreasing(array)) {
        return DYN_ERR;
    }

    left = 0;
    right = array->size - 1;
    while (left < right) {
        long long sum = (long long)array->data[left] + array->data[right];
        if (sum == target) {
            *left_out = left;
            *right_out = right;
            return DYN_OK;
        }
        if (sum < target) {
            ++left;
        } else {
            --right;
        }
    }
    return DYN_ERR;
}

/* 同向双指针。slow 写非零，fast 扫描。 */
void move_zeros(DynArray *array) {
    size_t slow;
    size_t fast;

    if (array == NULL) {
        return;
    }
    slow = 0;
    for (fast = 0; fast < array->size; ++fast) {
        if (array->data[fast] != 0) {
            if (slow != fast) {
                int tmp = array->data[slow];
                array->data[slow] = array->data[fast];
                array->data[fast] = tmp;
            }
            ++slow;
        }
    }
}

/* 同向双指针。有序数组原地去重，新的逻辑长度写到 new_size。 */
int remove_duplicates_sorted(DynArray *array, size_t *new_size) {
    size_t slow;
    size_t fast;

    if (array == NULL || new_size == NULL || !is_non_decreasing(array)) {
        return DYN_ERR;
    }
    if (array->size == 0) {
        *new_size = 0;
        return DYN_OK;
    }

    slow = 0;
    for (fast = 1; fast < array->size; ++fast) {
        if (array->data[fast] != array->data[slow]) {
            ++slow;
            array->data[slow] = array->data[fast];
        }
    }
    if (dyn_truncate(array, slow + 1) != DYN_OK) {
        return DYN_ERR;
    }
    *new_size = array->size;
    return DYN_OK;
}

static int check(int condition, const char *name) {
    if (!condition) {
        fprintf(stderr, "FAILED %s\n", name);
        return 1;
    }
    printf("ok %s\n", name);
    return 0;
}

int main(void) {
    int failed = 0;
    DynArray array;
    long long best_sum = 0;
    size_t best_len = 0;
    size_t left = 0;
    size_t right = 0;
    size_t new_size = 0;
    const int window_data[] = {2, 1, 5, 1, 3, 2};
    const int positive[] = {2, 3, 1, 2, 4, 3};
    const int sorted[] = {1, 2, 4, 6, 10};
    const int zeros[] = {0, 1, 0, 3, 12};
    const int duplicates[] = {0, 0, 1, 1, 1, 2, 2, 3, 3, 4};

    if (load_ints(&array, window_data, 6) != DYN_OK) {
        return 1;
    }
    failed += check(max_sum_fixed_window(&array, 3, &best_sum) == DYN_OK && best_sum == 9,
                    "fixed window [2,1,5,1,3,2] k=3 -> 9");
    dyn_free(&array);

    if (load_ints(&array, positive, 6) != DYN_OK) {
        return 1;
    }
    failed += check(min_positive_subarray_len(&array, 7, &best_len) == DYN_OK && best_len == 2,
                    "variable window sum>=7 -> length 2");
    dyn_free(&array);

    if (load_ints(&array, sorted, 5) != DYN_OK) {
        return 1;
    }
    failed += check(two_sum_sorted(&array, 8, &left, &right) == DYN_OK && left == 1 && right == 3,
                    "two pointers 2+6=8");
    dyn_free(&array);

    if (load_ints(&array, zeros, 5) != DYN_OK) {
        return 1;
    }
    move_zeros(&array);
    failed += check(array.size == 5 && array.data[0] == 1 && array.data[1] == 3 &&
                        array.data[2] == 12 && array.data[3] == 0 && array.data[4] == 0,
                    "move zeros -> [1,3,12,0,0]");
    dyn_free(&array);

    if (load_ints(&array, duplicates, 10) != DYN_OK) {
        return 1;
    }
    failed += check(remove_duplicates_sorted(&array, &new_size) == DYN_OK && new_size == 5 &&
                        array.data[0] == 0 && array.data[1] == 1 && array.data[2] == 2 &&
                        array.data[3] == 3 && array.data[4] == 4,
                    "dedup sorted -> [0,1,2,3,4]");
    dyn_free(&array);

    return failed == 0 ? 0 : 1;
}

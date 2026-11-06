#include <stdio.h>
#include <stdlib.h>

#define DYN_OK 0
#define DYN_ERR (-1)

typedef struct DynArray {
    int *data;
    size_t size;
    size_t capacity;
} DynArray;

static int dyn_reserve(DynArray *arr, size_t need) {
    size_t cap;
    int *next;

    if (arr == NULL) {
        return DYN_ERR;
    }
    if (need <= arr->capacity) {
        return DYN_OK;
    }

    cap = (arr->capacity == 0) ? 4 : arr->capacity;
    while (cap < need) {
        if (cap > ((size_t)-1) / 2) {
            cap = need;
            break;
        }
        cap *= 2;
    }

    next = (int *)realloc(arr->data, cap * sizeof(int));
    if (next == NULL) {
        return DYN_ERR;
    }
    arr->data = next;
    arr->capacity = cap;
    return DYN_OK;
}

int dyn_init(DynArray *arr, size_t capacity) {
    if (arr == NULL) {
        return DYN_ERR;
    }
    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
    if (capacity == 0) {
        return DYN_OK;
    }
    arr->data = (int *)malloc(capacity * sizeof(int));
    if (arr->data == NULL) {
        return DYN_ERR;
    }
    arr->capacity = capacity;
    return DYN_OK;
}

void dyn_free(DynArray *arr) {
    if (arr == NULL) {
        return;
    }
    free(arr->data);
    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
}

size_t dyn_size(const DynArray *arr) {
    return (arr == NULL) ? 0 : arr->size;
}

/* 逻辑长度收到 n。n 大于 size 时失败。 */
int dyn_truncate(DynArray *arr, size_t n) {
    if (arr == NULL || n > arr->size) {
        return DYN_ERR;
    }
    arr->size = n;
    return DYN_OK;
}

/* 增：尾部追加 */
int dyn_push(DynArray *arr, int value) {
    if (arr == NULL) {
        return DYN_ERR;
    }
    if (dyn_reserve(arr, arr->size + 1) != DYN_OK) {
        return DYN_ERR;
    }
    arr->data[arr->size++] = value;
    return DYN_OK;
}

/* 增：在 index 处插入。index == size 时等价于尾插 */
int dyn_insert(DynArray *arr, size_t index, int value) {
    size_t i;

    if (arr == NULL || index > arr->size) {
        return DYN_ERR;
    }
    if (dyn_reserve(arr, arr->size + 1) != DYN_OK) {
        return DYN_ERR;
    }
    for (i = arr->size; i > index; --i) {
        arr->data[i] = arr->data[i - 1];
    }
    arr->data[index] = value;
    arr->size++;
    return DYN_OK;
}

/* 删：按下标删除，后面的元素前移 */
int dyn_remove_at(DynArray *arr, size_t index) {
    size_t i;

    if (arr == NULL || index >= arr->size) {
        return DYN_ERR;
    }
    for (i = index + 1; i < arr->size; ++i) {
        arr->data[i - 1] = arr->data[i];
    }
    arr->size--;
    return DYN_OK;
}

/* 删：删除第一个等于 value 的元素 */
int dyn_remove_value(DynArray *arr, int value) {
    size_t i;

    if (arr == NULL) {
        return DYN_ERR;
    }
    for (i = 0; i < arr->size; ++i) {
        if (arr->data[i] == value) {
            return dyn_remove_at(arr, i);
        }
    }
    return DYN_ERR;
}

/* 改：把 index 位置改成 value */
int dyn_set(DynArray *arr, size_t index, int value) {
    if (arr == NULL || index >= arr->size) {
        return DYN_ERR;
    }
    arr->data[index] = value;
    return DYN_OK;
}

/* 查：按下标读取 */
int dyn_get(const DynArray *arr, size_t index, int *out) {
    if (arr == NULL || out == NULL || index >= arr->size) {
        return DYN_ERR;
    }
    *out = arr->data[index];
    return DYN_OK;
}

/* 查：按值查找第一个匹配下标 */
int dyn_find(const DynArray *arr, int value, size_t *index_out) {
    size_t i;

    if (arr == NULL || index_out == NULL) {
        return DYN_ERR;
    }
    for (i = 0; i < arr->size; ++i) {
        if (arr->data[i] == value) {
            *index_out = i;
            return DYN_OK;
        }
    }
    return DYN_ERR;
}

void dyn_print(const DynArray *arr) {
    size_t i;

    if (arr == NULL) {
        printf("(null)\n");
        return;
    }
    printf("size=%zu capacity=%zu [", arr->size, arr->capacity);
    for (i = 0; i < arr->size; ++i) {
        if (i > 0) {
            printf(", ");
        }
        printf("%d", arr->data[i]);
    }
    printf("]\n");
}


#if 0
int main(void) {
    DynArray arr;
    int value = 0;
    size_t index = 0;

    if (dyn_init(&arr, 2) != DYN_OK) {
        fprintf(stderr, "init failed\n");
        return 1;
    }

    dyn_push(&arr, 10);
    dyn_push(&arr, 20);
    dyn_push(&arr, 30);
    dyn_insert(&arr, 1, 15);
    printf("after insert: ");
    dyn_print(&arr);

    dyn_set(&arr, 2, 25);
    printf("after set:    ");
    dyn_print(&arr);

    if (dyn_get(&arr, 2, &value) == DYN_OK) {
        printf("get(2) = %d\n", value);
    }
    if (dyn_find(&arr, 15, &index) == DYN_OK) {
        printf("find(15) = %zu\n", index);
    }

    dyn_remove_value(&arr, 25);
    dyn_remove_at(&arr, 0);
    printf("after remove: ");
    dyn_print(&arr);

    dyn_free(&arr);
    return 0;
}
#endif

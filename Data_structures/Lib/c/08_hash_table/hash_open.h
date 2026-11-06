/*
 * hash_open.h - 开放寻址法（线性探测）哈希表，int -> int。
 *
 * 所有元素都存放在一个数组里，冲突时向后找下一个空槽。
 * 删除不能直接把槽置空（会切断探测链），而是标记为"墓碑"(DELETED)：
 *   查找时跳过墓碑继续探测；插入时可以复用墓碑。
 * (已用槽 + 墓碑) 超过容量的 70% 时扩容并清理墓碑。
 */
#ifndef DSA_C_HASH_OPEN_H
#define DSA_C_HASH_OPEN_H

#include <stddef.h>

typedef enum { SLOT_EMPTY = 0, SLOT_USED, SLOT_DELETED } SlotState;

typedef struct {
    int key;
    int value;
    SlotState state;
} HOSlot;

typedef struct {
    HOSlot *slots;
    size_t capacity;
    size_t size;      /* USED 槽的个数 */
    size_t tombstones; /* DELETED 槽的个数 */
} HashOpen;

HashOpen *ho_create(size_t capacity);
void ho_destroy(HashOpen *h);
int ho_put(HashOpen *h, int key, int value); /* 新插入返回 1，更新返回 0，失败返回 -1 */
int ho_get(const HashOpen *h, int key, int *out);
int ho_remove(HashOpen *h, int key);
size_t ho_size(const HashOpen *h);
size_t ho_capacity(const HashOpen *h);

#endif

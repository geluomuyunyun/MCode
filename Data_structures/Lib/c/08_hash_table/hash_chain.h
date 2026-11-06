/*
 * hash_chain.h - 链地址法（拉链法）哈希表，int -> int。
 *
 * 每个桶是一条单链表，冲突的键挂在同一条链上。
 * 负载因子 size / nbuckets 超过 0.75 时桶数翻倍并重新散列（rehash）。
 */
#ifndef DSA_C_HASH_CHAIN_H
#define DSA_C_HASH_CHAIN_H

#include <stddef.h>

typedef struct HCNode {
    int key;
    int value;
    struct HCNode *next;
} HCNode;

typedef struct {
    HCNode **buckets;
    size_t nbuckets;
    size_t size;
} HashChain;

HashChain *hc_create(size_t nbuckets);
void hc_destroy(HashChain *h);
int hc_put(HashChain *h, int key, int value); /* 新插入返回 1，更新返回 0，失败返回 -1 */
int hc_get(const HashChain *h, int key, int *out); /* 找到返回 1 */
int hc_remove(HashChain *h, int key);              /* 删除返回 1 */
size_t hc_size(const HashChain *h);
size_t hc_bucket_count(const HashChain *h);

#endif

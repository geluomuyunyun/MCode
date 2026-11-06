/*
 * hash_common.h - 两种哈希表共用的整数哈希函数。
 * 直接用 key % n 时，规律性的键（如都是偶数）会集中在少数桶里，
 * 先把比特位充分打散再取模。
 */
#ifndef DSA_C_HASH_COMMON_H
#define DSA_C_HASH_COMMON_H

#include <stddef.h>

static inline size_t hash_int(int key)
{
    unsigned int x = (unsigned int)key;
    x = ((x >> 16) ^ x) * 0x45d9f3bu;
    x = ((x >> 16) ^ x) * 0x45d9f3bu;
    x = (x >> 16) ^ x;
    return (size_t)x;
}

#endif

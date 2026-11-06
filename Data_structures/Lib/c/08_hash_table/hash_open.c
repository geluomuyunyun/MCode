#include "hash_open.h"

#include <stdlib.h>

#include "hash_common.h"

HashOpen *ho_create(size_t capacity)
{
    HashOpen *h = (HashOpen *)malloc(sizeof(HashOpen));
    if (!h)
        return NULL;
    h->capacity = capacity < 8 ? 8 : capacity;
    h->slots = (HOSlot *)calloc(h->capacity, sizeof(HOSlot)); /* calloc 置零即 SLOT_EMPTY */
    if (!h->slots) {
        free(h);
        return NULL;
    }
    h->size = h->tombstones = 0;
    return h;
}

void ho_destroy(HashOpen *h)
{
    if (!h)
        return;
    free(h->slots);
    free(h);
}

/* 返回 key 所在槽的下标；不存在返回 capacity */
static size_t find_slot(const HashOpen *h, int key)
{
    size_t i = hash_int(key) % h->capacity;
    for (size_t probes = 0; probes < h->capacity; ++probes) {
        const HOSlot *s = &h->slots[i];
        if (s->state == SLOT_EMPTY)
            return h->capacity;
        if (s->state == SLOT_USED && s->key == key)
            return i;
        i = (i + 1) % h->capacity;
    }
    return h->capacity;
}

static int resize(HashOpen *h, size_t new_capacity)
{
    HOSlot *old = h->slots;
    size_t old_cap = h->capacity;
    HOSlot *ns = (HOSlot *)calloc(new_capacity, sizeof(HOSlot));
    if (!ns)
        return -1;
    h->slots = ns;
    h->capacity = new_capacity;
    h->size = h->tombstones = 0;
    for (size_t i = 0; i < old_cap; ++i)
        if (old[i].state == SLOT_USED)
            ho_put(h, old[i].key, old[i].value);
    free(old);
    return 0;
}

int ho_put(HashOpen *h, int key, int value)
{
    size_t found = find_slot(h, key);
    if (found < h->capacity) {
        h->slots[found].value = value;
        return 0;
    }
    if ((h->size + h->tombstones + 1) * 10 > h->capacity * 7) {
        /* 墓碑很多时只需同容量重建，否则翻倍 */
        size_t new_cap = (h->size + 1) * 10 > h->capacity * 5 ? h->capacity * 2 : h->capacity;
        if (resize(h, new_cap) != 0)
            return -1;
    }
    size_t i = hash_int(key) % h->capacity;
    while (h->slots[i].state == SLOT_USED)
        i = (i + 1) % h->capacity;
    if (h->slots[i].state == SLOT_DELETED)
        h->tombstones--;
    h->slots[i].key = key;
    h->slots[i].value = value;
    h->slots[i].state = SLOT_USED;
    h->size++;
    return 1;
}

int ho_get(const HashOpen *h, int key, int *out)
{
    size_t i = find_slot(h, key);
    if (i == h->capacity)
        return 0;
    if (out)
        *out = h->slots[i].value;
    return 1;
}

int ho_remove(HashOpen *h, int key)
{
    size_t i = find_slot(h, key);
    if (i == h->capacity)
        return 0;
    h->slots[i].state = SLOT_DELETED;
    h->size--;
    h->tombstones++;
    return 1;
}

size_t ho_size(const HashOpen *h)
{
    return h->size;
}

size_t ho_capacity(const HashOpen *h)
{
    return h->capacity;
}

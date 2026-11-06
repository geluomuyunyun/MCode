#include "hash_chain.h"

#include <stdlib.h>

#include "hash_common.h"

HashChain *hc_create(size_t nbuckets)
{
    HashChain *h = (HashChain *)malloc(sizeof(HashChain));
    if (!h)
        return NULL;
    h->nbuckets = nbuckets ? nbuckets : 8;
    h->buckets = (HCNode **)calloc(h->nbuckets, sizeof(HCNode *));
    if (!h->buckets) {
        free(h);
        return NULL;
    }
    h->size = 0;
    return h;
}

void hc_destroy(HashChain *h)
{
    if (!h)
        return;
    for (size_t i = 0; i < h->nbuckets; ++i) {
        HCNode *p = h->buckets[i];
        while (p) {
            HCNode *next = p->next;
            free(p);
            p = next;
        }
    }
    free(h->buckets);
    free(h);
}

static int rehash(HashChain *h, size_t new_count)
{
    HCNode **nb = (HCNode **)calloc(new_count, sizeof(HCNode *));
    if (!nb)
        return -1;
    /* 结点直接搬到新桶，不需要重新分配 */
    for (size_t i = 0; i < h->nbuckets; ++i) {
        HCNode *p = h->buckets[i];
        while (p) {
            HCNode *next = p->next;
            size_t idx = hash_int(p->key) % new_count;
            p->next = nb[idx];
            nb[idx] = p;
            p = next;
        }
    }
    free(h->buckets);
    h->buckets = nb;
    h->nbuckets = new_count;
    return 0;
}

int hc_put(HashChain *h, int key, int value)
{
    size_t idx = hash_int(key) % h->nbuckets;
    for (HCNode *p = h->buckets[idx]; p; p = p->next) {
        if (p->key == key) {
            p->value = value;
            return 0;
        }
    }
    if ((h->size + 1) * 4 > h->nbuckets * 3) {
        if (rehash(h, h->nbuckets * 2) != 0)
            return -1;
        idx = hash_int(key) % h->nbuckets;
    }
    HCNode *n = (HCNode *)malloc(sizeof(HCNode));
    if (!n)
        return -1;
    n->key = key;
    n->value = value;
    n->next = h->buckets[idx];
    h->buckets[idx] = n;
    h->size++;
    return 1;
}

int hc_get(const HashChain *h, int key, int *out)
{
    size_t idx = hash_int(key) % h->nbuckets;
    for (const HCNode *p = h->buckets[idx]; p; p = p->next) {
        if (p->key == key) {
            if (out)
                *out = p->value;
            return 1;
        }
    }
    return 0;
}

int hc_remove(HashChain *h, int key)
{
    size_t idx = hash_int(key) % h->nbuckets;
    for (HCNode **link = &h->buckets[idx]; *link; link = &(*link)->next) {
        if ((*link)->key == key) {
            HCNode *victim = *link;
            *link = victim->next;
            free(victim);
            h->size--;
            return 1;
        }
    }
    return 0;
}

size_t hc_size(const HashChain *h)
{
    return h->size;
}

size_t hc_bucket_count(const HashChain *h)
{
    return h->nbuckets;
}

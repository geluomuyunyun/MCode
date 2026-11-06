// HashMap.h - 链地址法哈希表模板（对照 c/08_hash_table/hash_chain 与 std::unordered_map）。
//
// 桶数组用我们自己的 Vector，每个桶是一条 List<pair<K, V>>。
// Hash、KeyEqual 作为模板参数，自定义类型只要提供这两个函数对象就能当键。
#pragma once

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>

#include "../01_vector/Vector.h"
#include "../02_list/List.h"

template <typename K, typename V, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<K>>
class HashMap {
    using Entry = std::pair<K, V>;
    using Bucket = List<Entry>;

public:
    explicit HashMap(std::size_t bucket_count = 8) : buckets_(bucket_count ? bucket_count : 1) {}

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    std::size_t bucket_count() const noexcept { return buckets_.size(); }
    double load_factor() const noexcept { return static_cast<double>(size_) / buckets_.size(); }

    // 键已存在时不覆盖，返回 false
    bool insert(const K& key, const V& value)
    {
        if (find(key))
            return false;
        add_new(key, value);
        return true;
    }

    // 键存在则覆盖，不存在则插入；新插入返回 true
    bool insert_or_assign(const K& key, V value)
    {
        if (V* v = find(key)) {
            *v = std::move(value);
            return false;
        }
        add_new(key, std::move(value));
        return true;
    }

    V& operator[](const K& key)
    {
        if (V* v = find(key))
            return *v;
        return add_new(key, V());
    }

    V& at(const K& key)
    {
        if (V* v = find(key))
            return *v;
        throw std::out_of_range("HashMap::at");
    }

    V* find(const K& key)
    {
        for (Entry& e : buckets_[index(key)])
            if (eq_(e.first, key))
                return &e.second;
        return nullptr;
    }
    const V* find(const K& key) const { return const_cast<HashMap*>(this)->find(key); }
    bool contains(const K& key) const { return find(key) != nullptr; }

    bool erase(const K& key)
    {
        Bucket& b = buckets_[index(key)];
        for (auto it = b.begin(); it != b.end(); ++it) {
            if (eq_(it->first, key)) {
                b.erase(it);
                --size_;
                return true;
            }
        }
        return false;
    }

    void clear()
    {
        for (Bucket& b : buckets_)
            b.clear();
        size_ = 0;
    }

    void rehash(std::size_t new_count)
    {
        if (new_count == 0)
            new_count = 1;
        Vector<Bucket> nb(new_count);
        for (Bucket& b : buckets_)
            for (Entry& e : b)
                nb[hash_(e.first) % new_count].push_back(std::move(e));
        buckets_ = std::move(nb);
    }

    template <typename F>
    void for_each(F&& f) const
    {
        for (const Bucket& b : buckets_)
            for (const Entry& e : b)
                f(e.first, e.second);
    }

    // 最长的桶有多长，用来观察哈希分布
    std::size_t longest_chain() const
    {
        std::size_t m = 0;
        for (const Bucket& b : buckets_)
            m = b.size() > m ? b.size() : m;
        return m;
    }

private:
    static constexpr double kMaxLoad = 1.0;

    std::size_t index(const K& key) const { return hash_(key) % buckets_.size(); }

    V& add_new(const K& key, V value)
    {
        if (static_cast<double>(size_ + 1) / buckets_.size() > kMaxLoad)
            rehash(buckets_.size() * 2);
        Bucket& b = buckets_[index(key)];
        b.push_back(Entry(key, std::move(value)));
        ++size_;
        return b.back().second;
    }

    Vector<Bucket> buckets_;
    std::size_t size_ = 0;
    Hash hash_;
    KeyEqual eq_;
};

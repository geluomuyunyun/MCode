// List.h - 仿 std::list 的双向循环链表模板（带哨兵结点）。
//
// 学习要点（对照 c/02_linked_list/dlist）：
//   - 哨兵只需要 prev/next，不需要存值，所以拆成 NodeBase 与 Node 两层，
//     这样 T 没有默认构造函数也能用。
//   - 自定义双向迭代器：提供 iterator_category 等类型别名后，
//     就能和 std::find、std::distance 等标准算法配合。
//   - 附带算法：reverse、remove、unique、归并排序 sort。
#pragma once

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <utility>

template <typename T>
class List {
    struct NodeBase {
        NodeBase* prev;
        NodeBase* next;
    };
    struct Node : NodeBase {
        T value;
        template <typename... Args>
        explicit Node(Args&&... args) : NodeBase{nullptr, nullptr}, value(std::forward<Args>(args)...) {}
    };

public:
    template <bool IsConst>
    class Iter {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<IsConst, const T*, T*>;
        using reference = std::conditional_t<IsConst, const T&, T&>;

        Iter() = default;
        explicit Iter(NodeBase* n) : node_(n) {}
        // 允许 iterator 隐式转换为 const_iterator
        template <bool C = IsConst, typename = std::enable_if_t<C>>
        Iter(const Iter<false>& other) : node_(other.node_) {}

        reference operator*() const { return static_cast<Node*>(node_)->value; }
        pointer operator->() const { return &static_cast<Node*>(node_)->value; }
        Iter& operator++() { node_ = node_->next; return *this; }
        Iter operator++(int) { Iter t = *this; ++*this; return t; }
        Iter& operator--() { node_ = node_->prev; return *this; }
        Iter operator--(int) { Iter t = *this; --*this; return t; }
        friend bool operator==(const Iter& a, const Iter& b) { return a.node_ == b.node_; }
        friend bool operator!=(const Iter& a, const Iter& b) { return a.node_ != b.node_; }

    private:
        friend class List;
        template <bool>
        friend class Iter;
        NodeBase* node_ = nullptr;
    };
    using iterator = Iter<false>;
    using const_iterator = Iter<true>;

    List() { sentinel_.prev = sentinel_.next = &sentinel_; }
    List(std::initializer_list<T> init) : List()
    {
        for (const T& v : init)
            push_back(v);
    }
    List(const List& other) : List()
    {
        for (const T& v : other)
            push_back(v);
    }
    List(List&& other) noexcept : List() { take(other); }
    List& operator=(const List& other)
    {
        if (this != &other) {
            List tmp(other);
            swap(tmp);
        }
        return *this;
    }
    List& operator=(List&& other) noexcept
    {
        if (this != &other) {
            clear();
            take(other);
        }
        return *this;
    }
    ~List() { clear(); }

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

    iterator begin() noexcept { return iterator(sentinel_.next); }
    iterator end() noexcept { return iterator(&sentinel_); }
    const_iterator begin() const noexcept { return const_iterator(sentinel_.next); }
    const_iterator end() const noexcept { return const_iterator(const_cast<NodeBase*>(&sentinel_)); }

    T& front() { return *begin(); }
    T& back() { return *std::prev(end()); }
    const T& front() const { return *begin(); }
    const T& back() const { return *std::prev(end()); }

    void push_front(const T& v) { emplace(begin(), v); }
    void push_front(T&& v) { emplace(begin(), std::move(v)); }
    void push_back(const T& v) { emplace(end(), v); }
    void push_back(T&& v) { emplace(end(), std::move(v)); }
    template <typename... Args>
    T& emplace_back(Args&&... args) { return *emplace(end(), std::forward<Args>(args)...); }
    void pop_front() { erase(begin()); }
    void pop_back() { erase(std::prev(end())); }

    iterator insert(const_iterator pos, const T& v) { return emplace(pos, v); }

    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        Node* n = new Node(std::forward<Args>(args)...);
        link_before(pos.node_, n);
        ++size_;
        return iterator(n);
    }

    iterator erase(const_iterator pos)
    {
        NodeBase* n = pos.node_;
        NodeBase* next = n->next;
        unlink(n);
        delete static_cast<Node*>(n);
        --size_;
        return iterator(next);
    }

    void clear() noexcept
    {
        NodeBase* p = sentinel_.next;
        while (p != &sentinel_) {
            NodeBase* next = p->next;
            delete static_cast<Node*>(p);
            p = next;
        }
        sentinel_.prev = sentinel_.next = &sentinel_;
        size_ = 0;
    }

    void swap(List& other) noexcept
    {
        List tmp;
        tmp.take(other);
        other.take(*this);
        take(tmp);
    }

    // ---------- 算法 ----------

    void reverse() noexcept
    {
        NodeBase* p = &sentinel_;
        do {
            std::swap(p->prev, p->next);
            p = p->prev; // 交换后 prev 才是原来的 next
        } while (p != &sentinel_);
    }

    std::size_t remove(const T& value)
    {
        std::size_t removed = 0;
        for (auto it = begin(); it != end();) {
            if (*it == value) {
                it = erase(it);
                ++removed;
            } else {
                ++it;
            }
        }
        return removed;
    }

    // 删除相邻的重复元素（先 sort 再 unique 就能去重）
    void unique()
    {
        if (size_ < 2)
            return;
        auto prev = begin();
        for (auto it = std::next(prev); it != end();) {
            if (*it == *prev)
                it = erase(it);
            else
                prev = it++;
        }
    }

    // 稳定的归并排序，只改指针、不移动元素，O(n log n)
    template <typename Compare = std::less<T>>
    void sort(Compare comp = Compare())
    {
        if (size_ < 2)
            return;
        // 先断开成以 nullptr 结尾的单链（只用 next），排序后再修复 prev
        sentinel_.prev->next = nullptr;
        NodeBase* head = merge_sort(sentinel_.next, size_, comp);
        NodeBase* prev = &sentinel_;
        for (NodeBase* p = head; p; p = p->next) {
            p->prev = prev;
            prev->next = p;
            prev = p;
        }
        prev->next = &sentinel_;
        sentinel_.prev = prev;
    }

private:
    static void link_before(NodeBase* pos, NodeBase* n)
    {
        n->prev = pos->prev;
        n->next = pos;
        pos->prev->next = n;
        pos->prev = n;
    }

    static void unlink(NodeBase* n)
    {
        n->prev->next = n->next;
        n->next->prev = n->prev;
    }

    // 把 other 的全部结点接管过来（要求 *this 为空）
    void take(List& other) noexcept
    {
        if (other.empty())
            return;
        sentinel_.next = other.sentinel_.next;
        sentinel_.prev = other.sentinel_.prev;
        sentinel_.next->prev = &sentinel_;
        sentinel_.prev->next = &sentinel_;
        size_ = other.size_;
        other.sentinel_.prev = other.sentinel_.next = &other.sentinel_;
        other.size_ = 0;
    }

    template <typename Compare>
    static NodeBase* merge_sort(NodeBase* head, std::size_t n, Compare& comp)
    {
        if (n <= 1) {
            if (head)
                head->next = nullptr;
            return head;
        }
        std::size_t half = n / 2;
        NodeBase* mid = head;
        for (std::size_t i = 0; i < half; ++i)
            mid = mid->next;
        NodeBase* left = merge_sort(head, half, comp);
        NodeBase* right = merge_sort(mid, n - half, comp);

        NodeBase dummy{nullptr, nullptr};
        NodeBase* tail = &dummy;
        while (left && right) {
            // 右边严格更小才取右边，保证稳定性
            if (comp(static_cast<Node*>(right)->value, static_cast<Node*>(left)->value)) {
                tail->next = right;
                right = right->next;
            } else {
                tail->next = left;
                left = left->next;
            }
            tail = tail->next;
        }
        tail->next = left ? left : right;
        return dummy.next;
    }

    NodeBase sentinel_;
    std::size_t size_ = 0;
};

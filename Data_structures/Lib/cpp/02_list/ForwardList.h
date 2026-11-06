// ForwardList.h - 用 std::unique_ptr 管理结点的单链表。
//
// 学习要点：
//   - 每个结点"拥有"它的下一个结点（unique_ptr<Node> next），
//     删除结点不再需要手写 delete，也不可能重复释放。
//   - 陷阱：默认析构是递归的（析构 head 会析构 head->next ...），
//     链表很长时会栈溢出，所以析构函数里要手动循环释放。
//   - 拷贝需要深拷贝，unique_ptr 本身不可拷贝，正好提醒你思考所有权。
#pragma once

#include <cstddef>
#include <initializer_list>
#include <memory>
#include <utility>

template <typename T>
class ForwardList {
    struct Node {
        T value;
        std::unique_ptr<Node> next;
        explicit Node(T v, std::unique_ptr<Node> n = nullptr) : value(std::move(v)), next(std::move(n)) {}
    };

public:
    ForwardList() = default;
    ForwardList(std::initializer_list<T> init)
    {
        Node* tail = nullptr;
        for (const T& v : init)
            tail = append_after(tail, v);
    }
    ForwardList(const ForwardList& other)
    {
        Node* tail = nullptr;
        for (Node* p = other.head_.get(); p; p = p->next.get())
            tail = append_after(tail, p->value);
    }
    ForwardList(ForwardList&& other) noexcept
        : head_(std::move(other.head_)), size_(std::exchange(other.size_, 0)) {}
    ForwardList& operator=(ForwardList other) noexcept
    {
        std::swap(head_, other.head_);
        std::swap(size_, other.size_);
        return *this;
    }
    ~ForwardList() { clear(); }

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return !head_; }
    T& front() { return head_->value; }
    const T& front() const { return head_->value; }

    void push_front(T v)
    {
        head_ = std::make_unique<Node>(std::move(v), std::move(head_));
        ++size_;
    }

    void pop_front()
    {
        head_ = std::move(head_->next); // 旧的 head 在这里被自动释放
        --size_;
    }

    void clear() noexcept
    {
        while (head_)
            head_ = std::move(head_->next); // 循环释放，避免递归析构
        size_ = 0;
    }

    bool remove_first(const T& v)
    {
        // 指向 unique_ptr 的指针，相当于 C 版里的 SNode **link
        std::unique_ptr<Node>* link = &head_;
        while (*link) {
            if ((*link)->value == v) {
                *link = std::move((*link)->next);
                --size_;
                return true;
            }
            link = &(*link)->next;
        }
        return false;
    }

    void reverse() noexcept
    {
        std::unique_ptr<Node> prev;
        while (head_) {
            std::unique_ptr<Node> next = std::move(head_->next);
            head_->next = std::move(prev);
            prev = std::move(head_);
            head_ = std::move(next);
        }
        head_ = std::move(prev);
    }

    template <typename F>
    void for_each(F&& f) const
    {
        for (Node* p = head_.get(); p; p = p->next.get())
            f(p->value);
    }

private:
    Node* append_after(Node* tail, const T& v)
    {
        auto n = std::make_unique<Node>(v);
        Node* raw = n.get();
        if (tail)
            tail->next = std::move(n);
        else
            head_ = std::move(n);
        ++size_;
        return raw;
    }

    std::unique_ptr<Node> head_;
    std::size_t size_ = 0;
};

// Queue.h - 可自动扩容的循环队列模板（对照 c/03_stack_queue/circular_queue）。
//
// 与 C 版"牺牲一个空位"不同，这里额外记录 size_，因此所有槽都能用：
//   队空 size_ == 0，队满 size_ == cap_。
// 扩容时按逻辑顺序（从 head_ 开始）把元素搬到新数组的开头。
#pragma once

#include <cstddef>
#include <new>
#include <stdexcept>
#include <utility>

template <typename T>
class Queue {
public:
    Queue() = default;
    Queue(const Queue& other)
    {
        reserve(other.size_);
        for (std::size_t i = 0; i < other.size_; ++i)
            push(other.at_logical(i));
    }
    Queue(Queue&& other) noexcept
        : buf_(std::exchange(other.buf_, nullptr)),
          cap_(std::exchange(other.cap_, 0)),
          head_(std::exchange(other.head_, 0)),
          size_(std::exchange(other.size_, 0)) {}
    Queue& operator=(Queue other) noexcept
    {
        swap(other);
        return *this;
    }
    ~Queue()
    {
        clear();
        ::operator delete(buf_);
    }

    void push(const T& v) { emplace(v); }
    void push(T&& v) { emplace(std::move(v)); }

    template <typename... Args>
    void emplace(Args&&... args)
    {
        if (size_ == cap_)
            reserve(cap_ ? cap_ * 2 : 4);
        new (buf_ + (head_ + size_) % cap_) T(std::forward<Args>(args)...);
        ++size_;
    }

    void pop()
    {
        if (size_ == 0)
            throw std::underflow_error("Queue::pop on empty queue");
        buf_[head_].~T();
        head_ = (head_ + 1) % cap_;
        --size_;
    }

    T& front()
    {
        if (size_ == 0)
            throw std::underflow_error("Queue::front on empty queue");
        return buf_[head_];
    }
    T& back()
    {
        if (size_ == 0)
            throw std::underflow_error("Queue::back on empty queue");
        return at_logical(size_ - 1);
    }

    bool empty() const noexcept { return size_ == 0; }
    std::size_t size() const noexcept { return size_; }
    std::size_t capacity() const noexcept { return cap_; }

    void clear() noexcept
    {
        while (size_ > 0) {
            buf_[head_].~T();
            head_ = (head_ + 1) % cap_;
            --size_;
        }
        head_ = 0;
    }

    void swap(Queue& o) noexcept
    {
        std::swap(buf_, o.buf_);
        std::swap(cap_, o.cap_);
        std::swap(head_, o.head_);
        std::swap(size_, o.size_);
    }

private:
    T& at_logical(std::size_t i) { return buf_[(head_ + i) % cap_]; }
    const T& at_logical(std::size_t i) const { return buf_[(head_ + i) % cap_]; }

    void reserve(std::size_t new_cap)
    {
        if (new_cap <= cap_)
            return;
        T* nb = static_cast<T*>(::operator new(new_cap * sizeof(T)));
        for (std::size_t i = 0; i < size_; ++i) {
            T& src = at_logical(i);
            new (nb + i) T(std::move(src));
            src.~T();
        }
        ::operator delete(buf_);
        buf_ = nb;
        cap_ = new_cap;
        head_ = 0;
    }

    T* buf_ = nullptr;
    std::size_t cap_ = 0;
    std::size_t head_ = 0;
    std::size_t size_ = 0;
};

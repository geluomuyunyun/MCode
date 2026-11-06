// Stack.h - 栈适配器模板。
//
// 和 std::stack 一样，Stack 本身不管理内存，而是把操作转发给底层容器
// （默认用我们自己写的 Vector）。这就是"适配器模式"。
#pragma once

#include <stdexcept>
#include <utility>

#include "../01_vector/Vector.h"

template <typename T, typename Container = Vector<T>>
class Stack {
public:
    void push(const T& v) { c_.push_back(v); }
    void push(T&& v) { c_.push_back(std::move(v)); }

    template <typename... Args>
    void emplace(Args&&... args) { c_.emplace_back(std::forward<Args>(args)...); }

    void pop()
    {
        if (c_.empty())
            throw std::underflow_error("Stack::pop on empty stack");
        c_.pop_back();
    }

    T& top()
    {
        if (c_.empty())
            throw std::underflow_error("Stack::top on empty stack");
        return c_.back();
    }
    const T& top() const
    {
        if (c_.empty())
            throw std::underflow_error("Stack::top on empty stack");
        return c_.back();
    }

    bool empty() const { return c_.empty(); }
    std::size_t size() const { return c_.size(); }

private:
    Container c_;
};

// Vector.h - 仿 std::vector 的动态数组模板。
//
// 学习要点（对照 c/01_dynamic_array）：
//   - 模板：一份代码支持任意元素类型。模板的实现必须放在头文件里，
//     因为编译器要在使用处看到完整定义才能实例化。
//   - 内存与对象分离：::operator new 只分配原始内存，placement new 在上面构造对象，
//     析构时先显式调用析构函数，再释放内存。这样 reserve 不会默认构造多余元素。
//   - 五法则：析构、拷贝构造、拷贝赋值、移动构造、移动赋值。
//   - 迭代器：这里直接用指针当迭代器，因此可以配合 <algorithm> 和范围 for。
#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <new>
#include <stdexcept>
#include <utility>

template <typename T>
class Vector {
public:
    using value_type = T;
    using size_type = std::size_t;
    using iterator = T*;
    using const_iterator = const T*;

    Vector() = default;

    explicit Vector(size_type n, const T& value = T())
    {
        reserve(n);
        for (size_type i = 0; i < n; ++i)
            new (data_ + i) T(value);
        size_ = n;
    }

    Vector(std::initializer_list<T> init)
    {
        reserve(init.size());
        for (const T& v : init)
            new (data_ + size_++) T(v);
    }

    Vector(const Vector& other)
    {
        reserve(other.size_);
        for (const T& v : other)
            new (data_ + size_++) T(v);
    }

    Vector(Vector&& other) noexcept
        : data_(other.data_), size_(other.size_), cap_(other.cap_)
    {
        other.data_ = nullptr;
        other.size_ = other.cap_ = 0;
    }

    Vector& operator=(const Vector& other)
    {
        if (this != &other) {
            Vector tmp(other); // 先拷贝再交换：即使拷贝抛异常，*this 也保持原样
            swap(tmp);
        }
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept
    {
        if (this != &other) {
            release();
            data_ = other.data_;
            size_ = other.size_;
            cap_ = other.cap_;
            other.data_ = nullptr;
            other.size_ = other.cap_ = 0;
        }
        return *this;
    }

    ~Vector() { release(); }

    // ---------- 容量 ----------
    size_type size() const noexcept { return size_; }
    size_type capacity() const noexcept { return cap_; }
    bool empty() const noexcept { return size_ == 0; }

    void reserve(size_type new_cap)
    {
        if (new_cap <= cap_)
            return;
        T* p = static_cast<T*>(::operator new(new_cap * sizeof(T)));
        for (size_type i = 0; i < size_; ++i) {
            new (p + i) T(std::move_if_noexcept(data_[i]));
            data_[i].~T();
        }
        ::operator delete(data_);
        data_ = p;
        cap_ = new_cap;
    }

    void resize(size_type n, const T& value = T())
    {
        if (n < size_) {
            for (size_type i = n; i < size_; ++i)
                data_[i].~T();
        } else {
            reserve(n);
            for (size_type i = size_; i < n; ++i)
                new (data_ + i) T(value);
        }
        size_ = n;
    }

    void clear() noexcept
    {
        for (size_type i = 0; i < size_; ++i)
            data_[i].~T();
        size_ = 0;
    }

    // ---------- 元素访问 ----------
    T& operator[](size_type i) { return data_[i]; }
    const T& operator[](size_type i) const { return data_[i]; }

    T& at(size_type i)
    {
        if (i >= size_)
            throw std::out_of_range("Vector::at");
        return data_[i];
    }
    const T& at(size_type i) const
    {
        if (i >= size_)
            throw std::out_of_range("Vector::at");
        return data_[i];
    }

    T& front() { return data_[0]; }
    T& back() { return data_[size_ - 1]; }
    const T& front() const { return data_[0]; }
    const T& back() const { return data_[size_ - 1]; }
    T* data() noexcept { return data_; }

    // ---------- 修改 ----------
    void push_back(const T& v) { emplace_back(v); }
    void push_back(T&& v) { emplace_back(std::move(v)); }

    template <typename... Args>
    T& emplace_back(Args&&... args)
    {
        if (size_ == cap_) {
            // 参数可能引用自身元素（如 v.push_back(v[0])），先构造再扩容
            T tmp(std::forward<Args>(args)...);
            grow();
            new (data_ + size_) T(std::move(tmp));
        } else {
            new (data_ + size_) T(std::forward<Args>(args)...);
        }
        return data_[size_++];
    }

    void pop_back()
    {
        data_[--size_].~T();
    }

    iterator insert(size_type pos, const T& value)
    {
        if (pos > size_)
            throw std::out_of_range("Vector::insert");
        T tmp(value);
        if (size_ == cap_)
            grow();
        if (pos == size_) {
            new (data_ + size_) T(std::move(tmp));
        } else {
            new (data_ + size_) T(std::move(data_[size_ - 1]));
            for (size_type i = size_ - 1; i > pos; --i)
                data_[i] = std::move(data_[i - 1]);
            data_[pos] = std::move(tmp);
        }
        ++size_;
        return data_ + pos;
    }

    iterator erase(size_type pos)
    {
        if (pos >= size_)
            throw std::out_of_range("Vector::erase");
        for (size_type i = pos; i + 1 < size_; ++i)
            data_[i] = std::move(data_[i + 1]);
        data_[--size_].~T();
        return data_ + pos;
    }

    void swap(Vector& other) noexcept
    {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(cap_, other.cap_);
    }

    // ---------- 迭代器 ----------
    iterator begin() noexcept { return data_; }
    iterator end() noexcept { return data_ + size_; }
    const_iterator begin() const noexcept { return data_; }
    const_iterator end() const noexcept { return data_ + size_; }

    friend bool operator==(const Vector& a, const Vector& b)
    {
        return a.size_ == b.size_ && std::equal(a.begin(), a.end(), b.begin());
    }
    friend bool operator!=(const Vector& a, const Vector& b) { return !(a == b); }

private:
    void grow() { reserve(cap_ ? cap_ * 2 : 4); }

    void release() noexcept
    {
        clear();
        ::operator delete(data_);
        data_ = nullptr;
        cap_ = 0;
    }

    T* data_ = nullptr;
    size_type size_ = 0;
    size_type cap_ = 0;
};

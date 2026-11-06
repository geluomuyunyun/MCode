#include <cstddef>
#include <iostream>
#include <new>
#include <utility>

/*
* 对于这个数据类型有哪些算法需要掌握？
* 
*/

template <typename T>
class DynArray {
public:
    explicit DynArray(std::size_t capacity = 0)
        : data_(nullptr), size_(0), capacity_(0) {
        if (capacity > 0) {
            data_ = new T[capacity];
            capacity_ = capacity;
        }
    }
  
    ~DynArray() {
        delete[] data_;
    }

    // 拷贝构造函数
    DynArray(const DynArray &other)
        : data_(nullptr), size_(0), capacity_(0) {
        if (other.capacity_ > 0) {
            data_ = new T[other.capacity_];
            capacity_ = other.capacity_;
            for (std::size_t i = 0; i < other.size_; ++i) {
                data_[i] = other.data_[i];
            }
            size_ = other.size_;
        }
    }

	// 拷贝赋值运算符
    DynArray &operator=(const DynArray &other) {
        if (this != &other) {
            DynArray tmp(other);
            swap(tmp);
        }
        return *this;
    }

    DynArray(DynArray &&other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    DynArray &operator=(DynArray &&other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    std::size_t size() const { return size_; }
    std::size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    /* 下标访问。合法范围是 [0, size())。这里不检查越界，调用方要自己保证。 */
    T &operator[](std::size_t index) { return data_[index]; }
    const T &operator[](std::size_t index) const { return data_[index]; }

    /* 逻辑长度收到 n。n 大于 size 时失败。收起的对象仍占着容量，析构时才销毁。 */
    bool truncate(std::size_t n) {
        if (n > size_) {
            return false;
        }
        size_ = n;
        return true;
    }

    /* 增：尾部追加。先拷贝，避免 value 指向本数组内部元素时被扩容搬走 */
    bool push_back(const T &value) {
        T copied = value;
        if (!reserve(size_ + 1)) {
            return false;
        }
        data_[size_++] = std::move(copied);
        return true;
    }

    /* 增：在 index 处插入。index == size 时等价于尾插 */
    bool insert(std::size_t index, const T &value) {
        if (index > size_) {
            return false;
        }
        T copied = value;
        if (!reserve(size_ + 1)) {
            return false;
        }
        for (std::size_t i = size_; i > index; --i) {
            data_[i] = std::move(data_[i - 1]);
        }
        data_[index] = std::move(copied);
        ++size_;
        return true;
    }

    /* 删：按下标删除 */
    bool remove_at(std::size_t index) {
        if (index >= size_) {
            return false;
        }
        for (std::size_t i = index + 1; i < size_; ++i) {
            data_[i - 1] = std::move(data_[i]);
        }
        --size_;
        return true;
    }

    /* 删：删除第一个等于 value 的元素 */
    bool remove_value(const T &value) {
        std::size_t index = 0;
        if (!find(value, index)) {
            return false;
        }
        return remove_at(index);
    }

    /* 改：把 index 位置改成 value */
    bool set(std::size_t index, const T &value) {
        if (index >= size_) {
            return false;
        }
        data_[index] = value;
        return true;
    }

    /* 查：按下标读取 */
    bool get(std::size_t index, T &out) const {
        if (index >= size_) {
            return false;
        }
        out = data_[index];
        return true;
    }

    /* 查：按值查找第一个匹配下标 */
    bool find(const T &value, std::size_t &index_out) const {
        for (std::size_t i = 0; i < size_; ++i) {
            if (data_[i] == value) {
                index_out = i;
                return true;
            }
        }
        return false;
    }

    void print(std::ostream &os = std::cout) const {
        os << "size=" << size_ << " capacity=" << capacity_ << " [";
        for (std::size_t i = 0; i < size_; ++i) {
            if (i > 0) {
                os << ", ";
            }
            os << data_[i];
        }
        os << "]\n";
    }

private:
    T *data_;
    std::size_t size_;
    std::size_t capacity_;

    void swap(DynArray &other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    bool reserve(std::size_t need) {
        if (need <= capacity_) {
            return true;
        }
        std::size_t cap = (capacity_ == 0) ? 4 : capacity_;
        while (cap < need) {
            if (cap > static_cast<std::size_t>(-1) / 2) {
                cap = need;
                break;
            }
            cap *= 2;
        }

        T *next = new (std::nothrow) T[cap];
        if (next == nullptr) {
            return false;
        }
        try {
            for (std::size_t i = 0; i < size_; ++i) {
                next[i] = std::move(data_[i]);
            }
        } catch (...) {
            delete[] next;
            throw;
        }
        delete[] data_;
        data_ = next;
        capacity_ = cap;
        return true;
    }
};



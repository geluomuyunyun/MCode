/* 
* 未理解：
*	 关键字：noexcept 
* 
*/

#include <iostream>
#include <cstdlib>
#include <utility>

#define ASSERT(x) if (!(x)) __debugbreak;

template <typename T>
class DynArray {
private:
	T* data_;
	std::size_t size_;
	std::size_t capacity_;

	void swap(DynArray& other) noexcept {
		std::swap(data_, other.data_);
		std::swap(size_, other.size_);
		std::swap(capacity_, other.capacity_);
	}

public:
	explicit DynArray(std::size_t capacity = 0)
		: data_(nullptr), size_(0), capacity_(0) {
		std::cout << "构造函数被调用" << std::endl;
		if (capacity > 0) {

			capacity_ = capacity;
			data_ = new T[capacity_];

		}

		std::cout << "构造函数调用成功" << std::endl;
	}

	~DynArray() {
		delete[] data_;

		std::cout << "析构函数被调用" << std::endl;
	}

	DynArray(const DynArray<T>& other)
		: data_(nullptr), size_(0), capacity_(0) {
		std::cout << "拷贝构造函数被调用" << std::endl;

		if (other.capacity_ > 0) {

			capacity_ = other.capacity_;
			data_ = new T[capacity_];

			ASSERT(other.size_ < other.capacity_);

			for (std::size_t i = 0; i < other.size_; ++i) {
				data_[i] = other.data_[i];
			}

			size_ = other.size_;
		}

		std::cout << "拷贝构造函数调用成功" << std::endl;
	}

	DynArray& operator=(const DynArray& other) {
		std::cout << "拷贝赋值运算符被调用" << std::endl;
		if (this != &other) {
			DynArray<int> temp(other);
			swap(temp);
		}

		std::cout << "拷贝赋值运算符调用成功" << std::endl;
		return *this;
	}

	DynArray(DynArray&& other) noexcept
		: data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
		std::cout << "移动构造函数被调用" << std::endl;

		other.data_ = nullptr;
		other.size_ = 0;
		other.capacity_ = 0;

		std::cout << "移动构造函数调用成功" << std::endl;
	}

	DynArray& operator=(DynArray&& other) {
		std::cout << "移动赋值函数被调用" << std::endl;

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

};



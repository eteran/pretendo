
#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include <array>
#include <cassert>
#include <cstddef>

template <class T, size_t N>
class circular_buffer {
public:
	using value_type      = T;
	using pointer         = T *;
	using const_pointer   = const T *;
	using reference       = T &;
	using const_reference = const T &;
	using size_type       = size_t;
	using difference_type = ptrdiff_t;

public:
	circular_buffer()  = default;
	~circular_buffer() = default;

public:
	reference front() {
		assert(!empty());
		return array_[head_];
	}
	reference back() {
		assert(!empty());
		return array_[tail_];
	}
	const_reference front() const {
		assert(!empty());
		return array_[head_];
	}
	const_reference back() const {
		assert(!empty());
		return array_[tail_];
	}

public:
	const T *buffer() const { return &array_[0]; }
	size_t head() const { return head_; }
	size_t tail() const { return tail_; }

public:
	void clear() {
		head_ = 0;
		tail_ = 0;
		size_ = 0;
	}

public:
	const_reference operator[](size_t index) const {
		return array_[(head_ + index) % N];
	}

public:
	void push_back(const value_type &elem) {

		array_[tail_] = elem;
		if (size_ == N) {
			increment_head();
		}
		increment_tail();
	}

	void pop_front() {
		assert(!empty());
		increment_head();
	}

public:
	size_type size() const {
		return size_;
	}

	size_type capacity() const {
		return N;
	}

	bool empty() const {
		return size_ == 0;
	}

	size_type max_size() const {
		return size_type(-1) / sizeof(value_type);
	}

private:
	void increment_tail() {
		++size_;
		tail_ = (tail_ + 1) % N;
	}

	void increment_head() {
		assert(!empty());
		--size_;
		head_ = (head_ + 1) % N;
	}

private:
	std::array<T, N> array_;
	size_t head_ = 0;
	size_t tail_ = 0;
	size_t size_ = 0;
};

#endif

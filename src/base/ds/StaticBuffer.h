/*
 * StaticBuffer.h
 *
 * A sequential data container
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_STATICBUFFER_H_
#define WH_BASE_DS_STATICBUFFER_H_
#include "BufferVector.h"
#include "Twiddler.h"
#include "../common/reflect.h"
#include <cstring>

namespace wanhive {
/**
 * A sequential container of POD (plain old data) types that simulates a single
 * track magnetic tape storage.
 * @tparam X storage type
 * @tparam SIZE buffer's size
 */
template<typename X, unsigned int SIZE> class StaticBuffer {
public:
	/**
	 * Default constructor: creates a new buffer.
	 */
	StaticBuffer() noexcept;
	/**
	 * Destructor
	 */
	~StaticBuffer();
	//-----------------------------------------------------------------
	/**
	 * Clears the buffer: index is set to zero and limit is set to capacity.
	 */
	void clear() noexcept;
	/**
	 * Rewinds the buffer: limit is set to the current index and then the index
	 * is set to zero.
	 */
	void rewind() noexcept;
	/**
	 * Packs the buffer: moves elements in the interval [index, limit) to the
	 * beginning of the buffer. Index is then set to (limit-index) and limit is
	 * set to the capacity.
	 */
	void pack() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns buffer's capacity.
	 * @return buffer's capacity
	 */
	unsigned int capacity() const noexcept;
	/**
	 * Returns buffer's current index.
	 * @return the current index
	 */
	unsigned int getIndex() const noexcept;
	/**
	 * Sets buffer's index: new index should not be greater than the current limit.
	 * @param index the new index's value
	 * @return true on success, false on failure (invalid index)
	 */
	bool setIndex(unsigned int index) noexcept;
	/**
	 * Returns buffer's current limit.
	 * @return the current limit
	 */
	unsigned int getLimit() const noexcept;
	/**
	 * Sets buffer's limit: the new limit should not be greater than the
	 * capacity, and, should not be less than the current index.
	 * @param limit the new limit value
	 * @return true on success, false on failure (invalid limit)
	 */
	bool setLimit(unsigned int limit) noexcept;
	/**
	 * Returns the number of elements between the current index and the limit.
	 * @return number of elements remaining in the buffer
	 */
	unsigned int space() const noexcept;
	/**
	 * Checks if there are any elements between the current index and the limit.
	 * @return true if there are elements remaining in the buffer, false otherwise
	 */
	bool hasSpace() const noexcept;
	/**
	 * Gets the status of this buffer.
	 * @return this buffer's status
	 */
	int getStatus() const noexcept;
	/**
	 * Sets the status of this buffer.
	 * @param status the new status value
	 */
	void setStatus(int status) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Relative get method: reads the element at the current index and then
	 * increments the index.
	 * @param value object for storing the value
	 * @return true on success, false on failure (no remaining element)
	 */
	bool get(X &value) noexcept;
	/**
	 * Absolute get method: reads element at the given index which should be less
	 * than the current limit.
	 * @param value object for storing the value
	 * @param index the index to read from
	 * @return true on success, false otherwise (invalid index)
	 */
	bool get(X &value, unsigned int index) const noexcept;
	/**
	 * Relative bulk get method: transfers elements from the buffer into the given
	 * destination starting at the current index of the buffer and then updates
	 * the index.
	 * @param dest memory space for storing the values
	 * @param length maximum number of elements to read
	 * @return actual number of elements copied from the buffer
	 */
	unsigned int get(X *dest, unsigned int length) noexcept;
	/**
	 * Relative get method: returns a pointer to the element stored at the current
	 * index and then increments the index.
	 * @return pointer to element at the current index
	 */
	X* get() noexcept;
	/**
	 * Relative put method: writes the given value into the buffer at the current
	 * index and then increments the index.
	 * @param value value to put into the buffer
	 * @return true on success, false otherwise (no space remaining)
	 */
	bool put(X const &value) noexcept;
	/**
	 * Absolute put method: writes a value at the given index. The given index's
	 * value should be less than the current limit.
	 * @param value value to write at the given index
	 * @param index the index to write at
	 * @return true on success, false otherwise (invalid index)
	 */
	bool put(X const &value, unsigned int index) noexcept;
	/**
	 * Relative bulk put method: transfers values into the buffer from the given
	 * source starting at the current index of the buffer and updates the index.
	 * @param src source of data
	 * @param length maximum number of elements to write
	 * @return actual number of elements copied into the buffer
	 */
	unsigned int put(const X *src, unsigned int length) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns a pointer to the backing array.
	 * @return pointer to the backing array
	 */
	X* array() noexcept;
	/**
	 * Returns a constant pointer to the backing array.
	 * @return pointer to the backing array
	 */
	const X* array() const noexcept;
	/**
	 * Returns a pointer to the backing array offset by the index.
	 * @return pointer to the backing subarray starting at the current index
	 */
	X* offset() noexcept;
	/**
	 * Returns a constant pointer to the backing array offset by the index.
	 * @return pointer to the backing subarray starting at the current index
	 */
	const X* offset() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Pushes a value on top of the stack.
	 * @param e the value to write
	 * @return true on success, false otherwise (stack is full)
	 */
	bool push(X const &e) noexcept;
	/**
	 * Pops a value from top of the stack.
	 * @param e object for storing the value
	 * @return true on success, false otherwise (stack is empty)
	 */
	bool pop(X &e) noexcept;
	/**
	 * Peeks into the value on top of stack.
	 * @param e object for storing the value
	 * @return true on success, false otherwise (stack is empty)
	 */
	bool peek(X &e) noexcept;
	/**
	 * Checks if the stack is full.
	 * @return true if the stack is full, false otherwise.
	 */
	bool isFull() const noexcept;
	/**
	 * Checks if the stack is empty.
	 * @return true if the stack is empty, false otherwise.
	 */
	bool isEmpty() const noexcept;
private:
	WH_POD_ASSERT(X);
	static_assert((SIZE > 0), "Bad buffer size");
	const unsigned int _capacity { SIZE };
	unsigned int _limit { _capacity };
	unsigned int _index { 0 };
	int status { 0 };
	X storage[SIZE];
};

} /* namespace wanhive */

template<typename X, unsigned int SIZE>
wanhive::StaticBuffer<X, SIZE>::StaticBuffer() noexcept {

}

template<typename X, unsigned int SIZE>
wanhive::StaticBuffer<X, SIZE>::~StaticBuffer() {

}

template<typename X, unsigned int SIZE>
void wanhive::StaticBuffer<X, SIZE>::clear() noexcept {
	_limit = _capacity;
	_index = 0;
}

template<typename X, unsigned int SIZE>
void wanhive::StaticBuffer<X, SIZE>::rewind() noexcept {
	_limit = _index;
	_index = 0;
}

template<typename X, unsigned int SIZE>
void wanhive::StaticBuffer<X, SIZE>::pack() noexcept {
	auto n = space();
	if (_index && n) {
		memmove(storage, storage + _index, n * sizeof(X));
	}

	_limit = _capacity;
	_index = n;
}

template<typename X, unsigned int SIZE>
unsigned int wanhive::StaticBuffer<X, SIZE>::capacity() const noexcept {
	return _capacity;
}

template<typename X, unsigned int SIZE>
unsigned int wanhive::StaticBuffer<X, SIZE>::getIndex() const noexcept {
	return _index;
}

template<typename X, unsigned int SIZE>
bool wanhive::StaticBuffer<X, SIZE>::setIndex(unsigned int index) noexcept {
	if (index <= _limit) {
		_index = index;
		return true;
	} else {
		return false;
	}
}

template<typename X, unsigned int SIZE>
unsigned int wanhive::StaticBuffer<X, SIZE>::getLimit() const noexcept {
	return _limit;
}

template<typename X, unsigned int SIZE>
bool wanhive::StaticBuffer<X, SIZE>::setLimit(unsigned int limit) noexcept {
	if (limit <= _capacity && limit >= _index) {
		_limit = limit;
		return true;
	} else {
		return false;
	}
}

template<typename X, unsigned int SIZE>
unsigned int wanhive::StaticBuffer<X, SIZE>::space() const noexcept {
	return _limit - _index;
}

template<typename X, unsigned int SIZE>
bool wanhive::StaticBuffer<X, SIZE>::hasSpace() const noexcept {
	return _limit != _index;
}

template<typename X, unsigned int SIZE>
int wanhive::StaticBuffer<X, SIZE>::getStatus() const noexcept {
	return status;
}

template<typename X, unsigned int SIZE>
void wanhive::StaticBuffer<X, SIZE>::setStatus(int status) noexcept {
	this->status = status;
}

template<typename X, unsigned int SIZE>
bool wanhive::StaticBuffer<X, SIZE>::get(X &value) noexcept {
	if (_index != _limit) {
		value = storage[_index++];
		return true;
	} else {
		return false;
	}
}

template<typename X, unsigned int SIZE>
bool wanhive::StaticBuffer<X, SIZE>::get(X &value,
		unsigned int index) const noexcept {
	if (index < _limit) {
		value = storage[index];
		return true;
	} else {
		return false;
	}
}

template<typename X, unsigned int SIZE>
unsigned int wanhive::StaticBuffer<X, SIZE>::get(X *dest,
		unsigned int length) noexcept {
	length = Twiddler::min(space(), length);
	if (dest && length) {
		memcpy(dest, storage + _index, length * sizeof(X));
		_index += length;
		return length;
	} else {
		return 0;
	}
}

template<typename X, unsigned int SIZE>
X* wanhive::StaticBuffer<X, SIZE>::get() noexcept {
	if (_index != _limit) {
		return &storage[_index++];
	} else {
		return nullptr;
	}
}

template<typename X, unsigned int SIZE>
bool wanhive::StaticBuffer<X, SIZE>::put(const X &value) noexcept {
	if (_index != _limit) {
		storage[_index++] = value;
		return true;
	} else {
		return false;
	}
}

template<typename X, unsigned int SIZE>
bool wanhive::StaticBuffer<X, SIZE>::put(const X &value,
		unsigned int index) noexcept {
	if (index < _limit) {
		storage[index] = value;
		return true;
	} else {
		return false;
	}
}

template<typename X, unsigned int SIZE>
unsigned int wanhive::StaticBuffer<X, SIZE>::put(const X *src,
		unsigned int length) noexcept {
	length = Twiddler::min(space(), length);
	if (src && length) {
		memcpy(storage + _index, src, length * sizeof(X));
		_index += length;
		return length;
	} else {
		return 0;
	}
}

template<typename X, unsigned int SIZE>
X* wanhive::StaticBuffer<X, SIZE>::array() noexcept {
	return storage;
}

template<typename X, unsigned int SIZE>
const X* wanhive::StaticBuffer<X, SIZE>::array() const noexcept {
	return storage;
}

template<typename X, unsigned int SIZE>
X* wanhive::StaticBuffer<X, SIZE>::offset() noexcept {
	return storage + _index;
}

template<typename X, unsigned int SIZE>
const X* wanhive::StaticBuffer<X, SIZE>::offset() const noexcept {
	return storage + _index;
}

template<typename X, unsigned int SIZE>
bool wanhive::StaticBuffer<X, SIZE>::push(const X &e) noexcept {
	if (_index != _limit) {
		storage[_index++] = e;
		return true;
	} else {
		return false;
	}
}

template<typename X, unsigned int SIZE>
bool wanhive::StaticBuffer<X, SIZE>::pop(X &e) noexcept {
	if (_index) {
		e = storage[--_index];
		return true;
	} else {
		return false;
	}
}

template<typename X, unsigned int SIZE>
bool wanhive::StaticBuffer<X, SIZE>::peek(X &e) noexcept {
	if (_index) {
		e = storage[_index - 1];
		return true;
	} else {
		return false;
	}
}

template<typename X, unsigned int SIZE>
bool wanhive::StaticBuffer<X, SIZE>::isFull() const noexcept {
	return _index == _limit;
}

template<typename X, unsigned int SIZE>
bool wanhive::StaticBuffer<X, SIZE>::isEmpty() const noexcept {
	return _index == 0;
}

#endif /* WH_BASE_DS_STATICBUFFER_H_ */

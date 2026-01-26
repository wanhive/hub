/**
 * @file Buffer.h
 *
 * A sequential data container
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_BUFFER_H_
#define WH_BASE_DS_BUFFER_H_
#include "BufferVector.h"
#include "Twiddler.h"
#include "../common/Exception.h"
#include "../common/NonCopyable.h"
#include "../common/reflect.h"
#include <cstring>

/*! @namespace wanhive */
namespace wanhive {
/**
 * A sequential container of POD (plain old data) types that simulates a single
 * track magnetic tape storage.
 * @tparam X storage type
 */
template<typename X> class Buffer: private NonCopyable {
public:
	/**
	 * Default constructor: creates an empty buffer of zero capacity.
	 */
	Buffer() noexcept;
	/**
	 * Constructor: creates buffer of given capacity.
	 * @param size buffer's capacity
	 */
	Buffer(unsigned int size);
	/**
	 * Destructor
	 */
	~Buffer();
	//-----------------------------------------------------------------
	/**
	 * Clears and resizes the buffer.
	 * @param size buffer's new capacity
	 */
	void initialize(unsigned int size);
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
	 * Returns buffer's capacity (see Buffer::initialize()).
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
	X *storage { };
	unsigned int _capacity { };
	unsigned int _limit { };
	unsigned int _index { };
	int status { };
};

} /* namespace wanhive */

template<typename X>
wanhive::Buffer<X>::Buffer() noexcept {

}

template<typename X>
wanhive::Buffer<X>::Buffer(unsigned int size) {
	initialize(size);
}

template<typename X>
wanhive::Buffer<X>::~Buffer() {
	delete[] storage;
}

template<typename X>
void wanhive::Buffer<X>::initialize(unsigned int size) {
	try {
		delete[] storage;
		storage = new X[size];
		_capacity = size;
		clear();
	} catch (...) {
		storage = nullptr;
		_capacity = 0;
		clear();
		throw Exception(EX_MEMORY);
	}
}

template<typename X>
void wanhive::Buffer<X>::clear() noexcept {
	_limit = _capacity;
	_index = 0;
}

template<typename X>
void wanhive::Buffer<X>::rewind() noexcept {
	_limit = _index;
	_index = 0;
}

template<typename X>
void wanhive::Buffer<X>::pack() noexcept {
	auto n = space();
	if (_index && n) {
		memmove(storage, storage + _index, n * sizeof(X));
	}

	_limit = _capacity;
	_index = n;
}

template<typename X>
unsigned int wanhive::Buffer<X>::capacity() const noexcept {
	return _capacity;
}

template<typename X>
unsigned int wanhive::Buffer<X>::getIndex() const noexcept {
	return _index;
}

template<typename X>
bool wanhive::Buffer<X>::setIndex(unsigned int index) noexcept {
	if (index <= _limit) {
		_index = index;
		return true;
	} else {
		return false;
	}
}

template<typename X>
unsigned int wanhive::Buffer<X>::getLimit() const noexcept {
	return _limit;
}

template<typename X>
bool wanhive::Buffer<X>::setLimit(unsigned int limit) noexcept {
	if (limit <= _capacity && limit >= _index) {
		_limit = limit;
		return true;
	} else {
		return false;
	}
}

template<typename X>
unsigned int wanhive::Buffer<X>::space() const noexcept {
	return _limit - _index;
}

template<typename X>
bool wanhive::Buffer<X>::hasSpace() const noexcept {
	return _limit != _index;
}

template<typename X>
int wanhive::Buffer<X>::getStatus() const noexcept {
	return status;
}

template<typename X>
void wanhive::Buffer<X>::setStatus(int status) noexcept {
	this->status = status;
}

template<typename X>
bool wanhive::Buffer<X>::get(X &value) noexcept {
	if (_index != _limit) {
		value = storage[_index++];
		return true;
	} else {
		return false;
	}
}

template<typename X>
bool wanhive::Buffer<X>::get(X &value, unsigned int index) const noexcept {
	if (index < _limit) {
		value = storage[index];
		return true;
	} else {
		return false;
	}
}

template<typename X>
unsigned int wanhive::Buffer<X>::get(X *dest, unsigned int length) noexcept {
	length = Twiddler::min(space(), length);
	if (dest && length) {
		memcpy(dest, storage + _index, length * sizeof(X));
		_index += length;
		return length;
	} else {
		return 0;
	}
}

template<typename X>
X* wanhive::Buffer<X>::get() noexcept {
	if (_index != _limit) {
		return &storage[_index++];
	} else {
		return nullptr;
	}
}

template<typename X>
bool wanhive::Buffer<X>::put(const X &value) noexcept {
	if (_index != _limit) {
		storage[_index++] = value;
		return true;
	} else {
		return false;
	}
}

template<typename X>
bool wanhive::Buffer<X>::put(const X &value, unsigned int index) noexcept {
	if (index < _limit) {
		storage[index] = value;
		return true;
	} else {
		return false;
	}
}

template<typename X>
unsigned int wanhive::Buffer<X>::put(const X *src, unsigned int length) noexcept {
	length = Twiddler::min(space(), length);
	if (src && length) {
		memcpy(storage + _index, src, length * sizeof(X));
		_index += length;
		return length;
	} else {
		return 0;
	}
}

template<typename X>
X* wanhive::Buffer<X>::array() noexcept {
	return storage;
}

template<typename X>
const X* wanhive::Buffer<X>::array() const noexcept {
	return storage;
}

template<typename X>
X* wanhive::Buffer<X>::offset() noexcept {
	return storage + _index;
}

template<typename X>
const X* wanhive::Buffer<X>::offset() const noexcept {
	return storage + _index;
}

template<typename X>
bool wanhive::Buffer<X>::push(const X &e) noexcept {
	if (_index != _limit) {
		storage[_index++] = e;
		return true;
	} else {
		return false;
	}
}

template<typename X>
bool wanhive::Buffer<X>::pop(X &e) noexcept {
	if (_index) {
		e = storage[--_index];
		return true;
	} else {
		return false;
	}
}

template<typename X>
bool wanhive::Buffer<X>::peek(X &e) noexcept {
	if (_index) {
		e = storage[_index - 1];
		return true;
	} else {
		return false;
	}
}

template<typename X>
bool wanhive::Buffer<X>::isFull() const noexcept {
	return _index == _limit;
}

template<typename X>
bool wanhive::Buffer<X>::isEmpty() const noexcept {
	return _index == 0;
}

#endif /* WH_BASE_DS_BUFFER_H_ */

/*
 * Buffer.h
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
#include "Twiddler.h"
#include "../common/Exception.h"
#include "../common/pod.h"
#include <cstring>

namespace wanhive {
/**
 * A sequential container for POD types
 * Simulates single track magnetic tape storage
 * Thread safe at class level
 */
template<typename X> class Buffer {
public:
	Buffer() noexcept;
	Buffer(unsigned int size);
	~Buffer();

	//Initializes the buffer to a particular size
	void initialize(unsigned int size);
	//Returns buffer's capacity.
	unsigned int capacity() const noexcept;
	//Returns buffer's current index.
	unsigned int getIndex() const noexcept;
	/*
	 * Sets buffer's index. The new index should not be greater than the current
	 * limit. Returns true on success, false otherwise.
	 */
	bool setIndex(unsigned int index) noexcept;
	//Returns buffer's current limit.
	unsigned int getLimit() const noexcept;
	/*
	 * Sets buffer's limit. The new limit should not be greater than the
	 * capacity, and, should not be less than the current index. Returns
	 * true on success, false otherwise.
	 */
	bool setLimit(unsigned int limit) noexcept;
	/*
	 * Resets the buffer to it's initial state.
	 * The index is set to zero and the limit is set to the capacity.
	 */
	void clear() noexcept;
	/*
	 * Rewinds the buffer after a sequence of relative put operations.
	 * The limit is set to the current index and then the index is set to zero.
	 */
	void rewind() noexcept;
	//Returns the number of elements between the current index and the limit.
	unsigned int space() const noexcept;
	//Returns true if there are any elements between the current index and the limit.
	bool hasSpace() const noexcept;
	/*
	 * Relative get method reads the element at the current index and then
	 * increments the index. Returns true on success, false otherwise.
	 */
	bool get(X &value) noexcept;
	/*
	 * Absolute get method reads the element at the given index.
	 * Returns true on success, false otherwise.
	 */
	bool get(X &value, unsigned int index) const noexcept;
	/*
	 * Relative bulk get method transfers elements from the buffer into the given
	 * destination starting at the current index of the buffer. At most <length>
	 * elements are transferred, index is updated and actual number of elements
	 * copied from the buffer is returned.
	 */
	unsigned int get(X *dest, unsigned int length) noexcept;
	/*
	 * Relative get method returns a pointer to the element stored
	 * at the current index and then increments the index.
	 */
	X* get() noexcept;
	/*
	 * Relative put method writes the given element into the buffer at
	 * the current index and then increments the index.
	 */
	bool put(X const &value) noexcept;
	/*
	 * Absolute put method writes the given element at the given index.
	 * Returns true on success, false otherwise.
	 */
	bool put(X const &value, unsigned int index) noexcept;
	/*
	 * Relative bulk put method transfers elements into the buffer from the given
	 * source starting at the current index of the buffer. At most <length> elements
	 * are transferred, index is updated and actual number of elements copied into
	 * the buffer is returned.
	 */
	unsigned int put(const X *src, unsigned int length) noexcept;
	/*
	 * Moves elements in the interval [index, limit) to the beginning of the buffer.
	 * The index is then set to (limit-index) and the limit is set to the capacity.
	 */
	void pack() noexcept;
	//Returns a pointer to the backing array.
	X* array() noexcept;
	//Returns a const pointer to the backing array.
	const X* array() const noexcept;
	//Returns a pointer to the backing array offset by the index.
	X* offset() noexcept;
	//Returns a const pointer to the backing array offset by the index.
	const X* offset() const noexcept;
	//Get the status of this buffer
	int getStatus() const noexcept;
	//Set the status of this buffer
	void setStatus(int status) noexcept;
	//=================================================================
	/**
	 * Stack operations
	 */
	bool push(X const &e) noexcept;
	bool pop(X &e) noexcept;
	bool peek(X &e) noexcept;
	bool isFull() const noexcept;
	bool isEmpty() const noexcept;
private:
	WH_POD_ASSERT(X);
	X *storage;
	unsigned int _capacity;
	unsigned int _limit;
	unsigned int _index;
	int status;
};

} /* namespace wanhive */

template<typename X> wanhive::Buffer<X>::Buffer() noexcept {
	storage = nullptr;
	_capacity = 0;
	clear();
}

template<typename X> wanhive::Buffer<X>::Buffer(unsigned int size) {
	storage = nullptr;
	_capacity = 0;
	initialize(size);
}

template<typename X> wanhive::Buffer<X>::~Buffer() {
	delete[] storage;
}

template<typename X> void wanhive::Buffer<X>::initialize(unsigned int size) {
	try {
		delete[] storage;
		storage = new X[size];
		_capacity = size;
		clear();
	} catch (...) {
		storage = nullptr;
		_capacity = 0;
		clear();
		throw Exception(EX_ALLOCFAILED);
	}
}

template<typename X> unsigned int wanhive::Buffer<X>::capacity() const noexcept {
	return _capacity;
}

template<typename X> unsigned int wanhive::Buffer<X>::getIndex() const noexcept {
	return _index;
}

template<typename X> bool wanhive::Buffer<X>::setIndex(
		unsigned int index) noexcept {
	if (index <= _limit) {
		_index = index;
		return true;
	} else {
		return false;
	}
}

template<typename X> unsigned int wanhive::Buffer<X>::getLimit() const noexcept {
	return _limit;
}

template<typename X> bool wanhive::Buffer<X>::setLimit(
		unsigned int limit) noexcept {
	if (limit <= _capacity && limit >= _index) {
		_limit = limit;
		return true;
	} else {
		return false;
	}
}

template<typename X> void wanhive::Buffer<X>::clear() noexcept {
	_limit = _capacity;
	_index = 0;
}

template<typename X> void wanhive::Buffer<X>::rewind() noexcept {
	_limit = _index;
	_index = 0;
}

template<typename X> unsigned int wanhive::Buffer<X>::space() const noexcept {
	return _limit - _index;
}

template<typename X> bool wanhive::Buffer<X>::hasSpace() const noexcept {
	return _limit != _index;
}

template<typename X> bool wanhive::Buffer<X>::get(X &value) noexcept {
	if (_index != _limit) {
		value = storage[_index++];
		return true;
	} else {
		return false;
	}
}

template<typename X> bool wanhive::Buffer<X>::get(X &value,
		unsigned int index) const noexcept {
	if (index < _limit) {
		value = storage[index];
		return true;
	} else {
		return false;
	}
}

template<typename X> unsigned int wanhive::Buffer<X>::get(X *dest,
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

template<typename X> X* wanhive::Buffer<X>::get() noexcept {
	if (_index != _limit) {
		return &storage[_index++];
	} else {
		return nullptr;
	}
}

template<typename X> bool wanhive::Buffer<X>::put(const X &value) noexcept {
	if (_index != _limit) {
		storage[_index++] = value;
		return true;
	} else {
		return false;
	}
}

template<typename X> bool wanhive::Buffer<X>::put(const X &value,
		unsigned int index) noexcept {
	if (index < _limit) {
		storage[index] = value;
		return true;
	} else {
		return false;
	}
}

template<typename X> unsigned int wanhive::Buffer<X>::put(const X *src,
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

template<typename X> void wanhive::Buffer<X>::pack() noexcept {
	auto n = space();
	if (_index && n) {
		memmove(storage, storage + _index, n * sizeof(X));
	}

	_limit = _capacity;
	_index = n;
}

template<typename X> X* wanhive::Buffer<X>::array() noexcept {
	return storage;
}

template<typename X> const X* wanhive::Buffer<X>::array() const noexcept {
	return storage;
}

template<typename X> X* wanhive::Buffer<X>::offset() noexcept {
	return storage + _index;
}

template<typename X> const X* wanhive::Buffer<X>::offset() const noexcept {
	return storage + _index;
}

template<typename X> int wanhive::Buffer<X>::getStatus() const noexcept {
	return status;
}

template<typename X> void wanhive::Buffer<X>::setStatus(int status) noexcept {
	this->status = status;
}

template<typename X> bool wanhive::Buffer<X>::push(const X &e) noexcept {
	if (_index != _limit) {
		storage[_index++] = e;
		return true;
	} else {
		return false;
	}
}

template<typename X> bool wanhive::Buffer<X>::pop(X &e) noexcept {
	if (_index) {
		e = storage[--_index];
		return true;
	} else {
		return false;
	}
}

template<typename X> bool wanhive::Buffer<X>::peek(X &e) noexcept {
	if (_index) {
		e = storage[_index - 1];
		return true;
	} else {
		return false;
	}
}

template<typename X> bool wanhive::Buffer<X>::isFull() const noexcept {
	return _index == _limit;
}

template<typename X> bool wanhive::Buffer<X>::isEmpty() const noexcept {
	return _index == 0;
}

#endif /* WH_BASE_DS_BUFFER_H_ */

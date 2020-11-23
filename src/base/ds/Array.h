/*
 * Array.h
 *
 * Resizable array of POD types
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_ARRAY_H_
#define WH_BASE_DS_ARRAY_H_
#include "../common/Memory.h"
#include "../common/pod.h"
#include <cstdlib>

namespace wanhive {
/**
 * Poor man's vector for POD types.
 * Capacity is doubled on every overflow.
 */
template<typename X = unsigned int> class Array {
public:
	Array() noexcept {
		storage = nullptr;
		_capacity = 0;
		_limit = 0;
		_index = 0;
	}

	//<size> can be zero
	Array(unsigned int size) noexcept {
		storage = nullptr;
		initialize(size);
	}

	~Array() {
		WH_free(storage);
	}

	//Initialize the container, <size> can be zero
	void initialize(unsigned int size = 0) noexcept {
		resize(size);
		clear();
	}

	unsigned int capacity() const noexcept {
		return _capacity;
	}

	bool isEmpty() const noexcept {
		return _limit == 0;
	}

	bool isFull() const noexcept {
		return _limit == _capacity;
	}

	//How many items can be read from this container
	unsigned int readSpace() const noexcept {
		return _limit;
	}

	//How many items can be added to this container without resizing it
	unsigned int writeSpace() const noexcept {
		return _capacity - _limit;
	}

	//Relative get method, reads an element from the container and removes it
	bool get(X &e) noexcept {
		if (_limit) {
			_index = _index % _limit;
			e = storage[_index];
			removeAtIndex(_index);
			_index += 1;
			return true;
		} else {
			return false;
		}
	}

	//Absolute get method, reads the value at the given index (doesn't remove)
	bool get(X &e, unsigned int index) const noexcept {
		if (index < _limit) {
			e = storage[index];
			return true;
		} else {
			return false;
		}
	}

	//Absolute get method, returns the pointer to the element at the given index
	X* get(unsigned int index) noexcept {
		if (index < _limit) {
			return storage + index;
		} else {
			return nullptr;
		}
	}

	//Inserts an element into the container
	void put(const X &value) noexcept {
		if (_capacity == _limit) {
			resize(_capacity ? (_capacity << 1) : 16);
		}

		storage[_limit++] = value;
	}

	//Removes the entry at the given index
	void remove(unsigned int index) noexcept {
		if (index < _limit) {
			removeAtIndex(index);
		}
	}

	//Shrinks the sparsely populated array if it is larger than <threshold>
	void shrink(unsigned int threshold) noexcept {
		if (threshold && (_limit > threshold) && _limit < (_capacity >> 2)) {
			resize(_limit << 1);
		}
	}

	//Clears the container (doesn't resize the storage)
	void clear() noexcept {
		_limit = 0;
		_index = 0;
	}

	//Container traversal, <f> must return 0 to continue
	void map(int (*f)(const X &entry, void *arg), void *data) {
		for (unsigned int i = 0; f && i < _limit; ++i) {
			if (f(storage[i], data)) {
				break;
			}
		}
	}
	//=================================================================
	/*
	 * Inserts the <value> at the end the <array>.
	 * <array> is automatically resized if needed. <capacity> and <offset> are updated
	 * and pointer to the newly inserted element is returned. <offset> is the current
	 * number of elements in the array while <capacity> is the size of the array.
	 */
	static X* insert(X *&array, unsigned int &capacity, unsigned int &offset,
			const X &value) noexcept {
		if (offset == capacity) {
			if (capacity < 4) {
				capacity = 4;
			} else {
				capacity <<= 1;
			}
			array = (X*) realloc(array, sizeof(X) * capacity);
			if (array == nullptr) {
				abort();
			}
		}
		array[offset++] = value;
		return &array[offset - 1];
	}
private:
	void removeAtIndex(unsigned int index) noexcept {
		storage[index] = storage[--_limit];
	}

	void resize(unsigned int size) noexcept {
		WH_resize(storage, size);
		_capacity = size;
	}
private:
	WH_POD_ASSERT(X);
	X *storage;
	unsigned int _capacity;
	unsigned int _limit;
	unsigned int _index;
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_ARRAY_H_ */

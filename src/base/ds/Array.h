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
 * Light weight vector for POD (plain old data) types.
 * @tparam X storage type
 */
template<typename X = int> class Array {
public:
	/**
	 * Default constructor: creates an empty container.
	 */
	Array() noexcept {
		storage = nullptr;
		_capacity = 0;
		_limit = 0;
		_index = 0;
	}
	/**
	 * Constructor: creates an empty container of the given (initial) size.
	 * @param size containers size
	 */
	Array(unsigned int size) noexcept {
		storage = nullptr;
		initialize(size);
	}
	/**
	 * Destructor
	 */
	~Array() {
		WH_free(storage);
	}
	//-----------------------------------------------------------------
	/**
	 * Empties and resizes the container.
	 * @param size container's new size
	 */
	void initialize(unsigned int size = 0) noexcept {
		resize(size);
		clear();
	}
	/**
	 * Empties the container.
	 */
	void clear() noexcept {
		_limit = 0;
		_index = 0;
	}
	//-----------------------------------------------------------------
	/**
	 * Returns the capacity.
	 * @return total capacity
	 */
	unsigned int capacity() const noexcept {
		return _capacity;
	}
	/**
	 * Checks if the container is empty.
	 * @return true if empty, false otherwise
	 */
	bool isEmpty() const noexcept {
		return _limit == 0;
	}
	/**
	 * Checks if the container is full, i.e. it's currently allocated capacity
	 * has been exhausted.
	 * @return true if full, false otherwise
	 */
	bool isFull() const noexcept {
		return _limit == _capacity;
	}
	/**
	 * Returns the number of items which can be read from the container.
	 * @return number of items present in the container
	 */
	unsigned int readSpace() const noexcept {
		return _limit;
	}
	/**
	 * Returns the number of items which can be added to the container without
	 * having to resize it.
	 * @return number of free slots in the container
	 */
	unsigned int writeSpace() const noexcept {
		return _capacity - _limit;
	}
	//-----------------------------------------------------------------
	/**
	 * Relative get method, reads a value from the container and removes it.
	 * @param e object for storing the value
	 * @return true on success, false otherwise (container is empty)
	 */
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
	/**
	 * Absolute get method, reads value at the given index and doesn't remove
	 * the value.
	 * @param e object for storing the value
	 * @param index index/position to read from
	 * @return true on success, false otherwise (invalid index)
	 */
	bool get(X &e, unsigned int index) const noexcept {
		if (index < _limit) {
			e = storage[index];
			return true;
		} else {
			return false;
		}
	}
	/**
	 * Absolute get method, returns a pointer to the element at the given index
	 * and doesn't remove the element.
	 * @param index index/position to read from
	 * @return pointer to the element at the given index on success, nullptr on
	 * failure (invalid index).
	 */
	X* get(unsigned int index) noexcept {
		if (index < _limit) {
			return storage + index;
		} else {
			return nullptr;
		}
	}

	/**
	 * Inserts a value into the container (automatically expands the container's
	 * capacity on overflow).
	 * @param value the value to insert
	 */
	void put(const X &value) noexcept {
		if (_capacity == _limit) {
			resize(_capacity ? (_capacity << 1) : 16);
		}

		storage[_limit++] = value;
	}
	/**
	 * Removes element at the given index from the container.
	 * @param index index/position of the element to remove
	 * @param shrink true to shrink the container if it is sparsely populated,
	 * false otherwise.
	 */
	void remove(unsigned int index, bool shrink = true) noexcept {
		if (index < _limit) {
			removeAtIndex(index);
		}

		if (shrink) {
			this->shrink(4096);
		}
	}
	/**
	 * Container traversal, the call back function must return 0 to continue,
	 * and any non-zero value to stop the traversal.
	 * @param f the callback function
	 * @param data extra argument for the callback function
	 */
	void map(int (&f)(const X &entry, void *arg), void *data) {
		for (unsigned int i = 0; i < _limit; ++i) {
			if (f(storage[i], data)) {
				break;
			}
		}
	}
	/**
	 * Shrinks the container if it is sparsely populated and it is larger than
	 * the given threshold.
	 * @param threshold the threshold value
	 */
	void shrink(unsigned int threshold) noexcept {
		if (threshold && (_limit > threshold) && _limit < (_capacity >> 2)) {
			resize(_limit << 1);
		}
	}
	//-----------------------------------------------------------------
	/**
	 * Inserts a value at the end of the dynamically resizable array.
	 * @param array pointer to the dynamically resizable array
	 * @param capacity value-result argument for passing on the array's current
	 * size and receiving the updated size (array may get resized).
	 * @param offset value-result argument for passing on the current number of
	 * elements in the array and receiving the updated count.
	 * @param value the value to insert
	 * @return pointer to the newly added value inside the array
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

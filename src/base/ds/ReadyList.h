/*
 * ReadyList.h
 *
 * Resizable ready list
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_READYLIST_H_
#define WH_BASE_DS_READYLIST_H_
#include "../common/Memory.h"
#include "../common/NonCopyable.h"

namespace wanhive {
/**
 * Light weight ready list of POD types.
 * @tparam T data type
 * @tparam NOTHROW true for succeed or bust mode, false for error checking
 */
template<typename T = int, bool NOTHROW = true> class ReadyList: private NonCopyable {
public:
	/**
	 * Default constructor: creates an empty container.
	 */
	ReadyList() noexcept {

	}
	/**
	 * Constructor: creates an empty container of a given initial capacity.
	 * @param size initial capacity
	 * @throw
	 */
	ReadyList(unsigned int size) noexcept (NOTHROW) {
		Memory<T, NOTHROW>::resize(storage, size);
		_capacity = size;
	}
	/**
	 * Destructor
	 */
	~ReadyList() {
		Memory<T, NOTHROW>::free(storage);
	}
	//-----------------------------------------------------------------
	/**
	 * Empties and resizes the container.
	 * @param size container's new size
	 * @throw
	 */
	void initialize(unsigned int size = 0) noexcept (NOTHROW) {
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
	 * Returns the current capacity.
	 * @return current capacity
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
	 * Checks if the container is full.
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
	 * @return number of free spaces in the container
	 */
	unsigned int writeSpace() const noexcept {
		return _capacity - _limit;
	}
	//-----------------------------------------------------------------
	/**
	 * Relative get method, reads a value from the container and removes it.
	 * @param e stores the read value
	 * @return true on success, false otherwise (container is empty)
	 */
	bool get(T &e) noexcept {
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
	 * Absolute get method, reads value at a given index and doesn't remove
	 * the value.
	 * @param e stores the read value
	 * @param index index's value
	 * @return true on success, false otherwise (invalid index)
	 */
	bool get(T &e, unsigned int index) const noexcept {
		if (index < _limit) {
			e = storage[index];
			return true;
		} else {
			return false;
		}
	}
	/**
	 * Absolute get method, returns a pointer to the element at a given index
	 * and doesn't remove the element.
	 * @param index index's value
	 * @return element's address at the given index on success, nullptr on
	 * failure (invalid index).
	 */
	T* get(unsigned int index) noexcept {
		if (index < _limit) {
			return storage + index;
		} else {
			return nullptr;
		}
	}

	/**
	 * Inserts a value into the container (automatically expands the container
	 * on overflow).
	 * @param e value to insert
	 * @throw
	 */
	void put(const T &e) noexcept (NOTHROW) {
		if (_capacity == _limit) {
			resize(_capacity ? (_capacity << 1) : 16);
		}

		storage[_limit++] = e;
	}
	/**
	 * Removes element at a given index from the container.
	 * @param index index/position of the element to remove
	 * @param shrink true to shrink the container if it is sparsely populated,
	 * false otherwise.
	 * @throw
	 */
	void remove(unsigned int index, bool shrink = true) noexcept (NOTHROW) {
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
	 * @param f callback function
	 * @param arg second argument for the callback function
	 */
	void map(int (&f)(const T&, void*), void *arg) {
		for (unsigned int i = 0; i < _limit; ++i) {
			if (f(storage[i], arg)) {
				break;
			}
		}
	}
	/**
	 * Shrinks the container if it is sparsely populated and it is larger than
	 * the given threshold.
	 * @param threshold threshold's value
	 * @throw
	 */
	void shrink(unsigned int threshold) noexcept (NOTHROW) {
		if (threshold && (_limit > threshold) && _limit < (_capacity >> 2)) {
			resize(_limit << 1);
		}
	}
private:
	void removeAtIndex(unsigned int index) noexcept {
		storage[index] = storage[--_limit];
	}

	void resize(unsigned int size) noexcept (NOTHROW) {
		Memory<T, NOTHROW>::resize(storage, size);
		_capacity = size;
		if (_limit > _capacity) {
			_limit = _capacity;
		}

		if (_index > _limit) {
			_index = 0;
		}
	}
private:
	WH_POD_ASSERT(T);
	T *storage { };
	unsigned int _capacity { };
	unsigned int _limit { };
	unsigned int _index { };
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_READYLIST_H_ */

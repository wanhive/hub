/*
 * Memory.h
 *
 * C style memory management routines
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_MEMORY_H_
#define WH_BASE_COMMON_MEMORY_H_
#include "Exception.h"
#include "NonCopyable.h"
#include "reflect.h"
#include <cstdlib>
#include <cstring>

namespace wanhive {
/**
 * C style memory management routines.
 * @tparam T data type
 * @tparam NOTHROW true for succeed or bust mode, false for error checking
 */
template<typename T, bool NOTHROW = true> class Memory: private NonCopyable {
public:
	/**
	 * Default constructor: creates an empty object.
	 */
	Memory() noexcept :
			p { nullptr }, _capacity { 0 } {

	}
	/**
	 * Constructor: dynamically allocates memory of a given size.
	 * @param count elements count
	 * @throw
	 */
	Memory(size_t count) noexcept (NOTHROW) :
			p { Memory::allocate(count) }, _capacity { count } {

	}
	/**
	 * Destructor: frees memory.
	 */
	~Memory() {
		Memory::free(p);
	}
	//-----------------------------------------------------------------
	/**
	 * Resizes the dynamically allocated memory.
	 * @param count new elements count
	 * @throw
	 */
	void resize(size_t count) noexcept (NOTHROW) {
		Memory::resize(p, count);
		_capacity = count;
	}
	/**
	 * Returns the allocated memory's capacity.
	 * @return current capacity (elements count)
	 */
	size_t capacity() const noexcept {
		return _capacity;
	}
	//-----------------------------------------------------------------
	/**
	 * Returns reference to the element stored at a given index.
	 * @param index index's value
	 * @return element's reference
	 */
	T& operator[](size_t index) {
		if (index < _capacity) {
			return p[index];
		} else {
			throw Exception(EX_INDEX);
		}
	}

	/**
	 * Returns reference to the element stored at a given index.
	 * @param index index's value
	 * @return element's reference
	 */
	const T& operator[](size_t index) const {
		if (index < _capacity) {
			return p[index];
		} else {
			throw Exception(EX_INDEX);
		}
	}

	/**
	 * Returns the element's address at a given index.
	 * @param index index's value
	 * @return element's address
	 */
	T* offset(size_t index = 0) noexcept {
		if (index < _capacity) {
			return &p[index];
		} else {
			return nullptr;
		}
	}

	/**
	 * Returns the element's address at a given index.
	 * @param index index's value
	 * @return element's address
	 */
	const T* offset(size_t index = 0) const noexcept {
		if (index < _capacity) {
			return &p[index];
		} else {
			return nullptr;
		}
	}
	//-----------------------------------------------------------------
	/**
	 * Appends a value to a dynamically resizable array.
	 * @param array pointer to the dynamically resizable array
	 * @param size value-result argument for array's total size
	 * @param limit value-result argument for array's limit
	 * @param value input value
	 * @throw
	 * @return updated memory location inside the array
	 */
	static T* append(T *&array, unsigned int &size, unsigned int &limit,
			const T &value) noexcept (NOTHROW) {
		if (limit == size) {
			if (size < 4) {
				size = 4;
			} else {
				size <<= 1;
			}

			Memory::resize(array, size);
		}
		array[limit++] = value;
		return &array[limit - 1];
	}
	//-----------------------------------------------------------------
	/**
	 * Dynamically allocates memory of a given size.
	 * @param count elements count
	 * @throw
	 * @return allocated memory's address
	 */
	static T* allocate(size_t count) noexcept (NOTHROW) {
		auto p = static_cast<T*>(::calloc(count, sizeof(T)));
		if (p || !count) {
			return p;
		} else if constexpr (!NOTHROW) {
			throw Exception(EX_MEMORY);
		} else {
			abort();
		}
	}

	/**
	 * Changes dynamically allocated memory's size (memory bock's address may
	 * change on successful operation).
	 * @param p value-result argument for memory block's address
	 * @param count new elements count
	 * @throw
	 */
	static void resize(T *&p, size_t count) noexcept (NOTHROW) {
#if _DEFAULT_SOURCE
		auto np = static_cast<T*>(::reallocarray(p, count, sizeof(T)));
#else
		auto np = static_cast<T*>(::realloc(p, (count * sizeof(T))));
#endif
		if (np || !count) {
			p = np;
		} else if constexpr (!NOTHROW) {
			throw Exception(EX_MEMORY);
		} else {
			abort();
		}
	}

	/**
	 * Frees dynamically allocated memory.
	 * @param p memory's address
	 */
	static void free(T *p) noexcept {
		::free(p);
	}
private:
	T *p;
	size_t _capacity;
	WH_POD_ASSERT(T);
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_MEMORY_H_ */

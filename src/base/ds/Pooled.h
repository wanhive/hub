/*
 * Pooled.h
 *
 * Object pool implementation
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_POOLED_H_
#define WH_BASE_DS_POOLED_H_
#include "MemoryPool.h"
#include "../common/Exception.h"

namespace wanhive {
/**
 * Object pool.
 * @tparam X object's type (class name)
 */
template<typename X> class Pooled {
public:
	/**
	 * Constructor: prevents dynamic array creation.
	 * @param c ignored
	 */
	Pooled(char c) noexcept;
	/**
	 * Destructor
	 */
	~Pooled() = default;
	/**
	 * Initializes the object pool
	 * @param size object pool's size
	 */
	static void initPool(unsigned int size);
	/**
	 * Destroys the object pool
	 */
	static void destroyPool();
	/**
	 * Returns the capacity.
	 * @return object pool's capacity
	 */
	static unsigned int poolSize() noexcept;
	/**
	 * Returns the number of objects currently allocated.
	 * @return the allocated objects count
	 */
	static unsigned int allocated() noexcept;
	/**
	 * Returns the number of additional objects which can be allocated.
	 * @return the unallocated objects count
	 */
	static unsigned int unallocated() noexcept;
protected:
	/**
	 * The new operator that works with the preallocated memory pool.
	 * @param size object's size
	 */
	void* operator new(size_t size) noexcept;
	/**
	 * The delete operator that works with the preallocated memory pool.
	 * @param p pointer to free
	 */
	void operator delete(void *p) noexcept;
private:
	static MemoryPool pool;
};

} /* namespace wanhive */

template<typename X>
wanhive::MemoryPool wanhive::Pooled<X>::pool;

template<typename X>
inline wanhive::Pooled<X>::Pooled(char c) noexcept {

}

template<typename X>
inline void wanhive::Pooled<X>::initPool(unsigned int size) {
	pool.initialize(sizeof(X), size);
}

template<typename X>
inline void wanhive::Pooled<X>::destroyPool() {
	if (pool.destroy() != 0) {
		throw Exception(EX_INVALIDSTATE);
	}
}

template<typename X>
inline unsigned int wanhive::Pooled<X>::poolSize() noexcept {
	return pool.capacity();
}

template<typename X>
inline unsigned int wanhive::Pooled<X>::allocated() noexcept {
	return pool.allocated();
}

template<typename X>
inline unsigned int wanhive::Pooled<X>::unallocated() noexcept {
	return poolSize() - allocated();
}

template<typename X>
inline void* wanhive::Pooled<X>::operator new(size_t size) noexcept {
	return pool.allocate();
}

template<typename X>
inline void wanhive::Pooled<X>::operator delete(void *p) noexcept {
	pool.deallocate(p);
}

#endif /* WH_BASE_DS_POOLED_H_ */

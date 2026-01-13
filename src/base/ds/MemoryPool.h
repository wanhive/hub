/*
 * MemoryPool.h
 *
 * Memory pool for fixed size objects
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_MEMORYPOOL_H_
#define WH_BASE_DS_MEMORYPOOL_H_
#include "../common/NonCopyable.h"

namespace wanhive {
/**
 * Memory pool for efficient memory allocation and deallocation.
 * Trades-off safety and versatility for speed.
 */
class MemoryPool: private NonCopyable {
public:
	/**
	 * Default constructor: creates a zero-capacity memory pool.
	 */
	MemoryPool() noexcept;
	/**
	 * Constructor: creates a memory pool
	 * @param size size of each memory block
	 * @param count number of memory blocks
	 */
	MemoryPool(unsigned int size, unsigned int count);
	/**
	 * Destructor: calls abort() on memory leak
	 */
	~MemoryPool();
	/**
	 * Initializes the memory pool if not already initialized.
	 * @param size size of each memory block
	 * @param count number of memory blocks (0 will result in no-op)
	 */
	void initialize(unsigned int size, unsigned int count);
	/**
	 * Frees the memory pool. On success, the memory pool can be reinitialized.
	 * @return number of memory blocks still in use. Non-zero value indicates a
	 * memory leak and/or an imminent segmentation fault.
	 */
	unsigned int destroy() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Allocates a memory block.
	 * @return pointer to the newly allocated memory block
	 */
	void* allocate() noexcept;
	/**
	 * Frees the given memory block and returns it into the pool. Freeing an
	 * already freed memory block or an external memory block will result in
	 * undefined behavior.
	 * @param p the memory block to deallocate
	 */
	void deallocate(void *p) noexcept;
	/**
	 * Returns the allocated memory blocks count.
	 * @return number of allocated blocks
	 */
	unsigned int allocated() const noexcept;
	/**
	 * Returns the total number of memory blocks including the allocated ones.
	 * @return this memory pool's capacity
	 */
	unsigned int capacity() const noexcept;
	/**
	 * Returns the size of each memory block.
	 * @return memory block's size
	 */
	unsigned int blockSize() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the memory pool's initialization status.
	 * @return true if successfully initialized, false otherwise
	 */
	bool isInitialized() const noexcept;
private:
	void *_bucket { };
	void *_head { };
	unsigned int _allocated { };
	unsigned int _capacity { };
	unsigned int _blockSize { };
};

} /* namespace wanhive */
#endif /* WH_BASE_DS_MEMORYPOOL_H_ */

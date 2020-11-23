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
#include <cstddef>

namespace wanhive {
/**
 * Memory pool for efficient memory management.
 * Trades off safety and versatility for fast allocation and deallocation.
 * Thread safe at class level.
 */
class MemoryPool {
public:
	//Doesn't initialize the pool
	MemoryPool() noexcept;
	//Initializes the pool, throws Exception
	MemoryPool(unsigned int size, unsigned int count);
	//Aborts on memory leak
	~MemoryPool();
	/*
	 * Initializes a memory pool containing <count> blocks of <size> bytes each.
	 * Throws Exception if called improperly or already initialized. <count> can
	 * be zero which results in a noop.
	 */
	void initialize(unsigned int size, unsigned int count);
	/*
	 * Frees the memory pool and returns the number of blocks still in use.
	 * A non-zero returned value indicates a memory leak and an imminent
	 * segmentation fault. On success, the memory pool can be re-initialized.
	 */
	unsigned int destroy() noexcept;
	//-----------------------------------------------------------------
	//Allocates a memory block from the pool
	void* allocate() noexcept;
	//Deallocates the given memory block and return it into the pool
	void deallocate(void *p) noexcept;
	//-----------------------------------------------------------------
	//Returns true if the memory pool was successfully initialized
	bool isInitialized() const noexcept;
	//Number of allocated blocks
	unsigned int allocated() const noexcept;
	//Total number of blocks in this pool (including the allocated ones)
	unsigned int capacity() const noexcept;
	//Size of each memory block
	unsigned int blockSize() const noexcept;
private:
	void *_bucket;
	void *_head;				//Pointer to the first available block
	unsigned int _allocated;
	unsigned int _capacity;
	unsigned int _blockSize;
	//Desired alignment of each block
	static constexpr unsigned int ALIGNMENT = (alignof(max_align_t));
};

} /* namespace wanhive */
#endif /* WH_BASE_DS_MEMORYPOOL_H_ */

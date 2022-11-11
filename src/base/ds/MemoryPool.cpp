/*
 * MemoryPool.cpp
 *
 * Memory pool for fixed size objects
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "MemoryPool.h"
#include "Twiddler.h"
#include "../common/Exception.h"
#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace {

//Desired alignment of each block
static constexpr unsigned int ALIGNMENT = (alignof(max_align_t));

}  // namespace

namespace wanhive {

MemoryPool::MemoryPool() noexcept {

}

MemoryPool::MemoryPool(unsigned int size, unsigned int count) {
	initialize(size, count);
}

MemoryPool::~MemoryPool() {
	if (destroy()) {
		abort();
	}
}

void MemoryPool::initialize(unsigned int size, unsigned int count) {
	if (isInitialized()) {
		throw Exception(EX_OPERATION);
	} else if (!size && count) {
		throw Exception(EX_ARGUMENT);
	} else if (!count) {
		return;
	}

	size = Twiddler::align(size, Twiddler::power2Ceil(ALIGNMENT));
	if ((_bucket = calloc(count, size)) == nullptr) {
		throw Exception(EX_MEMORY);
	}

	_head = _bucket;
	_capacity = count;
	_blockSize = size;

	/*
	 * Linked list of unallocated blocks
	 */
	auto block = (char*) _bucket;
	while (--count != 0) {
		auto next = block + size;
		*(void**) block = next;
		block = next;
	}
	*(void**) block = nullptr;
}

unsigned int MemoryPool::destroy() noexcept {
	auto ret = _allocated;
	if (_bucket) {
		memset(_bucket, 0, ((size_t) _capacity) * _blockSize);
	}
	free(_bucket);
	_bucket = nullptr;
	_head = nullptr;
	_allocated = 0;
	_capacity = 0;
	_blockSize = 0;
	return ret;
}

void* MemoryPool::allocate() noexcept {
	auto result = _head;
	if (result) {
		_head = *(void**) _head;
		++_allocated;
	}
	return result;
}

void MemoryPool::deallocate(void *p) noexcept {
	if (p) {
		if (_allocated == 0) {
			abort();
		}
		--_allocated;
		*(void**) p = _head;
		_head = p;
	}
}

unsigned int MemoryPool::allocated() const noexcept {
	return _allocated;
}

unsigned int MemoryPool::capacity() const noexcept {
	return _capacity;
}

unsigned int MemoryPool::blockSize() const noexcept {
	return _blockSize;
}

bool MemoryPool::isInitialized() const noexcept {
	return _bucket;
}

} /* namespace wanhive */

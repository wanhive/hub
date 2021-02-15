/*
 * Memory.cpp
 *
 * C style memory management in C++
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Memory.h"
#include "audit.h"
#include <cstring>

namespace wanhive {

void* Memory::malloc(size_t size, const char *filename, int linenumber,
		const char *function) noexcept {
	auto result = ::malloc(size);
	if (result == nullptr && size != 0) {
		WH_FATAL_ERROR_FLF("Memory::malloc", filename, linenumber, function);
	}
	return result;
}

void Memory::free(void *p) noexcept {
	::free(p);
}

void Memory::realloc(void **p, size_t elementSize, size_t count,
		const char *filename, int linenumber, const char *function) noexcept {
	auto size = elementSize * count;
	//If size==0, then free the heap memory (Linux specific)
	auto ptr = ::realloc(*p, size);

	if (ptr == nullptr && size != 0) {
		WH_FATAL_ERROR_FLF("Memory::realloc", filename, linenumber, function);
	}
	*p = ptr;
}

char* Memory::strdup(const char *s, const char *filename, int linenumber,
		const char *function) noexcept {
	auto ret = ::strdup(s);
	if (ret == nullptr) {
		WH_FATAL_ERROR_FLF("Memory::strdup", filename, linenumber, function);
	}
	return ret;
}

} /* namespace wanhive */

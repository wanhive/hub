/*
 * Memory.h
 *
 * C style memory management in C++
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_MEMORY_H_
#define WH_BASE_COMMON_MEMORY_H_
#include "defines.h"
#include <cstddef>

namespace wanhive {
/**
 * C style memory management routines, succeed or bust mode.
 * Use the macros provided at the bottom of this header.
 * Do not mix with new and delete.
 */
class Memory {
public:
	/**
	 * Wrapper for malloc(3) library function.
	 * @param size number of bytes to allocate
	 * @param filename source filename for debugging
	 * @param linenumber line number for debugging
	 * @param function caller's name for debugging
	 */
	static void* malloc(size_t size, const char *filename, int linenumber,
			const char *function) noexcept;
	/**
	 * Wrapper for free(3) library function
	 * @param p pointer to the memory location
	 */
	static void free(void *p) noexcept;
	/**
	 * Wrapper for realloc(3) library function
	 * @param p pointer to the pointer to the effected memory location
	 * @param elementSize unit size in bytes
	 * @param count number of units
	 * @param filename source filename for debugging
	 * @param linenumber line number for debugging
	 * @param function caller's name for debugging
	 */
	static void realloc(void **p, size_t elementSize, size_t count,
			const char *filename, int linenumber, const char *function) noexcept;
	/**
	 * Wrapper for strdup(3) library function
	 * @param s pointer to the string to duplicate
	 * @param filename source filename for debugging
	 * @param linenumber line number for debugging
	 * @param function caller's name for debugging
	 * @return pointer to the duplicated string
	 */
	static char* strdup(const char *s, const char *filename, int linenumber,
			const char *function) noexcept;
};
//-----------------------------------------------------------------
/*
 * Useful macros
 */
#define WH_malloc(size) Memory::malloc((size), WH_FILE, WH_LINE, WH_FUNCTION)
#define WH_free(p) Memory::free((p))
#define WH_realloc(ptr, blockSize, length) \
	Memory::realloc((void**)&ptr, blockSize, length, WH_FILE, WH_LINE, WH_FUNCTION)
#define WH_resize(ptr, length) WH_realloc(ptr, sizeof(ptr[0]), length)
#define WH_strdup(s) Memory::strdup((s), WH_FILE, WH_LINE, WH_FUNCTION)
//-----------------------------------------------------------------
} /* namespace wanhive */

#endif /* WH_BASE_COMMON_MEMORY_H_ */

/*
 * Twiddler.h
 *
 * Bit and byte manipulations
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_TWIDDLER_H_
#define WH_BASE_DS_TWIDDLER_H_
#include "../common/defines.h"
#include <cstdint>

namespace wanhive {
/**
 * Integer operations
 * Ref: https://graphics.stanford.edu/~seander/bithacks.html
 */
class Twiddler {
public:
	//=================================================================
	/**
	 * Frequently used unsigned integer operations
	 */
	//Max of <x> and <y>
	static unsigned int max(unsigned int x, unsigned int y) noexcept;
	//Min of <x> and <y>
	static unsigned int min(unsigned int x, unsigned int y) noexcept;
	//Determine whether <x> is a power of 2 (x==0 returns false)
	static bool isPower2(unsigned int x) noexcept;
	//(n mod s) where s is a power of two
	static unsigned int modPow2(unsigned int n, unsigned int s) noexcept;
	//n mod 2^exp
	static unsigned int modExp2(unsigned int n, unsigned int exp) noexcept;
	//Swap values of the two integers <x> and <y>
	static void exchange(unsigned int &x, unsigned int &y) noexcept;
	//=================================================================
	//Greatest power-of-two integer less than or equal to <x>
	static unsigned int power2Floor(unsigned int x) noexcept;
	//Least power-of-two integer greater than or equal to <x>
	static unsigned int power2Ceil(unsigned int x) noexcept;
	//=================================================================
	/**
	 * Thomas Wang's mix functions for integers, better for non-random input
	 */
	//For 32-bit values
	static unsigned long mix(unsigned long i) noexcept;
	//For 64-bit values
	static unsigned long long mix(unsigned long long l) noexcept;
	//=================================================================
	//FVN-1a hash (Fowler–Noll–Vo hash)
	static unsigned long long FVN1aHash(const void *data,
			unsigned int bytes) noexcept;
	//=================================================================
	/**
	 * Interval functions
	 */
	//Check whether value  is inside the open *CIRCULAR* interval (<from>, <to>)
	static bool isBetween(unsigned int value, unsigned int from,
			unsigned int to) noexcept;
	//Check whether value is inside the closed *CIRCULAR* interval [<from>, <to>]
	static bool isInRange(unsigned int value, unsigned int from,
			unsigned int to) noexcept;
	//=================================================================
	/**
	 * Basic bit mask operations on 32-bit values
	 */
	//If (set) word |= mask; else word &= ~bitmask;
	static uint32_t mask(uint32_t word, uint32_t bitmask, bool set) noexcept;
	//word |= mask
	static uint32_t set(uint32_t word, uint32_t bitmask) noexcept;
	//word &= ~bitmask
	static uint32_t clear(uint32_t word, uint32_t bitmask) noexcept;
	//return (word & bitmask)
	static uint32_t test(uint32_t word, uint32_t bitmask) noexcept;
	//Merge bits of two integers x and y [(x & ~bitmask) | (y & bitmask))]
	static uint32_t merge(uint32_t x, uint32_t y, uint32_t bitmask) noexcept;
	//=================================================================
	/**
	 * Bit array implementation primitives.
	 * All functions assume 8-bit unsigned char.
	 * All functions assume valid parameters e.g. valid non-null pointer and valid index.
	 */
	//Fast modulus 8 of an unsigned integer
	static unsigned int mod8(unsigned int x) noexcept;
	//Fast division by 8 of an unsigned integer
	static unsigned int div8(unsigned int x) noexcept;
	//Fast multiplication with 8 of an unsigned integer
	static unsigned int mult8(unsigned int x) noexcept;
	//Count the number of bits in a 8-bit unsigned integer
	static unsigned int bitCount(unsigned char v) noexcept;
	//Count the number of bits in a 32-bit integer
	static unsigned int bitCount(uint32_t v) noexcept;
	//Position of the first set bit from right in a 8-bit unsigned integer
	static unsigned int bitOrdinal(unsigned char v) noexcept;
	//How many 8-bit slots are required to store given number of bits
	static unsigned int bitNSlots(unsigned int bits) noexcept;
	//To which 8-bit slot does the bit at the given index belong
	static unsigned int bitSlot(unsigned int index) noexcept;
	//Bitmask for the bit at the given index (can be used for bitwise operations)
	static unsigned char bitMask(unsigned int index) noexcept;

	//Set <index>th bit in the bitmap
	static void set(unsigned char *bitmap, unsigned int index) noexcept;
	//Clear <index>th bit in the bitmap
	static void clear(unsigned char *bitmap, unsigned int index) noexcept;
	//Toggle <index>th bit in the bitmap
	static void toggle(unsigned char *bitmap, unsigned int index) noexcept;
	//Test  <index>th bit in the bitmap
	static bool test(const unsigned char *bitmap, unsigned int index) noexcept;
	//=================================================================
	/*
	 * Returns the smallest multiple of <alignment> greater than or equal to <size>
	 * *<alignment> must be power of two
	 */
	static unsigned int align(unsigned int size,
			unsigned int alignment) noexcept;
	//Returns the ceiling of n/k (least integer >= n/k)
	static unsigned int ceiling(unsigned int n, unsigned int k) noexcept;
	//=================================================================
	/**
	 * ASCII STRING MANIPULATIONS
	 * Pointers cannot be null
	 */

	/*
	 * In following two functions <src> and <dest> may point to the same location,
	 * transformed buffer is placed in <dest>.
	 * <src> should be NUL terminated, <dest> is NUL terminated
	 */
	static unsigned int toUpperCase(char *dest, const char *src) noexcept;
	static unsigned int toLowerCase(char *dest, const char *src) noexcept;
	/*
	 * XOR <len> bytes of <s1> and <s2> and place the result in <dest>
	 * <dest> can be one of <s1> and <s2>, <dest> is not NUL-terminated
	 */
	static void xorString(unsigned char *dest, const unsigned char *s1,
			const unsigned char *s2, unsigned int length) noexcept;
	//Replaces last occurance of delimiter with NUL and returns that position (nullptr if not found)
	static char* stripLast(char *s, char delimiter) noexcept;
	//Removes all whitespace characters from an ASCII string
	static char* removeWhitespace(char *s) noexcept;
	//Trim the ASCII string (the final string length is copied into <len>)
	static char* trim(char *s, unsigned int &len) noexcept;
private:
	//Pre-calculated lookup table of number of bits in a 8-bit byte
	static const unsigned char bitCount_[256];
	//Pre-calculated lookup table of index to first set bit (from right) in a 8-bit byte
	static const unsigned char bitOrdinal_[256];
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_TWIDDLER_H_ */

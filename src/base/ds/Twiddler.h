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
#include <cstdint>

namespace wanhive {
/**
 * Integer operations
 * @ref https://graphics.stanford.edu/~seander/bithacks.html
 */
class Twiddler {
public:
	/**
	 * Calculates the maximum of two numbers.
	 * @param x first value
	 * @param y second value
	 * @return maximum value
	 */
	static unsigned int max(unsigned int x, unsigned int y) noexcept;
	/**
	 * Calculates the minimum of two numbers.
	 * @param x first value
	 * @param y second value
	 * @return minimum value
	 */
	static unsigned int min(unsigned int x, unsigned int y) noexcept;
	/**
	 * Determines if the given value is a power of two (2).
	 * @param x unsigned value
	 * @return true if the given value is a power of two, false otherwise
	 */
	static bool isPower2(unsigned int x) noexcept;
	/**
	 * Performs modulus arithmetic where the divisor is a power of two.
	 * @param n value to divide
	 * @param s a power of two divisor
	 * @return the remainder
	 */
	static unsigned int modPow2(unsigned int n, unsigned int s) noexcept;
	/**
	 * Performs modulus arithmetic where the divisor is a value of two raised
	 * to the power of the given exponent.
	 * @param n value to divide
	 * @param exp exponent's value (should be less than 32)
	 * @return the remainder
	 */
	static unsigned int modExp2(unsigned int n, unsigned int exp) noexcept;
	/**
	 * Efficiently swaps two numbers.
	 * @param x first value
	 * @param y second value
	 */
	static void exchange(unsigned int &x, unsigned int &y) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Calculates a power of two integer which is less than or equal to the
	 * given value.
	 * @param x unsigned value
	 * @return greatest power-of-two integer less than or equal to the
	 * given value.
	 */
	static unsigned int power2Floor(unsigned int x) noexcept;
	/**
	 * Calculates a power of two integer which is greater than or equal to the
	 * given value.
	 * @param x unsigned value
	 * @return least power-of-two integer greater than or equal to the
	 * given value.
	 */
	static unsigned int power2Ceil(unsigned int x) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Thomas Wang's mix function: for 32-bit integers.
	 * @param i 32-bit input
	 * @return 32-bit hash value
	 */
	static unsigned long mix(unsigned long i) noexcept;
	/**
	 * Thomas Wang's mix function: for 64-bit integers.
	 * @param l 64-bit input
	 * @return 64-bit hash value
	 */
	static unsigned long long mix(unsigned long long l) noexcept;
	/**
	 * Thomas Wang's 64-bit to 32-bit hash function.
	 * @param l 64-bit input
	 * @return 32-bit hash value
	 */
	static unsigned long hash(unsigned long long l) noexcept;
	//-----------------------------------------------------------------
	/**
	 * FVN-1a hash (Fowler–Noll–Vo hash) with 64-bit output.
	 * @param data input data
	 * @param bytes input data's size in bytes
	 * @return 64-bit hash value
	 */
	static unsigned long long FVN1aHash(const void *data,
			unsigned int bytes) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Interval function: checks whether the given value belongs to an open
	 * circular interval.
	 * @param value input value
	 * @param from lower bound of the open circular interval
	 * @param to upper bound of the open circular interval
	 * @return true if the given value lies within the open circular interval,
	 * false otherwise.
	 */
	static bool isBetween(unsigned int value, unsigned int from,
			unsigned int to) noexcept;
	/**
	 * Interval function: checks whether the given value belongs to a closed
	 * circular interval.
	 * @param value input value
	 * @param from lower bound of the closed circular interval
	 * @param to upper bound of the closed circular interval
	 * @return true if the given value lies within the closed circular interval,
	 * false otherwise.
	 */
	static bool isInRange(unsigned int value, unsigned int from,
			unsigned int to) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Bitwise operation: conditionally sets or clears bits of a given operand.
	 * @param word 32-bit operand
	 * @param bitmask applicable bitmask
	 * @param set true to set the bits, false to clear
	 * @return result of the operation
	 */
	static uint32_t mask(uint32_t word, uint32_t bitmask, bool set) noexcept;
	/**
	 * Bitwise operation: sets bits of a given operand.
	 * @param word 32-bit operand
	 * @param bitmask applicable bitmask
	 * @return result of the operation (operand | bitmask)
	 */
	static uint32_t set(uint32_t word, uint32_t bitmask) noexcept;
	/**
	 * Bitwise operation: clears bits of a given operand.
	 * @param word 32-bit operand
	 * @param bitmask applicable bitmask
	 * @return result of the operation (operand & ~bitmask)
	 */
	static uint32_t clear(uint32_t word, uint32_t bitmask) noexcept;
	/**
	 * Bitwise operation: tests bits of a given operand.
	 * @param word 32-bit operand
	 * @param bitmask applicable bitmask
	 * @return result of the operation (operand & bitmask).
	 */
	static uint32_t test(uint32_t word, uint32_t bitmask) noexcept;
	/**
	 * Bitwise operation: merges bits from two values according to a bitmask.
	 * @param x  first value
	 * @param y second value
	 * @param bitmask applicable bitmask, 0 where bits from the first value
	 * should be selected, 1 where from the second value.
	 * @return result of the operation ((first & ~bitmask) | (second & bitmask))
	 */
	static uint32_t merge(uint32_t x, uint32_t y, uint32_t bitmask) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Fast modulus 8 of an unsigned integer.
	 * @param x unsigned value
	 * @return remainder of (value / 8)
	 */
	static unsigned int mod8(unsigned int x) noexcept;
	/**
	 * Fast division by 8 of an unsigned integer.
	 * @param x unsigned value
	 * @return result of (value / 8)
	 */
	static unsigned int div8(unsigned int x) noexcept;
	/**
	 * Fast multiplication with 8 of an unsigned integer.
	 * @param x unsigned value
	 * @return result of (value * 8)
	 */
	static unsigned int mult8(unsigned int x) noexcept;
	/**
	 * Counts the number of set bits in a 8-bit unsigned integer.
	 * @param v 8-bit unsigned value
	 * @return set bits count
	 */
	static unsigned int bitCount(unsigned char v) noexcept;
	/**
	 * Counts the number of set bits in a 32-bit unsigned integer.
	 * @param v 32-bit unsigned value
	 * @return set bits count
	 */
	static unsigned int bitCount(uint32_t v) noexcept;
	/**
	 * Returns the first set bit's position from right in an 8-bit unsigned
	 * integer value.
	 * @param v 8-bit unsigned value
	 * @return the first set bit's position from right, 8 if none of the bits
	 * are set (value = 0).
	 */
	static unsigned int bitOrdinal(unsigned char v) noexcept;
	/**
	 * Calculates a bit array's size in bytes.
	 * @param bits number of bits
	 * @return number of bytes
	 */
	static unsigned int bitNSlots(unsigned int bits) noexcept;
	/**
	 * Calculates byte's position that corresponds to a given index in a
	 * bit array.
	 * @param index bit's index
	 * @return octet's index
	 */
	static unsigned int bitSlot(unsigned int index) noexcept;
	/**
	 * Calculates a suitable bitmask to operate on the byte corresponding to
	 * a given index in a bit array (see Twiddler::bitSlot()).
	 * @param index bit's index
	 * @return applicable bitmask
	 */
	static unsigned char bitMask(unsigned int index) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Bit array operation: sets bit at a given index.
	 * @param bits bit array
	 * @param index index to set
	 */
	static void set(unsigned char *bits, unsigned int index) noexcept;
	/**
	 * Bit array operation: clears bit at a given index.
	 * @param bits bit array
	 * @param index index to clear
	 */
	static void clear(unsigned char *bits, unsigned int index) noexcept;
	/**
	 * Bit array operation: toggles bit (if bit is set then it is cleared, if
	 * bit is clear then it is set) at a given index.
	 * @param bits bit aray
	 * @param index index to toggle
	 */
	static void toggle(unsigned char *bits, unsigned int index) noexcept;
	/**
	 * Bit array operation: checks if bit at a given index is set.
	 * @param bits bit array
	 * @param index index to test
	 * @return true if set, false if clear
	 */
	static bool test(const unsigned char *bits, unsigned int index) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the smallest multiple of an alignment greater than or equal to a
	 * given size.
	 * @param size applicable size
	 * @param alignment a power-of-two value describing the alignment
	 * @return a value that is equal to or greater than the given size and fits
	 * the given alignment/byte-boundary.
	 */
	static unsigned int align(unsigned int size,
			unsigned int alignment) noexcept;
	/**
	 * Calculates the minimum value greater than or equal to the result of a
	 * division operation.
	 * @param n dividend's value
	 * @param k divisor's value
	 * @return smallest integer greater than or equal to (dividend / divisor)
	 */
	static unsigned int ceiling(unsigned int n, unsigned int k) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Converts ASCII string's characters to upper-case. The conversion can be
	 * done in-place, i.e. the function can safely overwrite the input buffer.
	 * @param dest output buffer
	 * @param src nul-terminated input string
	 * @return output string's length in bytes
	 */
	static unsigned int toUpperCase(char *dest, const char *src) noexcept;
	/**
	 * Converts ASCII string's characters to lower-case. The conversion can be
	 * done in-place, i.e. the function can safely overwrite the source buffer.
	 * @param dest output buffer
	 * @param src nul-terminated input string
	 * @return output string's length in bytes
	 */
	static unsigned int toLowerCase(char *dest, const char *src) noexcept;
	/**
	 * XORs two byte sequences together. The conversion can be done in-place,
	 * the result can be safely stored into one of the input buffers.
	 * @param dest output buffer
	 * @param s1 first byte sequence
	 * @param s2 second byte sequence
	 * @param length number of bytes
	 */
	static void xorString(unsigned char *dest, const unsigned char *s1,
			const unsigned char *s2, unsigned int length) noexcept;
	/**
	 * Replaces the last occurrence of a character in an ASCII-string with nul
	 * and returns that position.
	 * @param s input string
	 * @param delimiter character to replace
	 * @return pointer to the given character's last occurrence in the string,
	 * nullptr if the character is not found.
	 */
	static char* stripLast(char *s, char delimiter) noexcept;
	/**
	 * Removes all whitespace characters from an ASCII string.
	 * @param s input string
	 * @return modified input string
	 */
	static char* removeWhitespace(char *s) noexcept;
	/**
	 * Trims an ASCII string (removes leading and trailing backspace characters).
	 * @param s nul-terminated string
	 * @param len stores trimmed string's length
	 * @return modified input string
	 */
	static char* trim(char *s, unsigned int &len) noexcept;
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_TWIDDLER_H_ */

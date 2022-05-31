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
 * @ref https://graphics.stanford.edu/~seander/bithacks.html
 */
class Twiddler {
public:
	/**
	 * Calculates the maximum of two numbers.
	 * @param x the first value
	 * @param y the second value
	 * @return the maximum value
	 */
	static unsigned int max(unsigned int x, unsigned int y) noexcept;
	/**
	 * Calculates the minimum of two numbers.
	 * @param x the first value
	 * @param y the second value
	 * @return the minimum value
	 */
	static unsigned int min(unsigned int x, unsigned int y) noexcept;
	/**
	 * Determines if the given value is power of two (2).
	 * @param x the value
	 * @return true if the given value is power of two, false otherwise
	 */
	static bool isPower2(unsigned int x) noexcept;
	/**
	 * Performs modulus arithmetic where the divisor is a power of two.
	 * @param n the value to divide
	 * @param s the power of two divisor
	 * @return the remainder
	 */
	static unsigned int modPow2(unsigned int n, unsigned int s) noexcept;
	/**
	 * Performs modulus arithmetic where the divisor is a value of two raised
	 * to the power of the given exponent.
	 * @param n the value to divide
	 * @param exp the exponent
	 * @return the remainder
	 */
	static unsigned int modExp2(unsigned int n, unsigned int exp) noexcept;
	/**
	 * Efficiently swaps the two numbers.
	 * @param x the first value
	 * @param y the second value
	 */
	static void exchange(unsigned int &x, unsigned int &y) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Calculates a power of two number which is less than or equal to the
	 * given value.
	 * @param x the value
	 * @return greatest power-of-two integer less than or equal to the value
	 */
	static unsigned int power2Floor(unsigned int x) noexcept;
	/**
	 * Calculates a power of two number which is greater than or equal to the
	 * given value.
	 * @param x the value
	 * @return least power-of-two integer greater than or equal to the value
	 */
	static unsigned int power2Ceil(unsigned int x) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Thomas wang's mixing function: for 32-bit integers.
	 * @param i the 32-bit value
	 * @return 32-bit hash value
	 */
	static unsigned long mix(unsigned long i) noexcept;
	/**
	 * Thomas wang's mixing function: for 64-bit integers.
	 * @param l the 64-bit value
	 * @return 64-bit hash value
	 */
	static unsigned long long mix(unsigned long long l) noexcept;
	/**
	 * Tomas wang's 64-bit to 32-bit hash function.
	 * @param l the 64-bit value
	 * @return 32-bit hash value
	 */
	static unsigned long hash(unsigned long long l) noexcept;
	//-----------------------------------------------------------------
	/**
	 * FVN-1a hash (Fowler–Noll–Vo hash) with 64-bit output.
	 * @param data value for hashing
	 * @param bytes data's size in bytes
	 * @return 64-bit hash value
	 */
	static unsigned long long FVN1aHash(const void *data,
			unsigned int bytes) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Interval function: Checks whether the given value  is inside an open
	 * circular interval.
	 * @param value the value to check
	 * @param from lower bound of the open circular interval
	 * @param to upper bound of the open circular interval
	 * @return true if the given value lies within the open circular interval,
	 * false otherwise.
	 */
	static bool isBetween(unsigned int value, unsigned int from,
			unsigned int to) noexcept;
	/**
	 * Interval function: Checks whether the given value  is inside a closed
	 * circular interval.
	 * @param value the value to check
	 * @param from lower bound of the closed circular interval
	 * @param to upper bound of the closed circular interval
	 * @return true if the given value lies within the closed circular interval,
	 * false otherwise.
	 */
	static bool isInRange(unsigned int value, unsigned int from,
			unsigned int to) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Bitwise operation: conditionally sets or clears the given bits.
	 * @param word the 32-bit operand
	 * @param bitmask bits to apply
	 * @param set if true then the given bits are set, otherwise the given bits
	 * are cleared.
	 * @return result of the operation
	 */
	static uint32_t mask(uint32_t word, uint32_t bitmask, bool set) noexcept;
	/**
	 * Bitwise operation: sets the given bits.
	 * @param word the 32-bit operand
	 * @param bitmask the bits to set
	 * @return result of the operation (operand | bitmask)
	 */
	static uint32_t set(uint32_t word, uint32_t bitmask) noexcept;
	/**
	 * Bitwise operation: clears the given bits.
	 * @param word the 32-bit operand
	 * @param bitmask the bits to clear
	 * @return result of the operation (operand & ~bitmask)
	 */
	static uint32_t clear(uint32_t word, uint32_t bitmask) noexcept;
	/**
	 * Bitwise operation: tests the given bits.
	 * @param word the 32-bit operand
	 * @param bitmask the bits to test
	 * @return true if at least one of the bits is set, false otherwise (the truth
	 * value of (operand & bitmask))
	 */
	static uint32_t test(uint32_t word, uint32_t bitmask) noexcept;
	/**
	 * Bitwise operation: merges bits from two values according to a mask.
	 * @param x  the first value
	 * @param y the second value
	 * @param bitmask the mask, 0 where bits from the first value should be
	 * selected, 1 where from the second value.
	 * @return result of the operation ((first & ~bitmask) | (second & bitmask))
	 */
	static uint32_t merge(uint32_t x, uint32_t y, uint32_t bitmask) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Fast modulus 8 of an unsigned integer.
	 * @param x the value
	 * @return remainder of (value / 8)
	 */
	static unsigned int mod8(unsigned int x) noexcept;
	/**
	 * Fast division by 8 of an unsigned integer.
	 * @param x the value
	 * @return result of (value / 8)
	 */
	static unsigned int div8(unsigned int x) noexcept;
	/**
	 * Fast multiplication with 8 of an unsigned integer.
	 * @param x the value
	 * @return result of (value * 8)
	 */
	static unsigned int mult8(unsigned int x) noexcept;
	/**
	 * Counts the number of set bits in a 8-bit unsigned integer.
	 * @param v the 8-bit unsigned value
	 * @return the number of set bits
	 */
	static unsigned int bitCount(unsigned char v) noexcept;
	/**
	 * Count the number of bits in a 32-bit integer.
	 * @param v the 32-bit unsigned value
	 * @return the number of set bits
	 */
	static unsigned int bitCount(uint32_t v) noexcept;
	/**
	 * Returns the first set bit's position from right in a 8-bit unsigned integer.
	 * @param v the 8-bit unsigned value
	 * @return index from right of the first set bit, 8 if none of the bits are
	 * set (value = 0).
	 */
	static unsigned int bitOrdinal(unsigned char v) noexcept;
	/**
	 * Calculates the number of 8-bit slots required for storing the given number
	 * of bits.
	 * @param bits the bits count
	 * @return how many 8-bit slots are required to store the given number of bits
	 */
	static unsigned int bitNSlots(unsigned int bits) noexcept;
	/**
	 * Calculates the 8-bit slot's position in a bit array that corresponds to
	 * the given bit index.
	 * @param index the bit index
	 * @return to which 8-bit slot does a bit at the given index belong
	 */
	static unsigned int bitSlot(unsigned int index) noexcept;
	/**
	 * Calculates the bitmask to apply on the 8-bit slot in a bit array that
	 * corresponds to the given bit index.
	 * @param index the bit index
	 * @return the bitmask for bitwise operations which effect the bit at the
	 * given index
	 */
	static unsigned char bitMask(unsigned int index) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Bit array operation: sets bit at the given index in the bit array.
	 * @param bitmap the bit array
	 * @param index the index to set
	 */
	static void set(unsigned char *bitmap, unsigned int index) noexcept;
	/**
	 * Bit array operation: clears bit at the given index in the bit array.
	 * @param bitmap the bit array
	 * @param index the index to clear
	 */
	static void clear(unsigned char *bitmap, unsigned int index) noexcept;
	/**
	 * Bit array operation: toggles bit (if the bit is set then it is cleared, if
	 * the bit is clear then it is set) at the given index in the bit array.
	 * @param bitmap the bit aray
	 * @param index the index to toggle.
	 */
	static void toggle(unsigned char *bitmap, unsigned int index) noexcept;
	/**
	 * Bit array operation: checks if bit at the given index is set in the bit
	 * array.
	 * @param bitmap the bit array
	 * @param index the index to test
	 * @return true if bit at the given index is set, false otherwise
	 */
	static bool test(const unsigned char *bitmap, unsigned int index) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the smallest multiple of alignment greater than or equal to the
	 * given size. The alignment must be power of two.
	 * @param size size of an object
	 * @param alignment power-of-two value describing alignment or byte boundary
	 * @return value that is equal to or greater than the given size and fits
	 * the given alignment/byte-boundary.
	 */
	static unsigned int align(unsigned int size,
			unsigned int alignment) noexcept;
	/**
	 * Calculates the least value greater than or equal to the result of a
	 * division operation.
	 * @param n dividend's value
	 * @param k divisor's value
	 * @return the smallest number greater than or equal to (dividend / divisor)
	 */
	static unsigned int ceiling(unsigned int n, unsigned int k) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Converts an ASCII string's characters to upper-case. The conversion can be
	 * done in-place, i.e. the function can safely overwrite the input buffer.
	 * @param dest buffer for storing the nul-terminated output
	 * @param src the input string (should be nul-terminated)
	 * @return length of the output string in bytes
	 */
	static unsigned int toUpperCase(char *dest, const char *src) noexcept;
	/**
	 * Converts an ASCII string's characters to lower-case. The conversion can be
	 * done in-place, i.e. the function can safely overwrite the source buffer.
	 * @param dest buffer for storing the nul-terminated output
	 * @param src the input string (should be nul-terminated)
	 * @return length of the output string in bytes
	 */
	static unsigned int toLowerCase(char *dest, const char *src) noexcept;
	/**
	 * XORs the two byte sequences together. The conversion can be done in-place,
	 * i.e. the function can safely store the result in one of the source buffers.
	 * @param dest pointer to the buffer for storing the output
	 * @param s1 pointer to the first byte sequence
	 * @param s2 pointer to the second byte sequence
	 * @param length number of bytes to update
	 */
	static void xorString(unsigned char *dest, const unsigned char *s1,
			const unsigned char *s2, unsigned int length) noexcept;
	/**
	 * Replaces the last occurrence of the given character in the ASCII-string
	 * with nul and returns that position.
	 * @param s the string to modify
	 * @param delimiter the character to replace
	 * @return the last position of the given character in the string, nullptr
	 * if the character not found.
	 */
	static char* stripLast(char *s, char delimiter) noexcept;
	/**
	 * Removes all whitespace characters from an ASCII string.
	 * @param s nul-terminated string
	 * @return pointer to the original string
	 */
	static char* removeWhitespace(char *s) noexcept;
	/**
	 * Trims an ASCII string (removes leading and trailing backspace characters).
	 * @param s nul-terminated string
	 * @param len the final string length
	 * @return pointer to the original string
	 */
	static char* trim(char *s, unsigned int &len) noexcept;
private:
	//Pre-calculated lookup table of number of bits in a 8-bit byte
	static const unsigned char bitCount_[256];
	//Pre-calculated lookup table of index to first set bit (from right) in a 8-bit byte
	static const unsigned char bitOrdinal_[256];
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_TWIDDLER_H_ */

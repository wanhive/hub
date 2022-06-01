/*
 * Serializer.h
 *
 * Byte order converter
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_SERIALIZER_H_
#define WH_BASE_DS_SERIALIZER_H_
#include "../common/defines.h"
#include <cinttypes>
#include <cstdarg>
#include <cstddef>

namespace wanhive {
/**
 * Platform independent serializer and deserializer.
 * Adapted from Beej’s Guide for Network Programming.
 * @ref https://beej.us/guide/bgnet/html/#serialization
 * @ref https://commandcenter.blogspot.com/2012/04/byte-order-fallacy.html
 */
class Serializer {
public:
	/**
	 * Stores data dictated by the format string in the buffer.
	 *
	 *   bits |signed   unsigned   float   string   blob
	 *   -----+-----------------------------------------
	 *      8 |   c        C
	 *     16 |   h        H         f
	 *     32 |   l        L         d
	 *     64 |   q        Q         g
	 *      - |                               s      b
	 *
	 * 16-bit unsigned length is automatically prepended to strings. The format
	 * character 'b' should be prefixed with the blob size which is prepended at
	 * the beginning of the blob as 16-bit unsigned value.
	 * @param buf the output buffer
	 * @param format the data format string
	 * @return number of bytes written into the buffer, 0 on error
	 */
	static size_t pack(unsigned char *buf, const char *format, ...) noexcept;
	/**
	 * Stores data dictated by the format string into the buffer.
	 * @param buf the output buffer
	 * @param format the data format string
	 * @param ap list of additional arguments
	 * @return number of bytes written into the buffer, 0 on error
	 */
	static size_t vpack(unsigned char *buf, const char *format,
			va_list ap) noexcept;
	/**
	 * Stores data dictated by the format string into the buffer. This one is
	 * the error checking version which detects and fails on buffer overflow.
	 * @param buf the output buffer
	 * @param size output buffer's size in bytes
	 * @param format the data format string
	 * @return number of bytes written into the buffer, 0 on error
	 */
	static size_t pack(unsigned char *buf, size_t size, const char *format,
			...) noexcept;
	/**
	 * Stores data dictated by the format string into the buffer. This one is
	 * the error checking version which detects and fails on buffer overflow.
	 * @param buf the output buffer
	 * @param size output buffer's size in bytes
	 * @param format the data format string
	 * @param ap list of additional arguments
	 * @return number of bytes written into the buffer, 0 on error
	 */
	static size_t vpack(unsigned char *buf, size_t size, const char *format,
			va_list ap) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Unpacks data dictated by the format string from the buffer.
	 *   bits |signed   unsigned   float   string   blob
	 *   -----+-----------------------------------------
	 *      8 |   c        C
	 *     16 |   h        H         f
	 *     32 |   l        L         d
	 *     64 |   q        Q         g
	 *      - |                               s      b
	 *
	 * String/blob is extracted based on its stored length, but 's' or 'b' can
	 * be prepended with a max length, which in case of a string must account
	 * for the NUL-terminator as well.
	 * @param buf the input buffer
	 * @param format the data format string
	 * @return number of transferred bytes, 0 on error
	 */
	static size_t unpack(const unsigned char *buf, const char *format,
			...) noexcept;
	/**
	 * Unpacks data dictated by the format string from the buffer.
	 * @param buf the input buffer
	 * @param format the data format string
	 * @param ap list of additional arguments (pointers to objects where the
	 * output data will be stored).
	 * @return number of transferred bytes, 0 on error
	 */
	static size_t vunpack(const unsigned char *buf, const char *format,
			va_list ap) noexcept;
	/**
	 * Unpacks data dictated by the format string from the buffer. This one is
	 * the error checking version which detects and fails on buffer overflow.
	 * @param buf the input buffer
	 * @param size input buffer's size in bytes
	 * @param format the data format string
	 * @return number of transferred bytes, 0 on error
	 */
	static size_t unpack(const unsigned char *buf, size_t size,
			const char *format, ...) noexcept;
	/**
	 * Unpacks data dictated by the format string from the buffer. This one is
	 * the error checking version which detects and fails on buffer overflow.
	 * @param buf the input buffer
	 * @param size input buffer's size in bytes
	 * @param format the data format string
	 * @param ap list of additional arguments (pointers to objects where the
	 * output data will be stored).
	 * @return number of transferred bytes, 0 on error
	 */
	static size_t vunpack(const unsigned char *buf, size_t size,
			const char *format, va_list ap) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Copies a sequence of bytes into a buffer.
	 * @param dest the output buffer (non-aliased)
	 * @param src the input data
	 * @param length number of bytes to transfer
	 */
	static void packib(unsigned char *dest, const unsigned char *src,
			size_t length) noexcept;
	/**
	 * Packs a 8-bit integer into a buffer.
	 * @param buf the output buffer
	 * @param i the 8-bit integer value
	 */
	static void packi8(unsigned char *buf, uint8_t i) noexcept;
	/**
	 * Packs a 16-bit integer into a buffer (like htons(3)).
	 * @param buf the output buffer
	 * @param i the 16-bit integer value
	 */
	static void packi16(unsigned char *buf, uint16_t i) noexcept;
	/**
	 * Packs a 32-bit integer into a buffer (like htonl(3)).
	 * @param buf the output buffer
	 * @param i the 32-bit integer value
	 */
	static void packi32(unsigned char *buf, uint32_t i) noexcept;
	/**
	 * Packs a 64-bit integer into a buffer.
	 * @param buf the output buffer
	 * @param i the 64-bit integer value
	 */
	static void packi64(unsigned char *buf, uint64_t i) noexcept;
	/**
	 * Packs a 16-bit half precision floating point number into a buffer.
	 * @param buf the output buffer
	 * @param i the decimal value
	 */
	static void packf16(unsigned char *buf, long double f) noexcept;
	/**
	 * Packs a 32-bit single precision floating point number into a buffer.
	 * @param buf the output buffer
	 * @param i the decimal value
	 */
	static void packf32(unsigned char *buf, long double f) noexcept;
	/**
	 * Packs a 64-bit double precision floating point number into a buffer.
	 * @param buf the output buffer
	 * @param i the decimal value
	 */
	static void packf64(unsigned char *buf, long double f) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reverse of Serializer()::packib(). Copies a sequence of bytes into
	 * a buffer.
	 * @param dest the output buffer (non-aliased)
	 * @param src the input data
	 * @param length number of bytes to transfer
	 */
	static void unpackib(unsigned char *dest, const unsigned char *src,
			unsigned int length) noexcept;
	/**
	 * Unpacks a 8-bit unsigned integer from a buffer.
	 * @param buf the input buffer
	 * @return 8-bit unsigned integer value
	 */
	static uint8_t unpacku8(const unsigned char *buf) noexcept;
	/**
	 * Unpacks a 8-bit signed integer from a buffer.
	 * @param buf the input buffer
	 * @return 8-bit signed integer value
	 */
	static int8_t unpacki8(const unsigned char *buf) noexcept;
	/**
	 * Unpacks a 16-bit unsigned integer from a buffer (like ntohs(3)).
	 * @param buf the input buffer
	 * @return 16-bit unsigned integer value
	 */
	static uint16_t unpacku16(const unsigned char *buf) noexcept;
	/**
	 * Unpacks a 16-bit signed integer from a buffer.
	 * @param buf the input buffer
	 * @return 16-bit signed integer value
	 */
	static int16_t unpacki16(const unsigned char *buf) noexcept;
	/**
	 * Unpacks a 32-bit unsigned integer from a buffer (like ntohl(3)).
	 * @param buf the input buffer
	 * @return 32-bit unsigned integer value
	 */
	static uint32_t unpacku32(const unsigned char *buf) noexcept;
	/**
	 * Unpacks a 32-bit signed integer from a buffer.
	 * @param buf the input buffer
	 * @return 32-bit signed integer value
	 */
	static int32_t unpacki32(const unsigned char *buf) noexcept;
	/**
	 * Unpacks a 64-bit unsigned integer from a buffer.
	 * @param buf the input buffer
	 * @return 64-bit unsigned integer value
	 */
	static uint64_t unpacku64(const unsigned char *buf) noexcept;
	/**
	 * Unpacks a 64-bit signed integer from a buffer.
	 * @param buf the input buffer
	 * @return 64-bit signed integer value
	 */
	static int64_t unpacki64(const unsigned char *buf) noexcept;
	/**
	 * Unpacks a half precision floating point number from a buffer.
	 * @param buf the input buffer
	 * @return decimal value
	 */
	static float unpackf16(const unsigned char *buf) noexcept;
	/**
	 * Unpacks a single precision floating point number from a buffer.
	 * @param buf the input buffer
	 * @return decimal value
	 */
	static float unpackf32(const unsigned char *buf) noexcept;
	/**
	 * Unpacks a double precision floating point number from a buffer.
	 * @param buf the input buffer
	 * @return decimal value
	 */
	static double unpackf64(const unsigned char *buf) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Converts a floating point number into IEEE-754 format.
	 *   type             |size    exponent
	 *   -----------------+----------------
	 *   Half precision   |  16           5
	 *   Single precision |  32           8
	 *   Double precision |  64          11
	 *
	 * @param f the decimal value
	 * @param bits input's size in bits
	 * @param expbits number of exponent bits
	 * @return IEEE-754 formatted value
	 */
	static unsigned long long pack754(long double f, unsigned bits,
			unsigned expbits) noexcept;
	/**
	 * Converts a floating point number from IEEE-754 format.
	 *   type             |size    exponent
	 *   -----------------+----------------
	 *   Half precision   |  16           5
	 *   Single precision |  32           8
	 *   Double precision |  64          11
	 *
	 * @param i IEEE-754 formatted value
	 * @param bits size of floating point number in bits
	 * @param expbits number of exponent bits
	 * @return decimal value
	 */
	static long double unpack754(unsigned long long i, unsigned bits,
			unsigned expbits) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Self-test for debugging.
	 */
	static void test() noexcept;
private:
	WH_STATIC_ASSERT(sizeof(float) == 4, "Platform is not IEEE-754 compliant");
	WH_STATIC_ASSERT(sizeof(double) == 8, "Platform is not IEEE-754 compliant");
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_SERIALIZER_H_ */

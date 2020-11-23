/*
 * Serializer.h
 *
 * Host-to-network serializer and deserializer
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
 * Platform independent serializer and deserializer for big endian streams.
 * Adapted from Beej’s Guide for Network Programming.
 * REF: https://beej.us/guide/bgnet/html/#serialization
 * REF: https://commandcenter.blogspot.com/2012/04/byte-order-fallacy.html
 */
class Serializer {
public:
	/*
	 ** pack() -- store data dictated by the format string in the buffer
	 **
	 **   bits |signed   unsigned   float   string   blob
	 **   -----+-----------------------------------------
	 **      8 |   c        C
	 **     16 |   h        H         f
	 **     32 |   l        L         d
	 **     64 |   q        Q         g
	 **      - |                               s      b
	 **
	 **  (16-bit unsigned length is automatically prepended to strings)
	 **  ('b' should be prefixed with blob size which is prepended at the beginning
	 **  of the blob as 16-bit unsigned value)
	 **  Returns the number of transferred bytes (potentially 0), 0 on error
	 */
	static size_t pack(unsigned char *buf, const char *format, ...) noexcept;
	static size_t vpack(unsigned char *buf, const char *format,
			va_list ap) noexcept;
	//The overflow checking versions (fail on overflow)
	static size_t pack(unsigned char *buf, size_t size, const char *format,
			...) noexcept;
	static size_t vpack(unsigned char *buf, size_t size, const char *format,
			va_list ap) noexcept;
	/*
	 ** unpack() -- unpack data dictated by the format string into the buffer
	 **
	 **   bits |signed   unsigned   float   string   blob
	 **   -----+-----------------------------------------
	 **      8 |   c        C
	 **     16 |   h        H         f
	 **     32 |   l        L         d
	 **     64 |   q        Q         g
	 **      - |                               s      b
	 **
	 **  (string/blob is extracted based on its stored length, but 's' or 'b'
	 **  can be prepended with a max length, which in case of a string must account
	 **  for the NUL-terminator as well)
	 **  Returns the number of transferred bytes (potentially 0), 0 on error
	 */
	static size_t unpack(const unsigned char *buf, const char *format,
			...) noexcept;
	static size_t vunpack(const unsigned char *buf, const char *format,
			va_list ap) noexcept;
	//The overflow checking versions (fail on overflow)
	static size_t unpack(const unsigned char *buf, size_t size,
			const char *format, ...) noexcept;
	static size_t vunpack(const unsigned char *buf, size_t size,
			const char *format, va_list ap) noexcept;
	//=================================================================
	/**
	 * Caller must pass valid pointer, length and index
	 */
	//packib() -- store <length> bytes from <src> into <dest> (non-aliased)
	static void packib(unsigned char *dest, const unsigned char *src,
			size_t length) noexcept;
	//packi8() -- store a 8-bit int into a buffer (like *buf=i)
	static void packi8(unsigned char *buf, uint8_t i) noexcept;
	//packi16() -- store a 16-bit int into a buffer (like htons())
	static void packi16(unsigned char *buf, uint16_t i) noexcept;
	//packi32() -- store a 32-bit int into a buffer (like htonl())
	static void packi32(unsigned char *buf, uint32_t i) noexcept;
	//packi64() -- store a 64-bit int into a buffer (like htonl())
	static void packi64(unsigned char *buf, uint64_t i) noexcept;
	//packf16() -- store a 16-bit half precision real number into a buffer
	static void packf16(unsigned char *buf, long double f) noexcept;
	//packf32() -- store a 32-bit single precision real number into a buffer
	static void packf32(unsigned char *buf, long double f) noexcept;
	//packf64() -- store a 64-bit double precision real number into a buffer
	static void packf64(unsigned char *buf, long double f) noexcept;

	//unpackib() -- reverse of packib (<src> and <dest> must be non-aliased)
	static void unpackib(unsigned char *dest, const unsigned char *src,
			unsigned int length) noexcept;
	//unpacku8() -- unpack a 8-bit unsigned from a buffer
	static uint8_t unpacku8(const unsigned char *buf) noexcept;
	//unpacki8() -- unpack a 8-bit int from a buffer
	static int8_t unpacki8(const unsigned char *buf) noexcept;
	//unpacku16() -- unpack a 16-bit unsigned from a buffer (like ntohs())
	static uint16_t unpacku16(const unsigned char *buf) noexcept;
	//unpacki16() -- unpack a 16-bit int from a buffer (like ntohs())
	static int16_t unpacki16(const unsigned char *buf) noexcept;
	//unpacku32() -- unpack a 32-bit unsigned from a buffer (like ntohl())
	static uint32_t unpacku32(const unsigned char *buf) noexcept;
	//unpacki32() -- unpack a 32-bit int from a buffer (like ntohl())
	static int32_t unpacki32(const unsigned char *buf) noexcept;
	//unpacku64() -- unpack a 64-bit unsigned from a buffer (like ntohll())
	static uint64_t unpacku64(const unsigned char *buf) noexcept;
	//unpacki64() -- unpack a 64-bit int from a buffer (like ntohll())
	static int64_t unpacki64(const unsigned char *buf) noexcept;
	//unpackf16() -- unpack a half precision real number from a buffer
	static float unpackf16(const unsigned char *buf) noexcept;
	//unpackf32() -- unpack a single precision real number from a buffer
	static float unpackf32(const unsigned char *buf) noexcept;
	//unpackf64() -- unpack a double precision real number from a buffer
	static double unpackf64(const unsigned char *buf) noexcept;
	/*
	 ** pack754() -- pack a floating point number into IEEE-754 format
	 ** Half precision Float (16-bit): bits=16, expbits=5
	 ** Single precision Float (32-bit): bits=32, expbits=8
	 ** Double precision Double (64-bit): bits=64, expbits=11
	 */
	static unsigned long long pack754(long double f, unsigned bits,
			unsigned expbits) noexcept;
	/*
	 ** unpack754() -- unpack a floating point number from IEEE-754 format
	 ** Half precision Float (16-bit): bits=16, expbits=5
	 ** Single precision Float (32-bit): bits=32, expbits=8
	 ** Double precision Double (64-bit): bits=64, expbits=11
	 */
	static long double unpack754(unsigned long long i, unsigned bits,
			unsigned expbits) noexcept;

	//Sanity test
	static void test() noexcept;
private:
	WH_STATIC_ASSERT(sizeof(float) == 4, "Platform is not IEEE-754 compliant");
	WH_STATIC_ASSERT(sizeof(double) == 8, "Platform is not IEEE-754 compliant");
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_SERIALIZER_H_ */

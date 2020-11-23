/*
 * Serializer.cpp
 *
 * Host-to-network serializer and deserializer
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Serializer.h"
#include <cctype>
#include <cfloat>
#include <cstdio>
#include <cstring>
#include <endian.h>

namespace wanhive {

size_t Serializer::pack(unsigned char *buf, const char *format, ...) noexcept {
	va_list ap;
	va_start(ap, format);
	size_t ret = vpack(buf, format, ap);
	va_end(ap);
	return ret;
}

size_t Serializer::vpack(unsigned char *buf, const char *format,
		va_list ap) noexcept {
	union {
		int8_t c; // 8-bit
		uint8_t C;

		int16_t h; // 16-bit
		uint16_t H;

		int32_t l; // 32-bit
		uint32_t L;

		int64_t q; // 64-bit
		uint64_t Q;

		float f;
		float d;
		double g;

		const char *s; // strings
		const unsigned char *b; //blobs
	};

	if (!buf || !format) {
		return 0;
	}
	size_t len = 0; //Cannot declare inside switch block (syntax error)
	//Preserve initial position, will use it to calculate the number of transferred bytes
	unsigned char *mark = buf;

	for (; *format != '\0'; ++format) {
		switch (*format) {
		case 'c': // 8-bit
			c = (int8_t) va_arg(ap, int); // promoted
			*buf++ = c;
			break;

		case 'C': // 8-bit unsigned
			C = (uint8_t) va_arg(ap, unsigned int); // promoted
			*buf++ = C;
			break;

		case 'h': // 16-bit
			h = (int16_t) va_arg(ap, int);
			packi16(buf, h);
			buf += sizeof(int16_t);
			break;

		case 'H': // 16-bit unsigned
			H = (uint16_t) va_arg(ap, unsigned int);
			packi16(buf, H);
			buf += sizeof(uint16_t);
			break;

		case 'l': // 32-bit
			l = va_arg(ap, int32_t);
			packi32(buf, l);
			buf += sizeof(int32_t);
			break;

		case 'L': // 32-bit unsigned
			L = va_arg(ap, uint32_t);
			packi32(buf, L);
			buf += sizeof(uint32_t);
			break;

		case 'q': // 64-bit
			q = va_arg(ap, int64_t);
			packi64(buf, q);
			buf += sizeof(int64_t);
			break;

		case 'Q': // 64-bit unsigned
			Q = va_arg(ap, uint64_t);
			packi64(buf, Q);
			buf += sizeof(uint64_t);
			break;

		case 'f': // float-16
			f = (float) va_arg(ap, double); // promoted
			packf16(buf, f);
			buf += sizeof(uint16_t);
			break;

		case 'd': // float-32
			d = (float) va_arg(ap, double); // promoted
			packf32(buf, d);
			buf += sizeof(uint32_t);
			break;

		case 'g': // float-64
			g = va_arg(ap, double);
			packf64(buf, g);
			buf += sizeof(uint64_t);
			break;

		case 's': // string
			s = va_arg(ap, const char*);
			len = strlen(s) & 0xffff;
			packi16(buf, len);
			buf += sizeof(uint16_t);
			packib(buf, (const unsigned char*) s, len);
			buf += len;
			break;

		case 'b': // blob
			b = va_arg(ap, const unsigned char*);
			packi16(buf, len);
			buf += sizeof(uint16_t);
			packib(buf, b, len);
			buf += len;
			break;

		default:
			if (isdigit(*format)) {
				//track blob size
				len = len * 10 + (*format - '0');
			} else {
				//invalid format character
				return 0;
			}
			break;
		}
		if (!isdigit(*format)) {
			len = 0;
		}
	}
	return (buf - mark);
}

size_t Serializer::pack(unsigned char *buf, size_t size, const char *format,
		...) noexcept {
	va_list ap;
	va_start(ap, format);
	size_t ret = vpack(buf, size, format, ap);
	va_end(ap);
	return ret;
}

size_t Serializer::vpack(unsigned char *buf, size_t size, const char *format,
		va_list ap) noexcept {
	union {
		int8_t c; // 8-bit
		uint8_t C;

		int16_t h; // 16-bit
		uint16_t H;

		int32_t l; // 32-bit
		uint32_t L;

		int64_t q; // 64-bit
		uint64_t Q;

		float f;
		float d;
		double g;

		const char *s; // strings
		const unsigned char *b; //blobs
	};

	if (!buf || !size || !format) {
		return 0;
	}
	size_t len = 0; //Cannot declare inside switch block (syntax error)
	//Preserve initial position, will use it to calculate the number of transferred bytes
	unsigned char *mark = buf;
	//Preserve the end position
	const unsigned char *end = buf + size;

	for (; *format != '\0'; ++format) {
		switch (*format) {
		case 'c': // 8-bit
			c = (int8_t) va_arg(ap, int); // promoted
			if (buf + sizeof(int8_t) <= end) {
				*buf++ = c;
			} else {
				return 0;
			}
			break;

		case 'C': // 8-bit unsigned
			C = (uint8_t) va_arg(ap, unsigned int); // promoted
			if (buf + sizeof(uint8_t) <= end) {
				*buf++ = C;
			} else {
				return 0;
			}
			break;

		case 'h': // 16-bit
			h = (int16_t) va_arg(ap, int);
			if (buf + sizeof(int16_t) <= end) {
				packi16(buf, h);
				buf += sizeof(int16_t);
			} else {
				return 0;
			}
			break;

		case 'H': // 16-bit unsigned
			H = (uint16_t) va_arg(ap, unsigned int);
			if (buf + sizeof(uint16_t) <= end) {
				packi16(buf, H);
				buf += sizeof(uint16_t);
			} else {
				return 0;
			}
			break;

		case 'l': // 32-bit
			l = va_arg(ap, int32_t);
			if (buf + sizeof(int32_t) <= end) {
				packi32(buf, l);
				buf += sizeof(int32_t);
			} else {
				return 0;
			}
			break;

		case 'L': // 32-bit unsigned
			L = va_arg(ap, uint32_t);
			if (buf + sizeof(uint32_t) <= end) {
				packi32(buf, L);
				buf += sizeof(uint32_t);
			} else {
				return 0;
			}
			break;

		case 'q': // 64-bit
			q = va_arg(ap, int64_t);
			if (buf + sizeof(int64_t) <= end) {
				packi64(buf, q);
				buf += sizeof(int64_t);
			} else {
				return 0;
			}
			break;

		case 'Q': // 64-bit unsigned
			Q = va_arg(ap, uint64_t);
			if (buf + sizeof(uint64_t) <= end) {
				packi64(buf, Q);
				buf += sizeof(uint64_t);
			} else {
				return 0;
			}
			break;

		case 'f': // float-16
			f = (float) va_arg(ap, double); // promoted
			if (buf + sizeof(uint16_t) <= end) {
				packf16(buf, f);
				buf += sizeof(uint16_t);
			} else {
				return 0;
			}
			break;

		case 'd': // float-32
			d = (float) va_arg(ap, double); // promoted
			if (buf + sizeof(uint32_t) <= end) {
				packf32(buf, d);
				buf += sizeof(uint32_t);
			} else {
				return 0;
			}
			break;

		case 'g': // float-64
			g = va_arg(ap, double);
			if (buf + sizeof(uint64_t) <= end) {
				packf64(buf, g);
				buf += sizeof(uint64_t);
			} else {
				return 0;
			}
			break;

		case 's': // string
			s = va_arg(ap, const char*);
			len = strlen(s) & 0xffff;
			if (buf + sizeof(uint16_t) + len <= end) {
				packi16(buf, len);
				buf += sizeof(uint16_t);
				packib(buf, (const unsigned char*) s, len);
				buf += len;
			} else {
				return 0;
			}
			break;

		case 'b': // blob
			b = va_arg(ap, const unsigned char*);
			if (buf + sizeof(uint16_t) + len <= end) {
				packi16(buf, len);
				buf += sizeof(uint16_t);
				packib(buf, b, len);
				buf += len;
			} else {
				return 0;
			}
			break;

		default:
			if (isdigit(*format)) {
				//track blob size
				len = len * 10 + (*format - '0');
			} else {
				//invalid format character
				return 0;
			}
			break;
		}
		if (!isdigit(*format)) {
			len = 0;
		}
	}
	return (buf - mark);
}

size_t Serializer::unpack(const unsigned char *buf, const char *format,
		...) noexcept {
	va_list ap;
	va_start(ap, format);
	size_t ret = vunpack(buf, format, ap);
	va_end(ap);
	return ret;
}

size_t Serializer::vunpack(const unsigned char *buf, const char *format,
		va_list ap) noexcept {
	union {
		int8_t *c; // 8-bit
		uint8_t *C;

		int16_t *h; // 16-bit
		uint16_t *H;

		int32_t *l; // 32-bit
		uint32_t *L;

		int64_t *q; // 64-bit
		uint64_t *Q;

		float *f;
		float *d;
		double *g;

		char *s; // strings
		unsigned char *b; //blobs
	};

	if (!buf || !format) {
		return 0;
	}

	size_t length = 0, maxLength = 0; //Cannot declare inside switch block (syntax error)
	//Preserve initial position, will use it to calculate the number of transferred bytes
	const unsigned char *mark = buf;

	for (; *format != '\0'; ++format) {
		switch (*format) {
		case 'c': // 8-bit
			c = va_arg(ap, int8_t*);
			*c = unpacki8(buf);
			buf += sizeof(int8_t);
			break;

		case 'C': // 8-bit unsigned
			C = va_arg(ap, uint8_t*);
			*C = unpacku8(buf);
			buf += sizeof(uint8_t);
			break;

		case 'h': // 16-bit
			h = va_arg(ap, int16_t*);
			*h = unpacki16(buf);
			buf += sizeof(int16_t);
			break;

		case 'H': // 16-bit unsigned
			H = va_arg(ap, uint16_t*);
			*H = unpacku16(buf);
			buf += sizeof(uint16_t);
			break;

		case 'l': // 32-bit
			l = va_arg(ap, int32_t*);
			*l = unpacki32(buf);
			buf += sizeof(int32_t);
			break;

		case 'L': // 32-bit unsigned
			L = va_arg(ap, uint32_t*);
			*L = unpacku32(buf);
			buf += sizeof(uint32_t);
			break;

		case 'q': // 64-bit
			q = va_arg(ap, int64_t*);
			*q = unpacki64(buf);
			buf += sizeof(int64_t);
			break;

		case 'Q': // 64-bit unsigned
			Q = va_arg(ap, uint64_t*);
			*Q = unpacku64(buf);
			buf += sizeof(uint64_t);
			break;

		case 'f': // float-16
			f = va_arg(ap, float*);
			*f = unpackf16(buf);
			buf += sizeof(uint16_t);
			break;

		case 'd': // float-32
			d = va_arg(ap, float*);
			*d = unpackf32(buf);
			buf += sizeof(uint32_t);
			break;

		case 'g': // float-64
			g = va_arg(ap, double*);
			*g = (double) unpackf64(buf);
			buf += sizeof(uint64_t);
			break;

		case 's': // string
			s = va_arg(ap, char*);
			length = unpacku16(buf);
			buf += sizeof(uint16_t);
			if (!maxLength || length < maxLength) {
				packib((unsigned char*) s, buf, length);
				s[length] = '\0';
				buf += length;
			} else {
				return 0; //overflow
			}
			break;

		case 'b': // blob
			b = va_arg(ap, unsigned char*);
			length = unpacku16(buf);
			buf += sizeof(uint16_t);
			if (!maxLength || length <= maxLength) {
				packib(b, buf, length);
				buf += length;
			} else {
				return 0; //overflow
			}
			break;

		default:
			if (isdigit(*format)) {
				//track max string/blob len
				maxLength = maxLength * 10 + (*format - '0');
			} else {
				//invalid format character
				return 0;
			}
			break;
		}

		if (!isdigit(*format)) {
			maxLength = 0;
		}
	}

	return (buf - mark);
}

size_t Serializer::unpack(const unsigned char *buf, size_t size,
		const char *format, ...) noexcept {
	va_list ap;
	va_start(ap, format);
	size_t ret = vunpack(buf, size, format, ap);
	va_end(ap);
	return ret;
}

size_t Serializer::vunpack(const unsigned char *buf, size_t size,
		const char *format, va_list ap) noexcept {
	union {
		int8_t *c; // 8-bit
		uint8_t *C;

		int16_t *h; // 16-bit
		uint16_t *H;

		int32_t *l; // 32-bit
		uint32_t *L;

		int64_t *q; // 64-bit
		uint64_t *Q;

		float *f;
		float *d;
		double *g;

		char *s; // strings
		unsigned char *b; //blobs
	};

	if (!buf || !size || !format) {
		return 0;
	}

	size_t length = 0, maxLength = 0; //Cannot declare inside switch block (syntax error)
	//Preserve initial position, will use it to calculate the number of transferred bytes
	const unsigned char *mark = buf;
	//Preserve the end position
	const unsigned char *end = buf + size;

	for (; *format != '\0'; ++format) {
		switch (*format) {
		case 'c': // 8-bit
			c = va_arg(ap, int8_t*);
			if (buf + sizeof(int8_t) <= end) {
				*c = unpacki8(buf);
				buf += sizeof(int8_t);
			} else {
				return 0;
			}
			break;

		case 'C': // 8-bit unsigned
			C = va_arg(ap, uint8_t*);
			if (buf + sizeof(uint8_t) <= end) {
				*C = unpacku8(buf);
				buf += sizeof(uint8_t);
			} else {
				return 0;
			}
			break;

		case 'h': // 16-bit
			h = va_arg(ap, int16_t*);
			if (buf + sizeof(int16_t) <= end) {
				*h = unpacki16(buf);
				buf += sizeof(int16_t);
			} else {
				return 0;
			}
			break;

		case 'H': // 16-bit unsigned
			H = va_arg(ap, uint16_t*);
			if (buf + sizeof(uint16_t) <= end) {
				*H = unpacku16(buf);
				buf += sizeof(uint16_t);
			} else {
				return 0;
			}
			break;

		case 'l': // 32-bit
			l = va_arg(ap, int32_t*);
			if (buf + sizeof(int32_t) <= end) {
				*l = unpacki32(buf);
				buf += sizeof(int32_t);
			} else {
				return 0;
			}
			break;

		case 'L': // 32-bit unsigned
			L = va_arg(ap, uint32_t*);
			if (buf + sizeof(uint32_t) <= end) {
				*L = unpacku32(buf);
				buf += sizeof(uint32_t);
			} else {
				return 0;
			}
			break;

		case 'q': // 64-bit
			q = va_arg(ap, int64_t*);
			if (buf + sizeof(int64_t) <= end) {
				*q = unpacki64(buf);
				buf += sizeof(int64_t);
			} else {
				return 0;
			}
			break;

		case 'Q': // 64-bit unsigned
			Q = va_arg(ap, uint64_t*);
			if (buf + sizeof(uint64_t) <= end) {
				*Q = unpacku64(buf);
				buf += sizeof(uint64_t);
			} else {
				return 0;
			}
			break;

		case 'f': // float-16
			f = va_arg(ap, float*);
			if (buf + sizeof(uint16_t) <= end) {
				*f = unpackf16(buf);
				buf += sizeof(uint16_t);
			} else {
				return 0;
			}
			break;

		case 'd': // float-32
			d = va_arg(ap, float*);
			if (buf + sizeof(uint32_t) <= end) {
				*d = unpackf32(buf);
				buf += sizeof(uint32_t);
			} else {
				return 0;
			}
			break;

		case 'g': // float-64
			g = va_arg(ap, double*);
			if (buf + sizeof(uint64_t) <= end) {
				*g = (double) unpackf64(buf);
				buf += sizeof(uint64_t);
			} else {
				return 0;
			}
			break;

		case 's': // string
			s = va_arg(ap, char*);
			if (buf + sizeof(uint16_t) <= end) {
				length = unpacku16(buf);
				buf += sizeof(uint16_t);
			} else {
				return 0;
			}

			if ((buf + length <= end) && (!maxLength || length < maxLength)) {
				packib((unsigned char*) s, buf, length);
				s[length] = '\0';
				buf += length;
			} else {
				return 0; //overflow
			}
			break;

		case 'b': // blob
			b = va_arg(ap, unsigned char*);
			if (buf + sizeof(uint16_t) <= end) {
				length = unpacku16(buf);
				buf += sizeof(uint16_t);
			} else {
				return 0;
			}

			if ((buf + length <= end) && (!maxLength || length <= maxLength)) {
				packib(b, buf, length);
				buf += length;
			} else {
				return 0; //overflow
			}
			break;

		default:
			if (isdigit(*format)) {
				//track max string/blob len
				maxLength = maxLength * 10 + (*format - '0');
			} else {
				//invalid format character
				return 0;
			}
			break;
		}

		if (!isdigit(*format)) {
			maxLength = 0;
		}
	}

	return (buf - mark);
}

void Serializer::packib(unsigned char *dest, const unsigned char *src,
		size_t length) noexcept {
	memcpy(dest, src, length);
}

void Serializer::packi8(unsigned char *buf, uint8_t i) noexcept {
	*buf = i;
}

void Serializer::packi16(unsigned char *buf, uint16_t i) noexcept {
	i = htobe16(i);
	memcpy(buf, &i, sizeof(uint16_t));
}

void Serializer::packi32(unsigned char *buf, uint32_t i) noexcept {
	i = htobe32(i);
	memcpy(buf, &i, sizeof(uint32_t));
}

void Serializer::packi64(unsigned char *buf, uint64_t i) noexcept {
	i = htobe64(i);
	memcpy(buf, &i, sizeof(uint64_t));
}

void Serializer::packf16(unsigned char *buf, long double f) noexcept {
	unsigned long long value = pack754(f, 16, 5);
	packi16(buf, value);
}

void Serializer::packf32(unsigned char *buf, long double f) noexcept {
	unsigned long long value = pack754(f, 32, 8);
	packi32(buf, value);
}

void Serializer::packf64(unsigned char *buf, long double f) noexcept {
	unsigned long long value = pack754(f, 64, 11);
	packi64(buf, value);
}

void Serializer::unpackib(unsigned char *dest, const unsigned char *src,
		unsigned int length) noexcept {
	memcpy(dest, src, length);
}

uint8_t Serializer::unpacku8(const unsigned char *buf) noexcept {
	return *buf;
}

int8_t Serializer::unpacki8(const unsigned char *buf) noexcept {
	uint8_t i = *buf;

	// change unsigned number to signed
	if (i <= 0x7f) {
		return i;
	} else {
		return (-1 - (int8_t) (0xffu - i));
	}
}

uint16_t Serializer::unpacku16(const unsigned char *buf) noexcept {
	uint16_t i;
	memcpy(&i, buf, sizeof(uint16_t));
	return be16toh(i);
}

int16_t Serializer::unpacki16(const unsigned char *buf) noexcept {
	uint16_t i;
	memcpy(&i, buf, sizeof(uint16_t));
	i = be16toh(i);

	//Change unsigned number to signed
	if (i <= 0x7fffu) {
		return i;
	} else {
		return (-1 - (int16_t) (0xffffu - i));
	}
}

uint32_t Serializer::unpacku32(const unsigned char *buf) noexcept {
	uint32_t i;
	memcpy(&i, buf, sizeof(uint32_t));
	return be32toh(i);
}

int32_t Serializer::unpacki32(const unsigned char *buf) noexcept {
	uint32_t i;
	memcpy(&i, buf, sizeof(uint32_t));
	i = be32toh(i);

	//Change unsigned number to signed
	if (i <= 0x7fffffffu) {
		return i;
	} else {
		return (-1 - (int32_t) (0xffffffffu - i));
	}
}

uint64_t Serializer::unpacku64(const unsigned char *buf) noexcept {
	uint64_t i;
	memcpy(&i, buf, sizeof(uint64_t));
	return be64toh(i);
}

int64_t Serializer::unpacki64(const unsigned char *buf) noexcept {
	uint64_t i;
	memcpy(&i, buf, sizeof(uint64_t));
	i = be64toh(i);

	//Change unsigned number to signed
	if (i <= 0x7fffffffffffffffu) {
		return i;
	} else {
		return (-1 - (int64_t) (0xffffffffffffffffu - i));
	}
}

float Serializer::unpackf16(const unsigned char *buf) noexcept {
	unsigned long long value = unpacku16(buf);
	return unpack754(value, 16, 5);
}

float Serializer::unpackf32(const unsigned char *buf) noexcept {
	unsigned long long value = unpacku32(buf);
	return unpack754(value, 32, 8);
}

double Serializer::unpackf64(const unsigned char *buf) noexcept {
	unsigned long long value = unpacku64(buf);
	return unpack754(value, 64, 11);
}

unsigned long long Serializer::pack754(long double f, unsigned bits,
		unsigned expbits) noexcept {
	if (f == 0.0) {
		return 0; // get this special case out of the way
	}

	long double fnorm;
	long long sign;

	// check sign and begin normalization
	if (f < 0) {
		sign = 1;
		fnorm = -f;
	} else {
		sign = 0;
		fnorm = f;
	}

	// get the normalized form of f and track the exponent
	int shift = 0;
	while (fnorm >= 2.0) {
		fnorm /= 2.0;
		shift++;
	}
	while (fnorm < 1.0) {
		fnorm *= 2.0;
		shift--;
	}
	fnorm = fnorm - 1.0;

	unsigned significandbits = bits - expbits - 1; // -1 for sign bit
	// calculate the binary form (non-float) of the significand data
	long long significand = fnorm * ((1LL << significandbits) + 0.5f);

	// get the biased exponent
	long long exp = shift + ((1 << (expbits - 1)) - 1); // shift + bias

	// return the final answer
	return (sign << (bits - 1)) | (exp << (bits - expbits - 1)) | significand;
}

long double Serializer::unpack754(unsigned long long i, unsigned bits,
		unsigned expbits) noexcept {
	if (i == 0) {
		return 0.0;
	}
	// pull the significand
	unsigned significandbits = bits - expbits - 1; // -1 for sign bit
	long double result = (i & ((1LL << significandbits) - 1)); // mask
	result /= (1LL << significandbits); // convert back to float
	result += 1.0f; // add the one back on

	// deal with the exponent
	unsigned bias = (1 << (expbits - 1)) - 1;
	long long shift = ((i >> significandbits) & ((1LL << expbits) - 1)) - bias;
	while (shift > 0) {
		result *= 2.0;
		shift--;
	}
	while (shift < 0) {
		result /= 2.0;
		shift++;
	}

	// sign it
	result *= (i >> (bits - 1)) & 1 ? -1.0 : 1.0;

	return result;
}

void Serializer::test() noexcept {
	unsigned char buf[1024];
	int x;

	int64_t k = 0, k2 = 0;
	int64_t test64[14] = { 0, -0, 1, 2, -1, -2, 0x7fffffffffffffffll >> 1,
			0x7ffffffffffffffell, 0x7fffffffffffffffll, -0x7fffffffffffffffll,
			-0x800000000000000ll, 9007199254740991ll, 9007199254740992ll,
			9007199254740993ll };

	uint64_t K = 0, K2 = 0;
	uint64_t testu64[14] = { 0, 0, 1, 2, 0, 0, 0xffffffffffffffffll >> 1,
			0xfffffffffffffffell, 0xffffffffffffffffll, 0, 0,
			9007199254740991ll, 9007199254740992ll, 9007199254740993ll };

	int32_t i = 0, i2 = 0;
	int32_t test32[14] =
			{ 0, -0, 1, 2, -1, -2, 0x7fffffffl >> 1, 0x7ffffffel, 0x7fffffffl,
					(int32_t) -0x7fffffffl, (int32_t) -0x80000000l, 0, 0, 0 };

	uint32_t I = 0, I2 = 0;
	uint32_t testu32[14] = { 0, 0, 1, 2, 0, 0, 0xffffffffl >> 1, 0xfffffffel,
			0xffffffffl, 0, 0, 0, 0, 0 };

	int16_t j = 0, j2 = 0;
	int16_t test16[14] = { 0, -0, 1, 2, -1, -2, 0x7fff >> 1, 0x7ffe, 0x7fff,
			-0x7fff, -0x8000, 0, 0, 0 };

	bool success = true;
	for (x = 0; x < 14; x++) {
		k = test64[x];
		pack(buf, "q", k);
		unpack(buf, "q", &k2);

		if (k2 != k) {
			success &= false;
			printf("64i(%d): %" PRId64 " != %" PRId64 "\n", x, k, k2);
			printf("  before: %016" PRIx64 "\n", k);
			printf("  after:  %016" PRIx64 "\n", k2);
			printf("  buffer: %02hhx %02hhx %02hhx %02hhx "
					" %02hhx %02hhx %02hhx %02hhx\n", buf[0], buf[1], buf[2],
					buf[3], buf[4], buf[5], buf[6], buf[7]);
		} else {

		}

		K = testu64[x];
		pack(buf, "Q", K);
		unpack(buf, "Q", &K2);

		if (K2 != K) {
			success &= false;
			printf("64u(%d): %" PRIu64 " != %" PRIu64 "\n", x, K, K2);
		} else {

		}

		i = test32[x];
		pack(buf, "l", i);
		i2 = 0;
		unpack(buf, "l", &i2);

		if (i2 != i) {
			success &= false;
			printf("32i(%d): %" PRId32 " != %" PRId32 "\n", x, i, i2);
			printf("  before: %08" PRIx32 "\n", i);
			printf("  after:  %08" PRIx32 "\n", i2);
			printf("  buffer: %02hhx %02hhx %02hhx %02hhx "
					" %02hhx %02hhx %02hhx %02hhx\n", buf[0], buf[1], buf[2],
					buf[3], buf[4], buf[5], buf[6], buf[7]);
		} else {

		}

		I = testu32[x];
		pack(buf, "L", I);
		I2 = 0;
		unpack(buf, "L", &I2);

		if (I2 != I) {
			success &= false;
			printf("32u(%d): %" PRIu32 " != %" PRIu32 "\n", x, I, I2);
		} else {

		}

		j = test16[x];
		pack(buf, "h", j);
		unpack(buf, "h", &j2);

		if (j2 != j) {
			success &= false;
			printf("16i(%d): %" PRId16 " != %" PRId16 "\n", x, j, j2);
		} else {

		}
	}

	if (true) {
		double testf64[8] = { -3490.6677, 0.0, 1.0, -1.0, DBL_MIN * 2,
		DBL_MAX / 2, DBL_MIN, DBL_MAX };
		double f, f2;

		for (i = 0; i < 8; i++) {
			f = testf64[i];
			pack(buf, "g", f);
			unpack(buf, "g", &f2);

			if (f2 != f) {
				success &= false;
				printf("f64: %f != %f\n", f, f2);
				printf("  buffer: %02hhx %02hhx %02hhx %02hhx "
						" %02hhx %02hhx %02hhx %02hhx\n", buf[0], buf[1],
						buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
			} else {

			}
		}
	}

	if (true) {
		float testf32[7] = { 0.0, 1.0, -1.0, 10, -3.6677, 3.1875, -3.1875 };
		float f, f2;

		for (i = 0; i < 7; i++) {
			f = testf32[i];
			pack(buf, "d", f);
			unpack(buf, "d", &f2);

			if (f2 != f) {
				success &= false;
				printf("f32: %.10f != %.10f\n", f, f2);
				printf("  buffer: %02hhx %02hhx %02hhx %02hhx "
						" %02hhx %02hhx %02hhx %02hhx\n", buf[0], buf[1],
						buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
			} else {

			}
		}
	}

	if (true) {
		float testf16[7] = { 0.0, 1.0, -1.0, 10, -10, 3.1875, -3.1875 };
		float f, f2;

		for (i = 0; i < 7; i++) {
			f = testf16[i];
			pack(buf, "f", f);
			unpack(buf, "f", &f2);

			if (f2 != f) {
				success &= false;
				printf("f16: %f != %f\n", f, f2);
				printf("  buffer: %02hhx %02hhx %02hhx %02hhx "
						" %02hhx %02hhx %02hhx %02hhx\n", buf[0], buf[1],
						buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
			} else {

			}
		}
	}

	if (success) {
		printf("Test finished without error\n");
	} else {
		printf("Test finished with error(s)\n");
	}
}

} /* namespace wanhive */


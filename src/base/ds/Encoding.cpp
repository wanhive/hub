/*
 * Encoding.cpp
 *
 * Binary-to-text encodings
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

/*
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 * Simplified BSD License
 *
 * All the files in this library are covered under the terms of the
 * Berkeley Software Distribution (BSD) License:
 *
 * Copyright (c) 2009-2017, Graham Bull.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Encoding.h"
#include "Twiddler.h"
#include <cctype>
#include <cstdio>
#include <cstring>

namespace {

constexpr unsigned char INVALID = 0xFF;
constexpr unsigned char PADDING = '=';

constexpr unsigned int BASE16_MAX_PADDING = 0;
constexpr unsigned int BASE16_MAX_VALUE = 15;

constexpr unsigned int BASE32_MAX_PADDING = 6;
constexpr unsigned int BASE32_MAX_VALUE = 31;

constexpr unsigned int BASE64_MAX_PADDING = 2;
constexpr unsigned int BASE64_MAX_VALUE = 63;

constexpr unsigned int BASE16_GROUP_LENGTH = 4;
constexpr unsigned int BASE32_GROUP_LENGTH = 5;
constexpr unsigned int BASE64_GROUP_LENGTH = 6;

constexpr unsigned int BASE16_DECODER_IN = 2;
constexpr unsigned int BASE16_ENCODER_IN = 1;
constexpr unsigned int BASE32_DECODER_IN = 8;
constexpr unsigned int BASE32_ENCODER_IN = 5;
constexpr unsigned int BASE64_DECODER_IN = 4;
constexpr unsigned int BASE64_ENCODER_IN = 3;

const char BASE16_ALPHABET[] = "0123456789ABCDEF";
const char BASE32_ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567=";
const char BASE64_ALPHABET[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

const unsigned char BASE16_INVERSE_LIST[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04,
		0x05, 0x06, 0x07, 0x08, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

const unsigned char BASE32_INVERSE_LIST[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1A, 0x1B, 0x1C,
		0x1D, 0x1E, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0xFF, 0xFF, 0xFF,
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
		0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

const unsigned char BASE64_INVERSE_LIST[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F, 0x34, 0x35, 0x36, 0x37, 0x38,
		0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0x40, 0xFF, 0xFF, 0xFF,
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
		0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1A, 0x1B, 0x1C, 0x1D,
		0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
		0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

struct EncodingTraits {
	unsigned int decoder;
	unsigned int encoder;
	unsigned int group;
	unsigned int padding;
	unsigned int value;
};

const EncodingTraits TRAITS[] = { { BASE16_DECODER_IN, BASE16_ENCODER_IN,
		BASE16_GROUP_LENGTH, BASE16_MAX_PADDING, BASE16_MAX_VALUE }, {
		BASE32_DECODER_IN, BASE32_ENCODER_IN, BASE32_GROUP_LENGTH,
		BASE32_MAX_PADDING, BASE32_MAX_VALUE }, { BASE64_DECODER_IN,
		BASE64_ENCODER_IN, BASE64_GROUP_LENGTH, BASE64_MAX_PADDING,
		BASE64_MAX_VALUE } };
const char *ALPHABETS[] = { BASE16_ALPHABET, BASE32_ALPHABET, BASE64_ALPHABET };
const unsigned char *INVERSE_LISTS[] = { BASE16_INVERSE_LIST,
		BASE32_INVERSE_LIST, BASE64_INVERSE_LIST };

/* RFC 4648 test vectors */
const char *testVectors[] = { "", "f", "fo", "foo", "foob", "fooba", "foobar",
		nullptr };

}  // namespace

namespace wanhive {

char Encoding::encode16(unsigned int index) noexcept {
	if (index <= BASE16_MAX_VALUE) {
		return BASE16_ALPHABET[index];
	} else {
		return '\0';
	}
}
char Encoding::encode32(unsigned int index) noexcept {
	if (index <= BASE32_MAX_VALUE) {
		return BASE32_ALPHABET[index];
	} else {
		return '\0';
	}
}
char Encoding::encode64(unsigned int index) noexcept {
	if (index <= BASE64_MAX_VALUE) {
		return BASE64_ALPHABET[index];
	} else {
		return '\0';
	}
}

unsigned char Encoding::decode16(unsigned char c) noexcept {
	return BASE16_INVERSE_LIST[c];
}
unsigned char Encoding::decode32(unsigned char c) noexcept {
	return BASE32_INVERSE_LIST[c];
}
unsigned char Encoding::decode64(unsigned char c) noexcept {
	return BASE64_INVERSE_LIST[c];
}

bool Encoding::validate16(const char *src, unsigned int size) noexcept {
	return validate(ENC_BASE16, src, size);
}
bool Encoding::validate32(const char *src, unsigned int size) noexcept {
	return validate(ENC_BASE32, src, size);
}
bool Encoding::validate64(const char *src, unsigned int size) noexcept {
	return validate(ENC_BASE64, src, size);
}

unsigned int Encoding::encodedLength16(unsigned int size) noexcept {
	return encodedLength(ENC_BASE16, size);
}
unsigned int Encoding::encodedLength32(unsigned int size) noexcept {
	return encodedLength(ENC_BASE32, size);
}
unsigned int Encoding::encodedLength64(unsigned int size) noexcept {
	return encodedLength(ENC_BASE64, size);
}

unsigned int Encoding::decodedLength16(unsigned int size) noexcept {
	return decodedLength(ENC_BASE16, size);
}
unsigned int Encoding::decodedLength32(unsigned int size) noexcept {
	return decodedLength(ENC_BASE32, size);
}
unsigned int Encoding::decodedLength64(unsigned int size) noexcept {
	return decodedLength(ENC_BASE64, size);
}

unsigned int Encoding::base64Encode(char *dest, const void *src,
		unsigned int srcLength, unsigned int destLength) noexcept {
	if (!dest || (encodedLength64(srcLength) > destLength)
			|| (srcLength && !src)) {
		return 0;
	} else if (!srcLength) {
		dest[0] = 0;
		return 0;
	}

	//Consume the input three characters at a time
	auto data = (const unsigned char*) src;
	unsigned int length = 0;
	for (unsigned int x = 0; x < srcLength; x += BASE64_ENCODER_IN) {
		auto in = srcLength - x;
		if (in > BASE64_ENCODER_IN) {
			in = BASE64_ENCODER_IN;
		}

		//Assemble the 24-bit number
		unsigned long long n = 0;
		switch (in) {
		case BASE64_ENCODER_IN:
			n += data[x + 2];
			/* no break */
		case 2:
			n += ((unsigned long) data[x + 1]) << 8;
			/* no break */
		case 1:
			n += ((unsigned long) data[x]) << 16;
			break;
		default:
			dest[0] = 0;
			return 0;
		}

		/*
		 * Split the 24-bit number into four 6-bit numbers
		 * Introduce padding wherever necessary
		 */
		constexpr unsigned char pad = BASE64_MAX_VALUE + 1;
		unsigned char p[4] = { pad, pad, pad, pad };
		switch (in) {
		case BASE64_ENCODER_IN:
			p[3] = (unsigned char) n & BASE64_MAX_VALUE;
			/* no break */
		case 2:
			p[2] = (unsigned char) (n >> BASE64_GROUP_LENGTH)
					& BASE64_MAX_VALUE;
			/* no break */
		case 1:
			p[1] = (unsigned char) (n >> BASE64_GROUP_LENGTH * 2)
					& BASE64_MAX_VALUE;
			p[0] = (unsigned char) (n >> BASE64_GROUP_LENGTH * 3)
					& BASE64_MAX_VALUE;
			break;
		default:
			dest[0] = 0;
			return 0;
		}

		//Assemble the output
		dest[length++] = BASE64_ALPHABET[p[0]];
		dest[length++] = BASE64_ALPHABET[p[1]];
		dest[length++] = BASE64_ALPHABET[p[2]];
		dest[length++] = BASE64_ALPHABET[p[3]];
	}

	dest[length] = 0; //NUL-terminate
	return length;
}

unsigned int Encoding::base64Decode(unsigned char *dest, const char *src,
		unsigned int srcLength, unsigned int destLength) noexcept {
	if (!src || !dest || !srcLength || (srcLength % BASE64_DECODER_IN)
			|| (decodedLength64(srcLength) > destLength)) {
		return 0;
	}

	unsigned int length = 0;
	unsigned long long buf = 1;
	for (unsigned int index = 0; index < srcLength; ++index) {
		auto c = decode64(src[index]);
		if (c <= BASE64_MAX_VALUE) {
			buf = buf << BASE64_GROUP_LENGTH | c;

			//Received 24 bits, split it up into 3-bytes
			if (buf & 0x1000000) { //6*4
				length += 3;
				*dest++ = buf >> 16;
				*dest++ = buf >> 8;
				*dest++ = buf;
				buf = 1;
			}
		} else if (c == BASE64_MAX_VALUE + 1) {
			break;
		} else {
			return 0;
		}
	}

	if (buf & 0x40000) { //6*3
		length += 2;
		*dest++ = buf >> 10;
		*dest++ = buf >> 2;
	} else if (buf & 0x1000) { //6*2
		++length;
		*dest++ = buf >> 4;
	}

	return length;
}

unsigned int Encoding::base32Encode(char *dest, const void *src,
		unsigned int srcLength, unsigned int destLength) noexcept {
	if (!dest || (encodedLength32(srcLength) > destLength)
			|| (srcLength && !src)) {
		return 0;
	} else if (!srcLength) {
		dest[0] = 0;
		return 0;
	}

	//Consume the input five characters at a time
	auto data = (const unsigned char*) src;
	unsigned int length = 0;
	for (unsigned int x = 0; x < srcLength; x += BASE32_ENCODER_IN) {
		auto in = srcLength - x;
		if (in > BASE32_ENCODER_IN) {
			in = BASE32_ENCODER_IN;
		}

		//Assemble the 40-bit number
		unsigned long long n = 0;
		switch (in) {
		case BASE32_ENCODER_IN:
			n += ((unsigned long long) data[x + 4]);
			/* no break */
		case 4:
			n += ((unsigned long long) data[x + 3]) << 8;
			/* no break */
		case 3:
			n += ((unsigned long long) data[x + 2]) << 16;
			/* no break */
		case 2:
			n += ((unsigned long long) data[x + 1]) << 24;
			/* no break */
		case 1:
			n += ((unsigned long long) data[x]) << 32;
			break;
		default:
			dest[0] = 0;
			return 0;
		}

		/*
		 * Split the 40-bit number into eight 5-bit numbers
		 * Introduce padding wherever necessary
		 */
		constexpr unsigned char pad = BASE32_MAX_VALUE + 1;
		unsigned char p[8] = { pad, pad, pad, pad, pad, pad, pad, pad };
		switch (in) {
		case BASE32_ENCODER_IN:
			p[7] = (unsigned char) n & BASE32_MAX_VALUE;
			/* no break */
		case 4:
			p[6] = (unsigned char) (n >> BASE32_GROUP_LENGTH)
					& BASE32_MAX_VALUE;
			p[5] = (unsigned char) (n >> BASE32_GROUP_LENGTH * 2)
					& BASE32_MAX_VALUE;
			/* no break */
		case 3:
			p[4] = (unsigned char) (n >> BASE32_GROUP_LENGTH * 3)
					& BASE32_MAX_VALUE;
			/* no break */
		case 2:
			p[3] = (unsigned char) (n >> BASE32_GROUP_LENGTH * 4)
					& BASE32_MAX_VALUE;
			p[2] = (unsigned char) (n >> BASE32_GROUP_LENGTH * 5)
					& BASE32_MAX_VALUE;
			/* no break */
		case 1:
			p[1] = (unsigned char) (n >> BASE32_GROUP_LENGTH * 6)
					& BASE32_MAX_VALUE;
			p[0] = (unsigned char) (n >> BASE32_GROUP_LENGTH * 7)
					& BASE32_MAX_VALUE;
			break;
		default:
			dest[0] = 0;
			return 0;
		}

		//Assemble the output
		dest[length++] = BASE32_ALPHABET[p[0]];
		dest[length++] = BASE32_ALPHABET[p[1]];
		dest[length++] = BASE32_ALPHABET[p[2]];
		dest[length++] = BASE32_ALPHABET[p[3]];
		dest[length++] = BASE32_ALPHABET[p[4]];
		dest[length++] = BASE32_ALPHABET[p[5]];
		dest[length++] = BASE32_ALPHABET[p[6]];
		dest[length++] = BASE32_ALPHABET[p[7]];
	}

	dest[length] = 0; //NUL-terminate
	return length;
}

unsigned int Encoding::base32Decode(unsigned char *dest, const char *src,
		unsigned int srcLength, unsigned int destLength) noexcept {
	if (!src || !dest || !srcLength || (srcLength % BASE32_DECODER_IN)
			|| (decodedLength32(srcLength) > destLength)) {
		return 0;
	}

	unsigned int length = 0;
	unsigned long long buf = 1;
	for (unsigned int index = 0; index < srcLength; ++index) {
		auto c = decode32(toupper(src[index]));
		if (c <= BASE32_MAX_VALUE) {
			buf = buf << BASE32_GROUP_LENGTH | c;
			//Received 40 bits, split it up into 5-bytes
			if (buf & 0x10000000000) { //5*8
				length += 5;
				*dest++ = buf >> 32;
				*dest++ = buf >> 24;
				*dest++ = buf >> 16;
				*dest++ = buf >> 8;
				*dest++ = buf;
				buf = 1;
			}
		} else if (c == BASE32_MAX_VALUE + 1) {
			break;
		} else {
			return 0;
		}
	}

	if (buf & 0x800000000) { //5*7
		length += 4;
		*dest++ = buf >> 27;
		*dest++ = buf >> 19;
		*dest++ = buf >> 11;
		*dest++ = buf >> 3;
	} else if (buf & 0x2000000) { //5*5
		length += 3;
		*dest++ = buf >> 17;
		*dest++ = buf >> 9;
		*dest++ = buf >> 1;
	} else if (buf & 0x100000) { //5*4
		length += 2;
		*dest++ = buf >> 12;
		*dest++ = buf >> 4;
	} else if (buf & 0x400) { //5*2
		++length;
		*dest++ = buf >> 2;
	}

	return length;
}

unsigned int Encoding::base16Encode(char *dest, const void *src,
		unsigned int srcLength, unsigned int destLength) noexcept {
	if (!dest || (encodedLength16(srcLength) > destLength)
			|| (srcLength && !src)) {
		return 0;
	} else if (!srcLength) {
		dest[0] = 0;
		return 0;
	}

	//Consume the input one characters at a time
	auto data = (const unsigned char*) src;
	unsigned int length = 0;
	for (unsigned int x = 0; x < srcLength; x += BASE16_ENCODER_IN) {
		//Assemble the 8-bit number
		unsigned long long n = data[x];

		//Split the 8-bit number into two 4-bit numbers
		unsigned char p[2];
		p[0] = (unsigned char) (n >> BASE16_GROUP_LENGTH) & BASE16_MAX_VALUE;
		p[1] = (unsigned char) n & BASE16_MAX_VALUE;

		//Assemble the output
		dest[length++] = BASE16_ALPHABET[p[0]];
		dest[length++] = BASE16_ALPHABET[p[1]];
	}

	dest[length] = 0; //NUL-terminate
	return length;

}

unsigned int Encoding::base16Decode(unsigned char *dest, const char *src,
		unsigned int srcLength, unsigned int destLength) noexcept {
	if (!src || !dest || !srcLength || (srcLength % BASE16_DECODER_IN)
			|| (decodedLength16(srcLength) > destLength)) {
		return 0;
	}

	unsigned int length = 0;
	unsigned long long buf = 1;
	for (unsigned int index = 0; index < srcLength; ++index) {
		auto c = decode16(toupper(src[index]));
		if (c <= BASE16_MAX_VALUE) {
			buf = buf << BASE16_GROUP_LENGTH | c;
			//Received 8 bits
			if (buf & 0x100) { //4*2
				++length;
				*dest++ = buf;
				buf = 1;
			}
		} else {
			return 0;
		}
	}

	return length;
}

unsigned int Encoding::encode(EncodingBase base, char *dest, const void *src,
		unsigned int srcLength, unsigned int destLength) noexcept {
	switch (base) {
	case ENC_BASE16:
		return base16Encode(dest, src, srcLength, destLength);
	case ENC_BASE32:
		return base32Encode(dest, src, srcLength, destLength);
	case ENC_BASE64:
		return base64Encode(dest, src, srcLength, destLength);
	default:
		return 0;
	}
}

unsigned int Encoding::decode(EncodingBase base, unsigned char *dest,
		const char *src, unsigned int srcLength,
		unsigned int destLength) noexcept {
	switch (base) {
	case ENC_BASE16:
		return base16Decode(dest, src, srcLength, destLength);
	case ENC_BASE32:
		return base32Decode(dest, src, srcLength, destLength);
	case ENC_BASE64:
		return base64Decode(dest, src, srcLength, destLength);
	default:
		return 0;
	}
}

unsigned int Encoding::encodedLength(EncodingBase base,
		unsigned int size) noexcept {
	auto &traits = TRAITS[base];
	return (Twiddler::ceiling(size, traits.encoder) * traits.decoder) + 1;
}

unsigned int Encoding::decodedLength(EncodingBase base,
		unsigned int size) noexcept {
	auto &traits = TRAITS[base];
	return (Twiddler::ceiling(size, traits.decoder) * traits.encoder);
}

bool Encoding::validate(EncodingBase base, const char *src,
		unsigned int size) noexcept {
	auto &traits = TRAITS[base];
	auto list = INVERSE_LISTS[base];

	if (!src || (size % traits.decoder != 0)) { //Invalid length
		return false;
	}

	unsigned int i = 0;
	for (; i < size; ++i) {
		auto c = list[(unsigned char) src[i]];
		if (c <= traits.value) {
			continue;
		} else if (c != (traits.value + 1)) {
			//Invalid character
			return false;
		} else if (size > (i + traits.padding)) {
			//Too much padding
			return false;
		} else {
			//Break out and check padding
			break;
		}
	}

	for (; i < size; ++i) {
		auto c = list[(unsigned char) src[i]];
		if (c != (traits.value + 1)) {
			//Padding character was expected
			return false;
		}
	}

	return true;
}

void Encoding::printAlphabet(EncodingBase base) noexcept {
	printf("%s\n", ALPHABETS[base]);
}

void Encoding::printConversionTable(EncodingBase base) noexcept {
	auto list = INVERSE_LISTS[base];
	for (unsigned int x = 0; x < 256 / 16; ++x) {
		for (unsigned int y = 0; y < 16; ++y) {
			printf("0x%02X", (unsigned int) (list[x * 16 + y] & 0xff));
			if (y != 15) {
				printf(", ");
			}
		}
		printf("\n");
	}
	printf("\n");
}

void Encoding::test() noexcept {
	unsigned char u[128];
	char s[128];
	printf("\nBASE64 test\n");
	printf("-----------\n");
	for (unsigned int i = 0; testVectors[i]; ++i) {
		memset(u, 0, sizeof(u));
		memset(s, 0, sizeof(s));
		auto vlen = strlen(testVectors[i]);
		auto len = base64Encode(s, testVectors[i], vlen, 128);
		printf("ENC(\"%s\") = \"%s\" [%u]\n", testVectors[i], s, len);
		if (!validate64(s, len) || encodedLength64(vlen) != (len + 1)) {
			printf("Encoding error\n");
		}
		len = base64Decode(u, s, len, 128);
		printf("DEC(\"%s\") = \"%s\" [%u]\n", s, u, len);
		if ((len != vlen) || memcmp(testVectors[i], u, len)) {
			printf("Decoding error\n");
		}
	}

	printf("\nBASE32 test\n");
	printf("-----------\n");
	for (unsigned int i = 0; testVectors[i]; ++i) {
		memset(u, 0, sizeof(u));
		memset(s, 0, sizeof(s));
		auto vlen = strlen(testVectors[i]);
		auto len = base32Encode(s, testVectors[i], vlen, 128);
		printf("ENC(\"%s\") = \"%s\" [%u]\n", testVectors[i], s, len);
		if (!validate32(s, len) || encodedLength32(vlen) != (len + 1)) {
			printf("Encoding error\n");
		}
		len = base32Decode(u, s, len, 128);
		printf("DEC(\"%s\") = \"%s\" [%u]\n", s, u, len);
		if ((len != vlen) || memcmp(testVectors[i], u, len)) {
			printf("Decoding error\n");
		}
	}

	printf("\nBASE16 test\n");
	printf("-----------\n");
	for (unsigned int i = 0; testVectors[i]; ++i) {
		memset(u, 0, sizeof(u));
		memset(s, 0, sizeof(s));
		auto vlen = strlen(testVectors[i]);
		auto len = base16Encode(s, testVectors[i], vlen, 128);
		printf("ENC(\"%s\") = \"%s\" [%u]\n", testVectors[i], s, len);
		if (!validate16(s, len) || encodedLength16(vlen) != (len + 1)) {
			printf("Encoding error\n");
		}
		len = base16Decode(u, s, len, 128);
		printf("DEC(\"%s\") = \"%s\" [%u]\n", s, u, len);
		if ((len != vlen) || memcmp(testVectors[i], u, len)) {
			printf("Decoding error\n");
		}
	}
}

} /* namespace wanhive */

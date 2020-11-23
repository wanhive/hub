/*
 * Encoding.h
 *
 * Binary-to-text encodings
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_ENCODING_H_
#define WH_BASE_DS_ENCODING_H_

namespace wanhive {
//-----------------------------------------------------------------
//Enumeration of supported encodings
enum EncodingBase {
	ENC_BASE16, ENC_BASE32, ENC_BASE64
};
//-----------------------------------------------------------------
/**
 * Base16, Base32, and Base64 binary-to-text encoder and decoder
 * REF: RFC 4648 (https://tools.ietf.org/html/rfc4648)
 * REF: http://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
 * REF: CyoEncode by Graham Bull (https://github.com/calzakk/CyoEncode)
 * NOTE: Doesn't support encoding or decoding of distorted data.
 */
class Encoding {
public:
	/**
	 * Following three functions return the encoding character at position
	 * specified by the <index>. On error, NUL character is returned.
	 * BaseXX: index must be in range [0, BASEXX_MAX_VALUE]
	 */
	static char encode16(unsigned int index) noexcept;
	static char encode32(unsigned int index) noexcept;
	static char encode64(unsigned int index) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Following three functions decode a character to the corresponding raw byte
	 */
	static unsigned char decode16(unsigned char c) noexcept;
	static unsigned char decode32(unsigned char c) noexcept;
	static unsigned char decode64(unsigned char c) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Following three functions validate a base-X-encoded string.
	 */
	static bool validate16(const char *src, unsigned int size) noexcept;
	static bool validate32(const char *src, unsigned int size) noexcept;
	static bool validate64(const char *src, unsigned int size) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Following six functions return the minimum buffer size required for
	 * storing the output for the given input size.
	 * encodedLengthXX functions account for the NUL terminator
	 */
	static unsigned int encodedLength16(unsigned int size) noexcept;
	static unsigned int encodedLength32(unsigned int size) noexcept;
	static unsigned int encodedLength64(unsigned int size) noexcept;

	static unsigned int decodedLength16(unsigned int size) noexcept;
	static unsigned int decodedLength32(unsigned int size) noexcept;
	static unsigned int decodedLength64(unsigned int size) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Following functions carry out baseXX encoding/decoding.
	 * This is the responsibility of the caller to allocate buffer of sufficient
	 * size for storing the results. Use encodeLengthXX/decodeLengthXX functions
	 * to calculate the minimum buffer size. Encoded strings are NUL-terminated.
	 */
	static unsigned int base64Encode(char *dest, const void *src,
			unsigned int srcLength, unsigned int destLength) noexcept;
	static unsigned int base64Decode(unsigned char *dest, const char *src,
			unsigned int srcLength, unsigned int destLength) noexcept;

	static unsigned int base32Encode(char *dest, const void *src,
			unsigned int srcLength, unsigned int destLength) noexcept;
	static unsigned int base32Decode(unsigned char *dest, const char *src,
			unsigned int srcLength, unsigned int destLength) noexcept;

	static unsigned int base16Encode(char *dest, const void *src,
			unsigned int srcLength, unsigned int destLength) noexcept;
	static unsigned int base16Decode(unsigned char *dest, const char *src,
			unsigned int srcLength, unsigned int destLength) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Following functions carry out encoding/decoding and take an explicit <base>
	 * parameter. Caller is responsible for allocating buffer of sufficient size to
	 * store the results. Use encodeLength and decodeLength functions to calculate
	 * the minimum buffer size. Encoded strings are NUL-terminated.
	 */
	static unsigned int encode(EncodingBase base, char *dest, const void *src,
			unsigned int srcLength, unsigned int destLength) noexcept;
	static unsigned int decode(EncodingBase base, unsigned char *dest,
			const char *src, unsigned int srcLength,
			unsigned int destLength) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Following two functions return the minimum buffer size required for
	 * storing the output for the given input size.
	 * Returned value of encodedLength accounts for the NUL terminator
	 */
	static unsigned int encodedLength(EncodingBase base,
			unsigned int size) noexcept;
	static unsigned int decodedLength(EncodingBase base,
			unsigned int size) noexcept;
	//-----------------------------------------------------------------
	//Validates an encoded string in the given <base>
	static bool validate(EncodingBase base, const char *src,
			unsigned int size) noexcept;
	//-----------------------------------------------------------------
	/**
	 * For testing and validation purposes
	 */
	static void printAlphabet(EncodingBase base) noexcept;
	static void printConversionTable(EncodingBase base) noexcept;
	//RFC 4648 vector test
	static void test() noexcept;
private:
	static constexpr unsigned int BASE16_MAX_PADDING = 0;
	static constexpr unsigned int BASE16_MAX_VALUE = 15;

	static constexpr unsigned int BASE32_MAX_PADDING = 6;
	static constexpr unsigned int BASE32_MAX_VALUE = 31;

	static constexpr unsigned int BASE64_MAX_PADDING = 2;
	static constexpr unsigned int BASE64_MAX_VALUE = 63;

	static constexpr unsigned int BASE16_GROUP_LENGTH = 4;
	static constexpr unsigned int BASE32_GROUP_LENGTH = 5;
	static constexpr unsigned int BASE64_GROUP_LENGTH = 6;

	static constexpr unsigned int BASE16_DECODER_IN = 2;
	static constexpr unsigned int BASE16_ENCODER_IN = 1;
	static constexpr unsigned int BASE32_DECODER_IN = 8;
	static constexpr unsigned int BASE32_ENCODER_IN = 5;
	static constexpr unsigned int BASE64_DECODER_IN = 4;
	static constexpr unsigned int BASE64_ENCODER_IN = 3;

	static const char BASE16_ALPHABET[];
	static const char BASE32_ALPHABET[];
	static const char BASE64_ALPHABET[];

	static const unsigned char BASE16_INVERSE_LIST[];
	static const unsigned char BASE32_INVERSE_LIST[];
	static const unsigned char BASE64_INVERSE_LIST[];

	struct Traits {
		unsigned int decoder;
		unsigned int encoder;
		unsigned int group;
		unsigned int padding;
		unsigned int value;
	};

	static const Traits TRAITS[];
	static const char *ALPHABETS[];
	static const unsigned char *INVERSE_LISTS[];

	/* RFC 4648 test vectors */
	static const char *testVectors[];
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_ENCODING_H_ */

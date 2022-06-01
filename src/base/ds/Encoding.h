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
/**
 * Enumeration of supported encodings
 */
enum EncodingBase {
	ENC_BASE16,/**< Base-16 encoding */
	ENC_BASE32,/**< Base-32 encoding */
	ENC_BASE64 /**< Base-64 encoding */
};
//-----------------------------------------------------------------
/**
 * Base16, Base32, and Base64 encoder and decoder.
 * @note Doesn't support encoding or decoding of distorted data.
 * @ref RFC 4648 (https://tools.ietf.org/html/rfc4648)
 * @ref http://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
 * @ref CyoEncode by Graham Bull (https://github.com/calzakk/CyoEncode)
 */
class Encoding {
public:
	/**
	 * Returns an encoding character from the base-16 alphabet.
	 * @param index an index into the base-16 alphabet, it's value should be in
	 * the range [0, Encoding::BASE16_MAX_VALUE].
	 * @return encoding character at the given index on success, nul character
	 * on error (invalid index).
	 */
	static char encode16(unsigned int index) noexcept;
	/**
	 * Returns an encoding character from the base-32 alphabet.
	 * @param index an index into the base-16 alphabet, it's value should be in
	 * the range [0, Encoding::BASE32_MAX_VALUE].
	 * @return encoding character at the given index on success, nul character
	 * on error (invalid index).
	 */
	static char encode32(unsigned int index) noexcept;
	/**
	 * Returns an encoding character from the base-64 alphabet.
	 * @param index an index into the base-16 alphabet, it's value should be in
	 * the range [0, Encoding::BASE64_MAX_VALUE].
	 * @return encoding character at the given index on success, nul character
	 * on error (invalid index).
	 */
	static char encode64(unsigned int index) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Decodes a base-16 encoding character.
	 * @param c a character from the base-16 alphabet
	 * @return the decoded value on success, 0xff on error (invalid character)
	 */
	static unsigned char decode16(unsigned char c) noexcept;
	/**
	 * Decodes a base-32 encoding character.
	 * @param c a character from the base-32 alphabet
	 * @return the decoded value on success, 0xff on error (invalid character)
	 */
	static unsigned char decode32(unsigned char c) noexcept;
	/**
	 * Decodes a base-64 encoding character.
	 * @param c a character from the base-64 alphabet
	 * @return the decoded value on success, 0xff on error (invalid character)
	 */
	static unsigned char decode64(unsigned char c) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Validates a base-16 encoded string.
	 * @param src the input string
	 * @param size input string's length in bytes
	 * @return true if the string is a valid base-16 encoding, false otherwise
	 */
	static bool validate16(const char *src, unsigned int size) noexcept;
	/**
	 * Validates a base-32 encoded string.
	 * @param src the input string
	 * @param size input string's length in bytes
	 * @return true if the string is a valid base-32 encoding, false otherwise
	 */
	static bool validate32(const char *src, unsigned int size) noexcept;
	/**
	 * Validates a base-64 encoded string.
	 * @param src the input string
	 * @param size input string's length in bytes
	 * @return true if the string is a valid base-64 encoding, false otherwise
	 */
	static bool validate64(const char *src, unsigned int size) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the base-16 encoded string's size for the given input size.
	 * @param size input size in bytes
	 * @return encoded string's size including the nul-terminator in bytes
	 */
	static unsigned int encodedLength16(unsigned int size) noexcept;
	/**
	 * Returns the base-32 encoded string's size for the given input size.
	 * @param size input size in bytes
	 * @return encoded string's size including the nul-terminator in bytes
	 */
	static unsigned int encodedLength32(unsigned int size) noexcept;
	/**
	 * Returns the base-64 encoded string's size for the given input size.
	 * @param size input size in bytes
	 * @return encoded string's size including the nul-terminator in bytes
	 */
	static unsigned int encodedLength64(unsigned int size) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the binary data's size corresponding to a base-16 encoded
	 * string of given length.
	 * @param size encoded string's length
	 * @return decoded data's size in bytes
	 */
	static unsigned int decodedLength16(unsigned int size) noexcept;
	/**
	 * Returns the binary data's size corresponding to a base-32 encoded
	 * string of given length.
	 * @param size encoded string's length
	 * @return decoded data's size in bytes
	 */
	static unsigned int decodedLength32(unsigned int size) noexcept;
	/**
	 * Returns the binary data's size corresponding to a base-64 encoded
	 * string of given length.
	 * @param size encoded string's length
	 * @return decoded data's size in bytes
	 */
	static unsigned int decodedLength64(unsigned int size) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Produces base-64 encoding of the given binary data. The encoded string is
	 * nul-terminated.
	 * @param dest the output buffer
	 * @param src the binary data
	 * @param srcLength binary data's size in bytes
	 * @param destLength output buffer's size in bytes
	 * @return encoded string's length
	 */
	static unsigned int base64Encode(char *dest, const void *src,
			unsigned int srcLength, unsigned int destLength) noexcept;
	/**
	 * Decodes a base-64 encoded string.
	 * @param dest the output buffer
	 * @param src the encoded string
	 * @param srcLength encoded string's length
	 * @param destLength output buffer's size in bytes
	 * @return decoded data's size in bytes
	 */
	static unsigned int base64Decode(unsigned char *dest, const char *src,
			unsigned int srcLength, unsigned int destLength) noexcept;
	/**
	 * Produces base-32 encoding of the given binary data. The encoded string is
	 * nul-terminated.
	 * @param dest the output buffer
	 * @param src the binary data
	 * @param srcLength binary data's size in bytes
	 * @param destLength output buffer's size in bytes
	 * @return encoded string's length
	 */
	static unsigned int base32Encode(char *dest, const void *src,
			unsigned int srcLength, unsigned int destLength) noexcept;
	/**
	 * Decodes a base-32 encoded string.
	 * @param dest the output buffer
	 * @param src the encoded string
	 * @param srcLength encoded string's length
	 * @param destLength output buffer's size in bytes
	 * @return decoded data's size in bytes
	 */
	static unsigned int base32Decode(unsigned char *dest, const char *src,
			unsigned int srcLength, unsigned int destLength) noexcept;
	/**
	 * Produces base-16 encoding of the given binary data. The encoded string is
	 * nul-terminated.
	 * @param dest the output buffer
	 * @param src the binary data
	 * @param srcLength binary data's size in bytes
	 * @param destLength output buffer's size in bytes
	 * @return encoded string's length
	 */
	static unsigned int base16Encode(char *dest, const void *src,
			unsigned int srcLength, unsigned int destLength) noexcept;
	/**
	 * Decodes a base-16 encoded string.
	 * @param dest the output buffer
	 * @param src the encoded string
	 * @param srcLength encoded string's length
	 * @param destLength output buffer's size in bytes
	 * @return decoded data's size in bytes
	 */
	static unsigned int base16Decode(unsigned char *dest, const char *src,
			unsigned int srcLength, unsigned int destLength) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Encodes the given data. The encoded string is nul-terminated.
	 * @param base the base selector
	 * @param dest the output buffer
	 * @param src the binary data
	 * @param srcLength binary data's size in bytes
	 * @param destLength output buffer's size in bytes
	 * @return encoded string's length
	 */
	static unsigned int encode(EncodingBase base, char *dest, const void *src,
			unsigned int srcLength, unsigned int destLength) noexcept;
	/**
	 * Decodes an encoded string.
	 * @param base the base selector
	 * @param dest the output buffer
	 * @param src the encoded string
	 * @param srcLength encoded string's length
	 * @param destLength output buffer's size in bytes
	 * @return decoded data's size in bytes
	 */
	static unsigned int decode(EncodingBase base, unsigned char *dest,
			const char *src, unsigned int srcLength,
			unsigned int destLength) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the encoded string's size for the given input size.
	 * @param base the base selector
	 * @param size encoded string's length
	 * @return encoded string's size including the nul-terminator in bytes
	 */
	static unsigned int encodedLength(EncodingBase base,
			unsigned int size) noexcept;
	/**
	 * Returns the binary data's size corresponding to a encoded string of
	 * given length.
	 * @param base the base selector
	 * @param size encoded string's length
	 * @return decoded data's size in bytes
	 */
	static unsigned int decodedLength(EncodingBase base,
			unsigned int size) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks if the given string is a valid encoding.
	 * @param base the base selector
	 * @param src the encoded string
	 * @param size encoded string's length
	 * @return true if the input string is a valid encoding, false otherwise
	 */
	static bool validate(EncodingBase base, const char *src,
			unsigned int size) noexcept;
	//-----------------------------------------------------------------
	/**
	 * For debugging: prints the alphabet table.
	 * @param base the base selector
	 */
	static void printAlphabet(EncodingBase base) noexcept;
	/**
	 * For debugging: prints the conversion table.
	 * @param base the base selector
	 */
	static void printConversionTable(EncodingBase base) noexcept;
	/**
	 * For debugging: self test.
	 */
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

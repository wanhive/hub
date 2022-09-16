/*
 * CryptoUtils.h
 *
 * Cipher initialization and error handling routines
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SECURITY_CRYPTOUTILS_H_
#define WH_BASE_SECURITY_CRYPTOUTILS_H_

namespace wanhive {
/**
 * Cipher initializer and error handler
 */
class CryptoUtils {
public:
	/**
	 * Consumes the most recent error and returns it.
	 * @return error code
	 */
	static unsigned long getError() noexcept;
	/**
	 * Consumes and reports the most recent error. This call is equivalent to
	 * CryptoUtils::getErrorMessage(Cryptoutils::getError(), buffer, length).
	 * @param buffer stores error's description
	 * @param length buffer's size in bytes
	 */
	static void getErrorMessage(char *buffer, unsigned int length) noexcept;
	/**
	 * Describes the given error code.
	 * @param error error code
	 * @param buffer stores error's description
	 * @param length buffer's size in bytes
	 */
	static void getErrorMessage(unsigned long error, char *buffer,
			unsigned int length) noexcept;
	/**
	 * Clears the error queue.
	 */
	static void clearErrors() noexcept;
private:
	class initializer;  //Forward declaration
	static const initializer _init;
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_CRYPTOUTILS_H_ */

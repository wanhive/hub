/*
 * CryptoUtils.h
 *
 * OpenSSL initializer and error handler
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
 * OpenSSL initializer and error handler
 */
class CryptoUtils {
public:
	/**
	 * Consumes the most recent error and returns it.
	 * @return error code
	 */
	static unsigned long getError() noexcept;
	/**
	 * Describes the most recent error.
	 * @param buffer stores error's description
	 * @param length buffer's size in bytes
	 */
	static void getErrorMessage(char *buffer, unsigned int length) noexcept;
	/**
	 * Describes an error code.
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
	class initializer;
	static const initializer _init;
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_CRYPTOUTILS_H_ */

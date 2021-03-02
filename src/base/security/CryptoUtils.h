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
	//Consume the most recent error and return it
	static unsigned long getError() noexcept;
	//Clear and report the most recent error
	static void getErrorMessage(char *buffer, unsigned int length) noexcept;
	//Write the <error> message into the <buffer> of size <length>
	static void getErrorMessage(unsigned long error, char *buffer,
			unsigned int length) noexcept;
	//Clear the error queue
	static void clearErrors() noexcept;
private:
	class initializer;  //Forward declaration
	static const initializer _init;
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_CRYPTOUTILS_H_ */

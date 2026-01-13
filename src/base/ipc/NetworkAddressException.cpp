/*
 * NetworkAddressException.cpp
 *
 * Exceptions/errors generated during network address translation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "NetworkAddressException.h"
#include <netdb.h>

namespace wanhive {

NetworkAddressException::NetworkAddressException(int type) noexcept :
		error { type } {

}

NetworkAddressException::~NetworkAddressException() {

}

const char* NetworkAddressException::what() const noexcept {
	return ::gai_strerror(error);
}

int NetworkAddressException::errorCode() const noexcept {
	return this->error;
}

} /* namespace wanhive */

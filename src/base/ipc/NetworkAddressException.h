/*
 * NetworkAddressException.h
 *
 * Exceptions/errors generated during network address translation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_IPC_NETWORKADDRESSEXCEPTION_H_
#define WH_BASE_IPC_NETWORKADDRESSEXCEPTION_H_
#include "../common/BaseException.h"

namespace wanhive {
/**
 * Exceptions generated during network address translation
 */
class NetworkAddressException: public BaseException {
public:
	NetworkAddressException(int type) noexcept;
	~NetworkAddressException();
	const char* what() const noexcept override;
	int errorCode() const noexcept override;
private:
	int error;
};

} /* namespace wanhive */

#endif /* WH_BASE_IPC_NETWORKADDRESSEXCEPTION_H_ */

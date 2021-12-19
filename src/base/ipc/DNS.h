/*
 * DNS.h
 *
 * Domain name resolution and reverse lookup
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_IPC_DNS_H_
#define WH_BASE_IPC_DNS_H_
#include "inet.h"

namespace wanhive {
/**
 * Domain name resolution and reverse lookup
 */
class DNS {
public:
	DNS() noexcept;
	DNS(const char *node, const char *service, const SocketTraits *hints);
	~DNS();

	//Wrapper for getaddrinfo(3): performs network address translation
	void lookup(const char *node, const char *service,
			const SocketTraits *traits);
	//Returns the next address (or nullptr)
	const addrinfo* next() noexcept;
	//Rewinds the address pointer
	void rewind() noexcept;
	//Clears out the address list
	void clear() noexcept;

	//Helper method: reads the socket traits from the addrinfo structure
	static void getTraits(const addrinfo *info, SocketTraits &traits) noexcept;
	//Helper method: reads the socket address from the addrinfo structure
	static void getAddress(const addrinfo *info, SocketAddress &sa) noexcept;

	//Wrapper for getnameinfo(3): performs address-to-name translation
	static void reverse(const SocketAddress &sa, NameInfo &ni,
			int flags = (NI_NUMERICHOST | NI_NUMERICSERV));
private:
	addrinfo *result;
	addrinfo *index;
};

} /* namespace wanhive */

#endif /* WH_BASE_IPC_DNS_H_ */

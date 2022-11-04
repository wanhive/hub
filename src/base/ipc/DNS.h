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
	/**
	 * Default constructor: initializes an empty object.
	 */
	DNS() noexcept;
	/**
	 * Constructor: performs network address translation.
	 * @param node host name
	 * @param service service name
	 * @param traits connection properties
	 */
	DNS(const char *node, const char *service, const SocketTraits *traits);
	/**
	 * Destructor
	 */
	~DNS();
	//-----------------------------------------------------------------
	/**
	 * Wrapper for getaddrinfo(3): performs network address translation.
	 * @param node host name
	 * @param service service name
	 * @param traits connection properties
	 */
	void lookup(const char *node, const char *service,
			const SocketTraits *traits);
	/**
	 * Reads an address from the network address list.
	 * @return the next entry
	 */
	const addrinfo* next() noexcept;
	/**
	 * Rewinds the network address list.
	 */
	void rewind() noexcept;
	/**
	 * Wrapper for freeaddrinfo(3): frees the network address list.
	 */
	void clear() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Helper method: reads connection properties from an addrinfo structure.
	 * @param info connection data
	 * @param traits stores the connection properties
	 */
	static void getTraits(const addrinfo *info, SocketTraits &traits) noexcept;
	/**
	 * Helper method: reads socket address from an addrinfo structure.
	 * @param info connection data
	 * @param sa stores the socket address
	 */
	static void getAddress(const addrinfo *info, SocketAddress &sa) noexcept;
	/**
	 * Wrapper for getnameinfo(3): performs address-to-name translation.
	 * @param sa network address for translation
	 * @param ni stores the resource name
	 * @param flags operation flags
	 */
	static void reverse(const SocketAddress &sa, NameInfo &ni,
			int flags = (NI_NUMERICHOST | NI_NUMERICSERV));
private:
	addrinfo *result { nullptr };
	addrinfo *index { nullptr };
};

} /* namespace wanhive */

#endif /* WH_BASE_IPC_DNS_H_ */

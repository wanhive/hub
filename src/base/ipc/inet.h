/*
 * inet.h
 *
 * Useful structures for the network-based IPC
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_IPC_INET_H_
#define WH_BASE_IPC_INET_H_
#include <netdb.h>

namespace wanhive {
/*
 * Resource name
 */
struct NameInfo {
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];
	int type;
};

/*
 * Socket address
 */
struct SocketAddress {
	sockaddr_storage address;
	socklen_t length;
};

/*
 * Socket connection properties
 */
struct SocketTraits {
	int domain; //Protocol family
	int type; //Socket type
	int protocol; //Socket's protocol
	int flags; //Additional flags
};

}  // namespace wanhive

#endif /* WH_BASE_IPC_INET_H_ */

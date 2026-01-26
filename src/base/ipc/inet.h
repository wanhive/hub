/**
 * @file inet.h
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

/*! @namespace wanhive */
namespace wanhive {
/**
 * Resource name
 */
struct NameInfo {
	/*! Host name */
	char host[NI_MAXHOST];
	/*! Service name */
	char service[NI_MAXSERV];
	/*! Host type */
	int type;
};

/**
 * Socket address
 */
struct SocketAddress {
	/*! Generic address information container */
	sockaddr_storage address;
	/*! Address information size in bytes */
	socklen_t length;
};

/**
 * Socket connection properties
 */
struct SocketTraits {
	/*! Protocol family */
	int domain;
	/*! Socket type */
	int type;
	/*! Socket's protocol */
	int protocol;
	/*! Additional flags */
	int flags;
};

}  // namespace wanhive

#endif /* WH_BASE_IPC_INET_H_ */

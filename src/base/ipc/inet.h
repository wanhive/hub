/*
 * inet.h
 *
 *  Created on: 19-Dec-2021
 *      Author: amit
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

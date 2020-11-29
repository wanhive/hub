/*
 * Network.h
 *
 * Linux network programming routines
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_NETWORK_H_
#define WH_BASE_NETWORK_H_
#include<netdb.h>

namespace wanhive {
//-----------------------------------------------------------------
struct NameInfo {
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];
};

struct SocketAddress {
	sockaddr_storage address;
	socklen_t length;
};
//-----------------------------------------------------------------
/**
 * Basic network programming routines
 * Adapted from Beej's Guide to Network Programming
 * REF: https://beej.us/guide/bgnet/html/
 */
class Network {
public:
	//Returns a bound socket
	static int serverSocket(const char *service, SocketAddress &sa,
			bool blocking, int type = SOCK_STREAM, int family = AF_UNSPEC,
			int protocol = 0);
	//Set AI_NUMERICHOST in <flags> to stop DNS resolution
	static int connectedSocket(const char *name, const char *service,
			SocketAddress &sa, bool blocking, int type = SOCK_STREAM,
			int family = AF_UNSPEC, int flags = 0, int protocol = 0);
	//Same as above, but uses nameInfo
	static int connectedSocket(const NameInfo &ni, SocketAddress &sa,
			bool blocking, int type = SOCK_STREAM, int family = AF_UNSPEC,
			int flags = 0, int protocol = 0);
	//The basic socket, not connected
	static int socket(const char *name, const char *service, SocketAddress &sa,
			int type, int family, int flags, int protocol = 0);
	//Same as above, but uses nameinfo
	static int socket(const NameInfo &ni, SocketAddress &sa, int type,
			int family, int flags, int protocol = 0);
	//Assign the address to the socket
	static void bind(int sfd, SocketAddress &sa);
	//Listen for incoming connections
	static void listen(int sfd, int backlog);
	/*
	 * Accept an incoming connection
	 * Useful flag=SOCK_NONBLOCK (linux specific)
	 */
	static int accept(int listenfd, SocketAddress &sa, int flags = 0);
	/*
	 * Connect to a server, in case connect fails, retry with a fresh socket
	 * MAN 2  CONNECT: The socket is nonblocking and the  connection  cannot  be  completed  immediately.
	 * It  is  possible  to select(2)  or  poll(2)  for  completion  by  selecting the socket for writing.
	 * After select(2) indicates writability, use getsockopt(2) to
	 * read the SO_ERROR option at level SOL_SOCKET to determine whether  connect()
	 * completed  successfully  (SO_ERROR is zero) or unsuccessfully
	 * (SO_ERROR is one of the usual error codes listed here, explaining the reason for the failure).
	 *
	 */
	static int connect(int sfd, SocketAddress &sa);
	//Wrapper for the shutdown(2) system call
	static int shutdown(int sfd, int how = SHUT_RDWR) noexcept;
	//closes an open socket, best effort
	static int close(int sfd) noexcept;
	//set a socket's blocking/non-blocking IO state
	static void setBlocking(int sfd, bool block);
	//Test whether the socket is blocking
	static bool isBlocking(int sfd);
	//Creates a unix domain socket and binds it to the given address
	static int unixServerSocket(const char *path, SocketAddress &sa,
			bool blocking, int type = SOCK_STREAM, int protocol = 0);
	//Connects to a Unix domain socket, returns the socket descriptor (if success or in progress)
	static int unixConnectedSocket(const char *path, SocketAddress &sa,
			bool blocking, int type = SOCK_STREAM, int protocol = 0);
	// Creates unnamed pair of connected sockets (unix domain)
	static void socketPair(int sv[2], bool blocking, int type = SOCK_STREAM);
	//No costly DNS resolution by default
	static void getNameInfo(const SocketAddress &sa, NameInfo &ni,
			int flags = (NI_NUMERICHOST | NI_NUMERICSERV));
	//returns the current address to which the socket sfd is bound
	static void getSockName(int sfd, SocketAddress &sa);
	//returns the address of the peer connected to the socket sfd
	static void getPeerName(int sfd, SocketAddress &sa);
	//=================================================================
	/**
	 * Blocking IO utilities
	 */
	static size_t sendStream(int sockfd, const unsigned char *buf,
			size_t length);
	//If <strict> then throws exception if the connection closes or times out mid-way
	static size_t receiveStream(int sockfd, unsigned char *buf, size_t length,
			bool strict = true);
	/*
	 * Set send and receive time-outs for blocking sockets
	 * If the timeout is 0 then the socket blocks forever
	 */
	static void setReceiveTimeout(int sfd, int milliseconds);
	static void setSendTimeout(int sfd, int milliseconds);
	/*
	 * Combines the above two functions, both timeout values
	 * are measured in milliseconds. If the timeout is negative,
	 * then it is ignored.
	 */
	static void setSocketTimeout(int sfd, int recvTimeout, int sendTimeout);
private:
	static addrinfo* getAddrInfo(const char *name, const char *service,
			int family, int type, int flags, int protocol);
	static void freeAddrInfo(addrinfo *res) noexcept;
};

} /* namespace wanhive */

#endif /* WH_BASE_NETWORK_H_ */

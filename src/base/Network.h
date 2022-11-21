/*
 * Network.h
 *
 * Stream socket routines
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_NETWORK_H_
#define WH_BASE_NETWORK_H_
#include "ipc/inet.h"

namespace wanhive {
/**
 * Stream (TCP/IP) socket routines
 */
class Network {
public:
	/**
	 * Creates a listening socket.
	 * @param service describes the internet service (usually a port number)
	 * @param sa stores socket's address
	 * @param blocking true for blocking mode, false for nonblocking IO
	 * @return listening socket's file descriptor
	 */
	static int serverSocket(const char *service, SocketAddress &sa,
			bool blocking);
	/**
	 * Creates a connected socket.
	 * @param name describes the internet host (usually the IP address)
	 * @param service describes the internet service (usually the port number)
	 * @param sa stores host's socket address
	 * @param blocking true for blocking mode, false for nonblocking IO
	 * @return connected socket's file descriptor
	 */
	static int connectedSocket(const char *name, const char *service,
			SocketAddress &sa, bool blocking);
	/**
	 * Creates a connected socket.
	 * @param ni the internet resource name
	 * @param sa stores host's socket address
	 * @param blocking true for blocking mode, false for nonblocking IO
	 * @return connected socket's file descriptor
	 */
	static int connectedSocket(const NameInfo &ni, SocketAddress &sa,
			bool blocking);
	//-----------------------------------------------------------------
	/**
	 * Listens for incoming connections.
	 * @param sfd listening socket's file descriptor
	 * @param backlog pending queue size
	 */
	static void listen(int sfd, int backlog);
	/**
	 * Accepts incoming connections on a given listening socket.
	 * @param listenfd listening socket's descriptor
	 * @param sa stores peer's socket address
	 * @param blocking true for blocking mode, false for nonblocking IO
	 * @return accepted connection's socket file descriptor
	 */
	static int accept(int listenfd, SocketAddress &sa, bool blocking);
	/**
	 * Wrapper for shutdown(2) system call.
	 * @param sfd socket descriptor
	 * @param how shutdown mode
	 * @return value returned by the system call
	 */
	static int shutdown(int sfd, int how = SHUT_RDWR) noexcept;
	/**
	 * Wrapper for close(2) system call.
	 * @param sfd socket descriptor
	 * @return value returned by the system call
	 */
	static int close(int sfd) noexcept;
	/**
	 * Configures a socket descriptor's blocking mode.
	 * @param sfd socket descriptor
	 * @param block true for blocking mode, false for nonblocking IO
	 */
	static void setBlocking(int sfd, bool block);
	/**
	 * Tests whether a socket descriptor is in blocking mode.
	 * @param sfd socket descriptor
	 * @return true if blocking IO is enabled, false otherwise
	 */
	static bool isBlocking(int sfd);
	//-----------------------------------------------------------------
	/**
	 * Creates a unix domain socket and binds it to the given path.
	 * @param path pathname of the socket file
	 * @param sa object for storing the address information
	 * @param blocking true for blocking mode, false for nonblocking IO
	 * @return listening socket descriptor
	 */
	static int unixServerSocket(const char *path, SocketAddress &sa,
			bool blocking);
	/**
	 * Establishes a unix domain socket connection (connection may complete in
	 * background).
	 * @param path socket file's pathname
	 * @param sa stores the peer's socket address
	 * @param blocking true for blocking mode, false for nonblocking IO
	 * @return connected socket descriptor
	 */
	static int unixConnectedSocket(const char *path, SocketAddress &sa,
			bool blocking);
	/**
	 * Creates an unnamed pair of connected unix domain sockets.
	 * @param sv stores the socket descriptors
	 * @param blocking true for blocking mode, false for nonblocking IO
	 */
	static void socketPair(int (&sv)[2], bool blocking);
	//-----------------------------------------------------------------
	/**
	 * Writes data to a socket connection opened in blocking mode.
	 * @param sockfd socket descriptor
	 * @param buf outgoing data
	 * @param length data size in bytes
	 * @return number of bytes written to the socket connection
	 */
	static size_t sendStream(int sockfd, const unsigned char *buf,
			size_t length);
	/**
	 * Reads data from a socket connection opened in blocking mode.
	 * @param sockfd socket descriptor
	 * @param buf stores incoming data
	 * @param length maximum number of bytes to read
	 * @param strict true to read exactly the given number of bytes, false to
	 * read at most the given number of bytes (potentially zero).
	 * @return number of bytes read from the socket connection
	 */
	static size_t receiveStream(int sockfd, unsigned char *buf, size_t length,
			bool strict = true);
	//-----------------------------------------------------------------
	/**
	 * Sets read operation's timeout.
	 * @param sfd socket descriptor
	 * @param milliseconds timeout value in milliseconds, set 0 to block forever
	 */
	static void setReceiveTimeout(int sfd, unsigned int milliseconds);
	/**
	 * Sets write operation's timeout.
	 * @param sfd socket descriptor
	 * @param milliseconds timeout value in milliseconds, set 0 to block forever
	 */
	static void setSendTimeout(int sfd, unsigned int milliseconds);
	/**
	 * Sets read and write timeout values. Negative timeout value is ignored.
	 * @param sfd socket descriptor
	 * @param recvTimeout read timeout in milliseconds, 0 to block forever
	 * @param sendTimeout write timeout in milliseconds, 0 to block forever
	 */
	static void setSocketTimeout(int sfd, int recvTimeout, int sendTimeout);
};

} /* namespace wanhive */

#endif /* WH_BASE_NETWORK_H_ */

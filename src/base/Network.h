/**
 * @file Network.h
 *
 * Stream (TCP/IP) socket routines
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

/*! @namespace wanhive */
namespace wanhive {
/**
 * Stream (TCP/IP) socket routines
 */
class Network {
public:
	/**
	 * Creates a listening socket.
	 * @param service service type (usually a port number)
	 * @param sa stores socket address
	 * @param blocking true for blocking mode, false otherwise
	 * @return listening socket file descriptor
	 */
	static int server(const char *service, SocketAddress &sa, bool blocking);
	/**
	 * Creates a connected socket.
	 * @param name host's name (usually the IP address)
	 * @param service host's service type (usually the port number)
	 * @param sa stores host's socket address
	 * @param blocking true for blocking mode, false otherwise
	 * @return connected socket file descriptor
	 */
	static int connect(const char *name, const char *service, SocketAddress &sa,
			bool blocking);
	/**
	 * Creates a connected socket.
	 * @param ni resource name
	 * @param sa stores host's socket address
	 * @param blocking true for blocking mode, false otherwise
	 * @return connected socket file descriptor
	 */
	static int connect(const NameInfo &ni, SocketAddress &sa, bool blocking);
	//-----------------------------------------------------------------
	/**
	 * Listens for incoming connections on a socket.
	 * @param listener listening socket
	 * @param backlog requests backlog
	 */
	static void listen(int listener, int backlog);
	/**
	 * Accepts connection on a listening socket.
	 * @param listener listening socket
	 * @param sa stores accepted connection's address
	 * @param blocking true to make the new socket non-blocking, false otherwise
	 * @return newly accepted socket file descriptor
	 */
	static int accept(int listener, SocketAddress &sa, bool blocking);
	/**
	 * Wrapper for shutdown(2) system call.
	 * @param sfd socket file descriptor
	 * @param how shutdown mode
	 * @return value returned by the system call
	 */
	static int shutdown(int sfd, int how = SHUT_RDWR) noexcept;
	/**
	 * Wrapper for close(2) system call.
	 * @param sfd socket file descriptor
	 * @return value returned by the system call
	 */
	static int close(int sfd) noexcept;
	/**
	 * Configures a socket's blocking IO mode.
	 * @param sfd socket file descriptor
	 * @param blocking true for blocking mode, false otherwise
	 */
	static void setBlocking(int sfd, bool blocking);
	/**
	 * Determines if a socket is set to blocking or non-blocking.
	 * @param sfd socket file descriptor
	 * @return true if blocking, false otherwise
	 */
	static bool isBlocking(int sfd);
	//-----------------------------------------------------------------
	/**
	 * Creates a unix domain socket.
	 * @param path socket file's path
	 * @param sa stores the address information
	 * @param blocking true to set blocking mode on the socket, false otherwise
	 * @return socket file descriptor
	 */
	static int unixServer(const char *path, SocketAddress &sa, bool blocking);
	/**
	 * Establishes a unix domain socket connection.
	 * @param path socket file's path
	 * @param sa stores host's address
	 * @param blocking true to set blocking mode on the socket, false otherwise
	 * @return socket file descriptor
	 */
	static int unixConnect(const char *path, SocketAddress &sa, bool blocking);
	/**
	 * Creates an unnamed unix domain socket pair.
	 * @param sv stores the socket file descriptors
	 * @param blocking true to set blocking mode on the sockets, false otherwise
	 */
	static void socketPair(int (&sv)[2], bool blocking);
	//-----------------------------------------------------------------
	/**
	 * Reads data from a blocking socket connection.
	 * @param sfd socket file descriptor
	 * @param buf stores incoming data
	 * @param bytes number of bytes to read
	 * @param strict true to read the specified number of bytes; false to read
	 * at most the specified number of bytes (potentially zero).
	 * @return number of bytes transferred
	 */
	static size_t read(int sfd, unsigned char *buf, size_t bytes, bool strict =
			true);
	/**
	 * Writes data to a blocking socket connection.
	 * @param sfd socket file descriptor
	 * @param buf outgoing data
	 * @param bytes data size in bytes
	 * @return number of bytes transferred
	 */
	static size_t write(int sfd, const unsigned char *buf, size_t bytes);
	//-----------------------------------------------------------------
	/**
	 * Sets a timeout for blocking receive operations.
	 * @param sfd socket file descriptor
	 * @param timeout timeout value in milliseconds (0 to wait forever)
	 */
	static void setReceiveTimeout(int sfd, unsigned int timeout);
	/**
	 * Sets a timeout for blocking send operations.
	 * @param sfd socket file descriptor
	 * @param timeout timeout value in milliseconds (0 to wait forever)
	 */
	static void setSendTimeout(int sfd, unsigned int timeout);
	/**
	 * Sets timeout values for blocking input (send) and output (receive)
	 * operations. Set a negative value to skip, zero (0) to wait forever.
	 * @param sfd socket file descriptor
	 * @param input receive timeout value in milliseconds
	 * @param output send timeout value in milliseconds
	 */
	static void setTimeout(int sfd, int input, int output);
};

} /* namespace wanhive */

#endif /* WH_BASE_NETWORK_H_ */

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
	 * Creates a socket descriptor which can listen for and accept incoming
	 * connection requests.
	 * @param service describes the internet service (usually the port number)
	 * @param sa object for storing the address information
	 * @param blocking true to configure the listening socket for blocking IO
	 * (see Network::accept()), false for nonblocking IO.
	 * @return listening socket descriptor
	 */
	static int serverSocket(const char *service, SocketAddress &sa,
			bool blocking);
	//Returns a connected socket
	/**
	 * Creates a connected socket descriptor.
	 * @param name describes the internet host (usually the IP address)
	 * @param service describes the internet service (usually the port number)
	 * @param sa object for storing the host's address information
	 * @param blocking true to configure the connection for blocking IO, false
	 * for nonblocking IO.
	 * @return connected socket descriptor
	 */
	static int connectedSocket(const char *name, const char *service,
			SocketAddress &sa, bool blocking);
	/**
	 * Creates a connected socket descriptor.
	 * @param ni the internet resource name
	 * @param sa object for storing the host's address information
	 * @param blocking true to configure the connection for blocking IO, false
	 * for nonblocking IO.
	 * @return connected socket descriptor
	 */
	static int connectedSocket(const NameInfo &ni, SocketAddress &sa,
			bool blocking);
	//-----------------------------------------------------------------
	/**
	 * Listens for incoming connections on the given socket descriptor.
	 * @param sfd socket descriptor
	 * @param backlog pending queue size
	 */
	static void listen(int sfd, int backlog);
	/**
	 * Accepts incoming connections on the given socket descriptor
	 * @param listenfd listening socket descriptor
	 * @param sa object for storing the address of peer socket
	 * @param blocking blocking true to configure the new connection for
	 * blocking IO, false for nonblocking IO.
	 * @return socket descriptor corresponding to the accepted connection
	 */
	static int accept(int listenfd, SocketAddress &sa, bool blocking);
	/**
	 * Wrapper for the shutdown(2) system call.
	 * @param sfd the scket descriptor
	 * @param how shutdown mode
	 * @return return value of the system call
	 */
	static int shutdown(int sfd, int how = SHUT_RDWR) noexcept;
	/**
	 * Wrapper for close(2) system call.
	 * @param sfd the socket descriptor
	 * @return return value of the system call
	 */
	static int close(int sfd) noexcept;
	/**
	 * Configures the given socket descriptor's blocking/non-blocking IO mode.
	 * @param sfd socket descriptor
	 * @param block true for blocking IO, false for non-blocking IO
	 */
	static void setBlocking(int sfd, bool block);
	/**
	 * Tests if an IO operation on the given socket descriptor will block.
	 * @param sfd socket descriptor
	 * @return true if blocking IO is enabled, false otherwise
	 */
	static bool isBlocking(int sfd);
	//-----------------------------------------------------------------
	/**
	 * Creates a unix domain socket and binds it to the given path.
	 * @param path pathname of the socket file
	 * @param sa object for storing the address information
	 * @param blocking true to configure the listening socket for blocking IO
	 * (see Network::accept()), false for nonblocking IO.
	 * @return listening socket descriptor
	 */
	static int unixServerSocket(const char *path, SocketAddress &sa,
			bool blocking);
	/**
	 * Establishes new connection to a listening unix domain socket (connection
	 * may complete in background).
	 * @param path pathname of the socket file
	 * @param sa object for storing the address of peer socket
	 * @param blocking true to configure the connection for blocking IO, false
	 * for nonblocking IO.
	 * @return connected socket descriptor
	 */
	static int unixConnectedSocket(const char *path, SocketAddress &sa,
			bool blocking);
	/**
	 * Creates an unnamed pair of connected sockets (unix domain).
	 * @param sv object for for storing the pair of socket descriptors
	 * @param blocking true to configure the socket pair for blocking IO, false
	 * for non-blocking IO.
	 */
	static void socketPair(int sv[2], bool blocking);
	//-----------------------------------------------------------------
	/**
	 * Writes data to a blocking socket connection.
	 * @param sockfd socket descriptor (must be configured for blocking IO)
	 * @param buf data to write
	 * @param length data size in bytes
	 * @return number of bytes written to the socket connection
	 */
	static size_t sendStream(int sockfd, const unsigned char *buf,
			size_t length);
	/**
	 * Reads data from a blocking socket connection.
	 * @param sockfd socket descriptor (must be configured for blocking IO)
	 * @param buf buffer for storing the incoming data
	 * @param length maximum number of bytes to read
	 * @param strict true to read exactly the given number of bytes, false to
	 * read at most the given number of bytes (potentially zero).
	 * @return number of bytes read from the socket connection
	 */
	static size_t receiveStream(int sockfd, unsigned char *buf, size_t length,
			bool strict = true);
	//-----------------------------------------------------------------
	/**
	 * Sets the socket read operation's timeout.
	 * @param sfd socket descriptor
	 * @param milliseconds timeout value in milliseconds, set 0 to block forever
	 */
	static void setReceiveTimeout(int sfd, int milliseconds);
	/**
	 * Sets the socket write operation's timeout.
	 * @param sfd socket descriptor
	 * @param milliseconds timeout value in milliseconds, set 0 to block forever
	 */
	static void setSendTimeout(int sfd, int milliseconds);
	/**
	 * Sets socket read and write operations timeout. Negative timeout value will
	 * be ignored.
	 * @param sfd socket descriptor
	 * @param recvTimeout read timeout value (milliseconds), 0 to block forever
	 * @param sendTimeout write timeout value (milliseconds), 0 to block forever
	 */
	static void setSocketTimeout(int sfd, int recvTimeout, int sendTimeout);
};

} /* namespace wanhive */

#endif /* WH_BASE_NETWORK_H_ */

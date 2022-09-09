/*
 * Socket.h
 *
 * Message stream watcher
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_SOCKET_H_
#define WH_HUB_SOCKET_H_
#include "Topic.h"
#include "../base/Network.h"
#include "../base/common/Source.h"
#include "../base/ds/Pooled.h"
#include "../base/ds/StaticBuffer.h"
#include "../base/ds/StaticCircularBuffer.h"
#include "../base/security/SSLContext.h"
#include "../reactor/Watcher.h"
#include "../util/Message.h"

namespace wanhive {
//-----------------------------------------------------------------
/**
 * Enumeration of socket flags, should not conflict with the watcher flags
 */
enum SocketFlag : uint32_t {
	SOCKET_PRIORITY = 1024, /**< Priority connection */
	SOCKET_OVERLAY = 2048, /**< Overlay connection */
	SOCKET_LOCAL = 4096 /**< Unix domain socket connection */
};

/**
 * Enumeration of socket types
 */
enum SocketType {
	SOCKET_LISTENER = 1,/**< Listening socket */
	SOCKET_PROXY = 2 /**< Outgoing connection socket  */
};
//-----------------------------------------------------------------
/**
 * Message stream watcher
 * Not thread safe
 */
class Socket final: public Pooled<Socket>,
		public Source<unsigned char>,
		public Watcher {
public:
	/**
	 * Constructor: associates the given file descriptor to the object
	 * @param fd the file descriptor to use with this object
	 */
	Socket(int fd) noexcept;
	/**
	 * Constructor: associates a secure connection to this object
	 * @param ssl the SSL/TLS connection object
	 */
	Socket(SSL *ssl);
	/**
	 * Constructor: connects to a host.
	 * @param ni the remote host's address, set the service filed to "unix" for
	 * establishing a unix domain socket connection.
	 * @param blocking true for blocking IO, false for non-blocking IO (default)
	 * @param timeoutMils the IO timeout for blocking connection, set 0 to block
	 * forever, -1 to ignore (use the default behavior).
	 */
	Socket(const NameInfo &ni, bool blocking = false, int timeoutMils = -1);
	/**
	 * Constructor: creates a host which can accept incoming connections.
	 * @param service the service name (usually the listening port number for
	 * TCP/IP socket, and the pathname for a unix domain socket).
	 * @param backlog the listening backlog
	 * @param isUnix true for creating a unix domain socket, false for creating
	 * a TCP/IP socket (default).
	 * @param blocking true for blocking IO, false for non-blocking IO (default)
	 */
	Socket(const char *service, int backlog, bool isUnix = false,
			bool blocking = false);
	/**
	 * Destructor
	 */
	~Socket();
	//-----------------------------------------------------------------
	void* operator new(size_t size);
	void operator delete(void *p) noexcept;
	//-----------------------------------------------------------------
	/*
	 * Source interface implementation
	 */
	size_t take(unsigned char *buffer, size_t count) override;
	size_t available() const noexcept override;
	//-----------------------------------------------------------------
	/*
	 * Watcher interface implementation
	 */
	void start() override;
	void stop() noexcept override;
	bool callback(void *arg) noexcept override;
	bool publish(void *arg) noexcept override;

	void setTopic(unsigned int index) noexcept override;
	void clearTopic(unsigned int index) noexcept override;
	bool testTopic(unsigned int index) const noexcept override;
	unsigned long long getOption(int name) const noexcept override;
	void setOption(int name, unsigned long long value) noexcept override;
	//-----------------------------------------------------------------
	/**
	 * Accepts an incoming connection request.
	 * @param blocking true for configuring the incoming connection for blocking
	 * IO, false for non-blocking IO.
	 * @return a new incoming connection, nullptr if the call would block
	 */
	Socket* accept(bool blocking = false);
	/**
	 * Reads incoming messages (see Socket::getMessage()).
	 * @return the number of bytes read on success (possible zero(0) if the
	 * internal buffer is full), zero(0) if the non-blocking mode is on and the
	 * read operation would block, -1 if the connection was closed cleanly.
	 */
	ssize_t read();
	/**
	 * Writes outgoing messages to the underlying connection.
	 * @return the number of bytes written, possibly zero (0) if no outgoing
	 * message queued up.
	 */
	ssize_t write();
	/**
	 * Returns the incoming messages read from the connection (see Socket::read()).
	 * @return the next incoming message
	 */
	Message* getMessage();
	/**
	 * Returns the underlying secure connection object
	 * @return the SSL/TLS connection object, nullptr if the underlying connection
	 * is not secure.
	 */
	SSL* getSecureSocket() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Creates a socket pair.
	 * @param sfd object for storing the other end of the socket pair
	 * @param blocking true for configuring the socket pair for blocking IO,
	 * false for non-blocking IO.
	 * @return object containing one end of the socket pair
	 */
	static Socket* createSocketPair(int &sfd, bool blocking = false);
	/**
	 * Sets the context for secure connections
	 * @param ctx object containing the SSL/TLS context
	 */
	static void setSSLContext(SSLContext *ctx) noexcept;
	/**
	 * Checks whether the given value is a temporary socket connection identifier.
	 * Identifiers outside the range [Socket::MIN_ACTIVE_ID, Socket::MAX_ACTIVE_ID]
	 * are used as the temporary socket identifier.
	 * @param id the value to check
	 * @return true if the given value is a temporary id, false otherwise.
	 */
	static bool isEphemeralId(unsigned long long id) noexcept;
private:
	//Read from a raw socket connection
	ssize_t socketRead();
	//Write to a raw socket connection
	ssize_t socketWrite();
	//Read from a secure connection
	ssize_t secureRead();
	//Write to a secure connection
	ssize_t secureWrite();
	//=================================================================
	//Set up a SSL/TLS secure connection to a good state
	void initSSL();
	/*
	 * Read operations return the number of bytes read on success (possibly 0),
	 * and 0 if the file descriptor is non-blocking and the operation would block.
	 */
	ssize_t sslRead(void *buf, size_t count);
	/*
	 * Write operations return the number of bytes written on success (possibly 0),
	 * and 0 if the file descriptor is non-blocking and the operation would block.
	 */
	ssize_t sslWrite(const void *buf, size_t count);
	//=================================================================
	//Create IOVECs from outgoing messages and return the count
	unsigned int fillOutgoingQueue() noexcept;
	//Adjust the IOVECs for the next write cycle
	void adjustOutgoingQueue(size_t bytes) noexcept;

	//Clear internal state
	void clear() noexcept;
	//Free internal resources
	void cleanup() noexcept;
public:
	/** The minimum value for active socket identifier */
	static constexpr uint64_t MIN_ACTIVE_ID = 0;
	/** The maximum value for active socket identifier */
	static constexpr uint64_t MAX_ACTIVE_ID = INT64_MAX;
	/** The incoming message buffer size in bytes (must be power of two) */
	static constexpr unsigned int READ_BUFFER_SIZE = (Message::MTU << 3);
	/** Size of the output queue (must be power of two) */
	static constexpr unsigned int OUT_QUEUE_SIZE = 1024;
private:
	//-----------------------------------------------------------------
	//Subscriptions
	Topic subscriptions;
	//-----------------------------------------------------------------
	struct {
		SSL *ssl; //SSL/TLS connection
		bool callRead; //Call read instead of write
		bool callWrite; //Call write instead of read
		bool verified; //Host certificate has been verified
	} secure;
	//-----------------------------------------------------------------
	//Total number of messages received by this object
	unsigned long long totalIncomingMessages;
	//Total number of messages sent by this object
	unsigned long long totalOutgoingMessages;
	//Maximum number of outgoing messages this object is allowed to hold
	unsigned int outQueueLimit;
	//Serialized I/P
	Message *incomingMessage;
	//This buffer stores the incoming raw bytes
	StaticCircularBuffer<unsigned char, READ_BUFFER_SIZE> in;

	//This buffer collects all the outgoing messages
	StaticCircularBuffer<Message*, OUT_QUEUE_SIZE> out;
	//Container for scatter-gather O/P
	StaticBuffer<iovec, OUT_QUEUE_SIZE> outgoingMessages;
	//-----------------------------------------------------------------
	static SSLContext *sslCtx; //SSL/TLS context
};

} /* namespace wanhive */

#endif /* WH_HUB_SOCKET_H_ */

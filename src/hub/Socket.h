/*
 * Socket.h
 *
 * Watcher for sending and receiving messages
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
#include "../base/security/SSLContext.h"
#include "../base/Timer.h"
#include "../base/ds/MemoryPool.h"
#include "../base/ds/StaticBuffer.h"
#include "../base/ds/StaticCircularBuffer.h"
#include "../reactor/Watcher.h"
#include "../util/Message.h"

namespace wanhive {
//-----------------------------------------------------------------
//Should not conflict with the Watcher flags
enum SocketFlag : uint32_t {
	SOCKET_PRIORITY = 128,	//Priority connection
	SOCKET_OVERLAY = 256,	//Overlay connection
};

enum SocketType {
	SOCKET_LISTENER = 1, //A listening socket
	SOCKET_LOCAL = 2, //A local unix domain socket
	SOCKET_PROXY = 3	//An outgoing connection socket
};
//-----------------------------------------------------------------
/**
 * Data stream watcher
 * Not thread safe
 * NOTE: Can watch any non blocking IO stream compatible with epoll including
 * pipe, stdin/stdout etc as long as the the data stream is compliant with the
 * Wanhive protocol.
 */
class Socket: public Watcher {
public:
	Socket(int fd) noexcept;
	Socket(int fd, const SocketAddress &sa) noexcept;
	/*
	 * Creates a connected Socket, type is set to SOCKET_PROXY
	 * If ni.service is "unix" then a unix domain socket is created
	 */
	Socket(const NameInfo &ni, bool blocking = false, int timeoutMils = -1);
	/*
	 * Creates a server Socket, type is set to SOCKET_LISTENER
	 * If <isUnix> is set then a unix domain socket is created
	 */
	Socket(const char *service, int backlog, bool isUnix = false,
			bool blocking = false);
	/*
	 * Creates a SSL/TLS socket
	 */
	Socket(SSL *ssl);

	virtual ~Socket();

	void* operator new(size_t size);
	void operator delete(void *p) noexcept;
	//=================================================================
	/**
	 * Watcher implementation
	 */
	//Does nothing
	void start() override final;
	//Calls shutdown on the underlying socket (cannot be undone)
	void stop() noexcept override final;
	bool callback(void *arg) noexcept override final;
	bool publish(void *arg) noexcept override final;
	void setTopic(unsigned int index) noexcept override final;
	void clearTopic(unsigned int index) noexcept override final;
	bool testTopic(unsigned int index) const noexcept override final;
	//=================================================================
	/*
	 * Wrapper for Network::accept. Returns the newly created connection.
	 * If <blocking> is true then the new connection will block on IO.
	 * Returns NULL if accept would block.
	 */
	Socket* accept(bool blocking = false);
	/*
	 * Shuts down reception and/or transmission.
	 * Wrapper for Network::shutdown.
	 */
	int shutdown(int how = SHUT_RDWR) noexcept;
	/*
	 * Returns the number of bytes read, possibly zero (buffer full or would
	 * block), or -1 if the connection has been closed cleanly. Clears out the
	 * read IO event from this connection if the call would block.
	 */
	ssize_t read();
	/*
	 * Returns the number of bytes written, possibly zero (no message queued up).
	 * Clears the write IO event from this connection if the call would block.
	 */
	ssize_t write();
	/*
	 * Fetches the next message from this connection. The returned message
	 * is configured correctly for reading from or writing to it's IO buffer.
	 */
	Message* getMessage();
	SocketAddress const& getAddress() const noexcept;
	//Whether the connection has outlived the specified timeOut (in miliseconds)
	bool hasTimedOut(unsigned int timeOut) const noexcept;
	/*
	 * Limit on the maximum number of outgoing messages this socket is allowed
	 * to hold in it's internal queue. Setting value to 0 disables it (no limit).
	 */
	void setOutputQueueLimit(unsigned int limit) noexcept;
	unsigned int getOutputQueueLimit() const noexcept;
	//Returns true if the <id> not in the range of the active IDs
	static bool isEphemeralId(unsigned long long id) noexcept;
	//Returns the underlying SSL/TLS connection (potentially nullptr)
	SSL* getSecureSocket() const noexcept;
	//=================================================================
	/*
	 * Create a unix domain socket pair, encapsulates one end of it into
	 * a new Socket and stores the other end into <sfd> on success.
	 * Type of the returned Socket is set to SOCKET_LOCAL
	 */
	static Socket* createSocketPair(int &sfd, bool blocking = false);
	//Set context for SSL connections
	static void setSSLContext(SSLContext *ctx) noexcept;
	//=================================================================
	static void initPool(unsigned int size);
	static void destroyPool();
	static unsigned int poolSize() noexcept;
	static unsigned int allocated() noexcept;
	static unsigned int unallocated() noexcept;
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
	void adjustOutgoingQueue(size_t count) noexcept;

	//Clear internal state
	void clear() noexcept;
	//Free internal resources
	void cleanup() noexcept;
public:
	//0-9223372036854775807 are used as Active Socket IDs
	static constexpr uint64_t MIN_ACTIVE_ID = 0;
	static constexpr uint64_t MAX_ACTIVE_ID = INT64_MAX;
	//Set to at least twice of the MTU, must be power of two
	static constexpr unsigned int READ_BUFFER_SIZE = (Message::MTU << 3);
	//Size of scatter-gather OP buffers, must be power of two
	static constexpr unsigned int OUT_QUEUE_SIZE = 256;
private:
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

	//Serialized O/P
	//This buffer collects all the outgoing messages
	StaticCircularBuffer<Message*, OUT_QUEUE_SIZE> out;
	//Container for scatter-gather O/P
	StaticBuffer<iovec, OUT_QUEUE_SIZE> outgoingMessages;
	//Subscriptions
	Topic subscriptions;
	//-----------------------------------------------------------------
	//Address associated with the current connection
	SocketAddress address;
	//When was this connection created
	Timer timer;
	//-----------------------------------------------------------------
	static MemoryPool pool;
	static SSLContext *sslCtx; //SSL/TLS context
};

} /* namespace wanhive */

#endif /* WH_HUB_SOCKET_H_ */

/**
 * @file Endpoint.h
 *
 * Message exchange
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_ENDPOINT_H_
#define WH_UTIL_ENDPOINT_H_
#include "Packet.h"
#include "../base/Network.h"
#include "../base/common/NonCopyable.h"
#include "../base/security/SSLContext.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Implementation of the request-response message exchange pattern
 * @note This implementation requires blocking I/O
 */
class Endpoint: protected Packet, private NonCopyable {
public:
	/**
	 * Constructor: creates a disconnected endpoint.
	 */
	Endpoint() noexcept;
	/**
	 * Destructor
	 */
	~Endpoint();
	//-----------------------------------------------------------------
	/**
	 * Sets secure connection's context.
	 * @param ctx new SSL/TLS context
	 */
	void setSSLContext(SSLContext *ctx) noexcept;
	/**
	 * Returns secure connection's context.
	 * @return current SSL/TLS context
	 */
	SSLContext* getSSLContext() const noexcept;
	/**
	 * Sets message signing and verification keys (nullptr to disable).
	 * @param pki new asymmetric keys
	 */
	void setKeyPair(Trust *pki = nullptr) noexcept;
	/**
	 * Returns message signing and verification keys.
	 * @return current asymmetric keys
	 */
	Trust* getKeyPair() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Connects to a new host (terminates existing connection).
	 * @param ni host's resource name
	 * @param timeout IO timeout value in milliseconds. Set 0 to block forever,
	 * negative value to ignore.
	 */
	void connect(const NameInfo &ni, int timeout = -1);
	/**
	 * Terminates existing connection.
	 */
	void disconnect();
	/**
	 * Returns the managed socket file descriptor.
	 * @return socket file descriptor
	 */
	int getSocket() const noexcept;
	/**
	 * Returns the managed secure connection object.
	 * @return SSL object (can be nullptr)
	 */
	SSL* getSSL() const noexcept;
	/**
	 * Replaces the managed socket file descriptor (closes the previous one).
	 * @param sfd new socket file descriptor
	 */
	void setSocket(int sfd);
	/**
	 * Replaces the managed secure connection object (closes the previous one).
	 * @param ssl new SSL object
	 */
	void setSSL(SSL *ssl);
	/**
	 * Returns the managed socket file descriptor and releases its ownership.
	 * @return socket file descriptor
	 */
	int releaseSocket() noexcept;
	/**
	 * Returns the managed secure connection object and releases its ownership.
	 * @return SSL object (can be nullptr)
	 */
	SSL* releaseSSL() noexcept;
	/**
	 * Swaps the managed socket file descriptor.
	 * @param sfd new file descriptor
	 * @return previous file descriptor
	 */
	int swapSocket(int sfd);
	/**
	 * Swaps the managed secure connection object.
	 * @param ssl new SSL object
	 * @return previous SSL object
	 */
	SSL* swapSSL(SSL *ssl);
	/**
	 * Sets existing connection's receive and send timeout values.
	 * @param input receive timeout in milliseconds. Set 0 to block forever,
	 * negative value to ignore.
	 * @param output send timeout in milliseconds.  Set 0 block forever,
	 * negative value to ignore.
	 */
	void setTimeout(int input, int output) const;
	//-----------------------------------------------------------------
	/**
	 * Sends a request, with the routing header's length field indicating the
	 * request size in bytes.
	 * @param sign true for message signing, false otherwise
	 */
	void send(bool sign = false);
	/**
	 * Receives a response.
	 * @param seq expected sequence number (0 to ignore)
	 * @param verify true to verify the response, false otherwise
	 */
	void receive(unsigned int seq = 0, bool verify = false);
	/**
	 * Sends a request and receives the response.
	 * @param sign true to sign the request, false otherwise
	 * @param verify true to verify the response, false otherwise
	 * @return true on success, false otherwise (request rejected)
	 */
	bool exchange(bool sign = false, bool verify = false);
	/**
	 * Waits for a ping and then responds back with a pong.
	 */
	void pong();
	//-----------------------------------------------------------------
	/**
	 * Connects to a host and returns the socket file descriptor.
	 * @param ni host's resource name
	 * @param sa stores the socket address on success
	 * @param timeout IO timeout in milliseconds for the new connection. Set
	 * to 0 to block indefinitely, or a negative value to ignore.
	 * @return socket file descriptor
	 */
	static int connect(const NameInfo &ni, SocketAddress &sa, int timeout = -1);
	/**
	 * Sends a request over a socket, signing it if a signing key is provided.
	 * @param sfd socket file descriptor
	 * @param packet outgoing request
	 * @param pki signing key
	 */
	static void send(int sfd, Packet &packet, Trust *pki = nullptr);
	/**
	 * Sends a request over a secure connection, signing it if a signing key
	 * is provided.
	 * @param ssl secure connection object
	 * @param packet outgoing request
	 * @param pki signing key
	 */
	static void send(SSL *ssl, Packet &packet, Trust *pki = nullptr);
	/**
	 * Receives a response from a socket, verifying the signature if a
	 * verification key is provided. Incoming messages not matching the expected
	 * sequence number are dropped silently.
	 * @param sfd socket file descriptor
	 * @param packet stores the incoming response
	 * @param seq expected sequence number (0 to ignore)
	 * @param pki verification key
	 */
	static void receive(int sfd, Packet &packet, unsigned int seq = 0,
			Trust *pki = nullptr);
	/**
	 * Receives a response from a secure connection, verifying the signature
	 * if a verification key is provided. Incoming messages not matching the
	 * expected sequence number are dropped silently.
	 * @param ssl secure connection object
	 * @param packet stores the incoming response
	 * @param seq expected sequence number (0 to ignore)
	 * @param pki verification key
	 */
	static void receive(SSL *ssl, Packet &packet, unsigned int seq = 0,
			Trust *pki = nullptr);
private:
	int sockfd { -1 };
	SSL *ssl { };
	SSLContext *sslContext { };
	Trust *pki { };
};

} /* namespace wanhive */

#endif /* WH_UTIL_ENDPOINT_H_ */

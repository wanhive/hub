/*
 * Endpoint.h
 *
 * Request-response pattern implementation
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
#include "../base/security/SSLContext.h"

namespace wanhive {
/**
 * Request-response pattern implementation.
 * @note Supports blocking IO only, the underlying socket file descriptor must
 * be in blocking mode.
 */
class Endpoint: protected Packet {
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
	 * Sets SSL/TLS context for secure connections.
	 * @param ctx new SSL/TLS context
	 */
	void setSSLContext(SSLContext *ctx) noexcept;
	/**
	 * Returns the SSL/TLS context.
	 * @return current SSL/TLS context
	 */
	SSLContext* getSSLContext() const noexcept;
	/**
	 * Sets message signing and verification keys (nullptr to disable).
	 * @param pki new asymmetric keys
	 */
	void useKeyPair(const PKI *pki = nullptr) noexcept;
	/**
	 * Returns the signing and verification keys.
	 * @return current asymmetric keys
	 */
	const PKI* getKeyPair() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Connects to a new host (terminates existing connection).
	 * @param ni host's resource name
	 * @param timeoutMils send/receive timeout value in milliseconds. Set 0 to
	 * block forever, negative value to ignore.
	 */
	void connect(const NameInfo &ni, int timeoutMils = -1);
	/**
	 * Terminates existing connection.
	 */
	void disconnect();
	/**
	 * Returns existing connection's socket file descriptor.
	 * @return socket file descriptor
	 */
	int getSocket() const noexcept;
	/**
	 * Returns secure connection object.
	 * @return pointer to the SSL object (can be nullptr)
	 */
	SSL* getSecureSocket() const noexcept;
	/**
	 * Replaces the managed socket file descriptor (closes the previous one).
	 * @param sfd new socket file descriptor
	 */
	void setSocket(int sfd);
	/**
	 * Replaces the managed secure connection object (closes the previous one).
	 * @param ssl new SSL object
	 */
	void setSecureSocket(SSL *ssl);
	/**
	 * Returns the managed socket file descriptor and releases its ownership.
	 * @return socket file descriptor
	 */
	int releaseSocket() noexcept;
	/**
	 * Releases the managed secure connection object and releases its ownership.
	 * @return pointer to SSL object (can be nullptr)
	 */
	SSL* releaseSecureSocket() noexcept;
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
	SSL* swapSecureSocket(SSL *ssl);
	/**
	 * Sets existing connection's receive and send timeout values.
	 * @param recvTimeout receive timeout in milliseconds. Set 0 to block
	 * forever, negative value to ignore.
	 * @param sendTimeout send timeout in milliseconds.  Set 0 block forever,
	 * negative value to ignore.
	 */
	void setSocketTimeout(int recvTimeout, int sendTimeout) const;
	//-----------------------------------------------------------------
	/**
	 * Sends out a request, routing header's length field determines the
	 * request's size in bytes.
	 * @param sign true for message signing, false otherwise
	 */
	void send(bool sign = false);
	/**
	 * Receives a response.
	 * @param sequenceNumber expected sequence number (0 to ignore)
	 * @param verify true for message verification, false otherwise
	 */
	void receive(unsigned int sequenceNumber = 0, bool verify = false);
	/**
	 * Executes a request: sends a request and receives the response.
	 * @param sign true to sign the outgoing request, false otherwise
	 * @param verify true to verify the incoming response, false otherwise
	 * @return true on success, false otherwise (request rejected)
	 */
	bool executeRequest(bool sign = false, bool verify = false);
	/**
	 * Waits for a ping and then responds back with a pong.
	 */
	void sendPong();
	//-----------------------------------------------------------------
	/**
	 * Connects to a host and returns the socket file descriptor.
	 * @param ni host's resource name
	 * @param sa object for storing the socket address on success
	 * @param timeoutMils send and receive timeout in milliseconds for the new
	 * connection. Set 0 to block forever, negative value to ignore.
	 * @return socket file descriptor
	 */
	static int connect(const NameInfo &ni, SocketAddress &sa, int timeoutMils =
			-1);
	/**
	 * Sends a request. If a signing key is provided (not nullptr) then the
	 * outgoing request is digitally signed.
	 * @param sfd socket file descriptor
	 * @param packet outgoing request
	 * @param pki signing key
	 */
	static void send(int sfd, Packet &packet, const PKI *pki = nullptr);
	/**
	 * Sends a request. If a signing key is provided (not nullptr) then the
	 * outgoing request is digitally signed.
	 * @param ssl secure connection object
	 * @param packet outgoing request
	 * @param pki signing key
	 */
	static void send(SSL *ssl, Packet &packet, const PKI *pki = nullptr);
	/**
	 * Receives a response. If a verification key is provided (not nullptr) then
	 * the response's digital signature is verified. If an "expected" sequence
	 * number is provided (not zero) then all incoming messages that fail to
	 * match the expected sequence number get silently dropped.
	 * @param sfd socket file descriptor
	 * @param packet stores the incoming response
	 * @param sequenceNumber expected sequence number
	 * @param pki verification key
	 */
	static void receive(int sfd, Packet &packet,
			unsigned int sequenceNumber = 0, const PKI *pki = nullptr);
	/**
	 * Receives a response. If a verification key is provided (not nullptr) then
	 * the response's digital signature is verified. If an "expected" sequence
	 * number is provided (not zero) then all incoming messages that fail to
	 * match the expected sequence number get silently dropped.
	 * @param ssl secure connection object
	 * @param packet stores the incoming response
	 * @param sequenceNumber expected sequence number
	 * @param pki verification key
	 */
	static void receive(SSL *ssl, Packet &packet, unsigned int sequenceNumber =
			0, const PKI *pki = nullptr);
private:
	int sockfd { -1 }; //Socket file descriptor
	SSL *ssl { nullptr };  //SSL/TLS connection
	SSLContext *sslContext { nullptr }; //SSL/TLS context
	const PKI *pki { nullptr }; //Keys for asymmetric cryptography
};

} /* namespace wanhive */

#endif /* WH_UTIL_ENDPOINT_H_ */

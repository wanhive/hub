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
 * be configured for blocking read and write operations.
 */
class Endpoint: protected Packet {
public:
	/**
	 * Constructor: initializes an unconnected endpoint
	 */
	Endpoint() noexcept;
	/**
	 * Destructor
	 */
	~Endpoint();
	//-----------------------------------------------------------------
	/**
	 * Sets the context for SSL/TLS connections for this object.
	 * @param ctx pointer to the SSL/TLS context
	 */
	void setSSLContext(SSLContext *ctx) noexcept;
	/**
	 * Returns the SSL/TLS context assigned to this object.
	 * @return pointer to the SSL/TLS context
	 */
	SSLContext* getSSLContext() const noexcept;
	/**
	 * Sets the keys for message signing and verification for this object.
	 * @param pki pointer to the object containing the keys
	 */
	void useKeyPair(const PKI *pki = nullptr) noexcept;
	/**
	 * Returns the signing and verification keys assigned to this object.
	 * @return pointer to object containing the signing and verification keys
	 */
	const PKI* getKeyPair() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Connects to a new host after terminating the existing connection.
	 * @param ni the resource name of the host
	 * @param timeoutMils specifies the IO timeout value in milliseconds. Set
	 * this value to 0 to block forever, -1 to ignore.
	 */
	void connect(const NameInfo &ni, int timeoutMils = -1);
	/**
	 * Terminates any existing connection.
	 */
	void disconnect();
	/**
	 * Returns the socket file descriptor associated with the currently
	 * established connection.
	 * @return the socket file descriptor, -1 if no connection
	 */
	int getSocket() const noexcept;
	/**
	 * Returns a pointer to the SSL connection object associated with the
	 * currently established connection.
	 * @return pointer to the SSL object, nullptr if no connection
	 */
	SSL* getSecureSocket() const noexcept;
	/**
	 * Establishes a new connection after closing the existing one.
	 * @param sfd the socket file descriptor associated with the new connection
	 */
	void setSocket(int sfd);
	/**
	 * Establishes a new secure connection after closing the existing one.
	 * @param ssl pointer to an object representing the new secure connection
	 */
	void setSecureSocket(SSL *ssl);
	/**
	 * Releases the socket file descriptor associated with the currently
	 * established connection and returns it. The object no longer owns the
	 * file descriptor.
	 * @return the socket file descriptor, -1 if not connected
	 */
	int releaseSocket() noexcept;
	/**
	 * Releases the pointer to the SSL object associated with the currently
	 * established secure connection. The object no longer owns the SSL object.
	 * @return pointer to the SSL object, nullptr if no connection
	 */
	SSL* releaseSecureSocket() noexcept;
	/**
	 * Swaps the existing socket connection with a new one and returns the old
	 * socket file descriptor.
	 * @param sfd the new socket file descriptor
	 * @return the old socket file descriptor
	 */
	int swapSocket(int sfd);
	/**
	 * Swaps the existing secure connection with a new one and returns the old
	 * SSL object.
	 * @param ssl pointer to the new SSL object
	 * @return pointer to the old SSL object
	 */
	SSL* swapSecureSocket(SSL *ssl);
	/**
	 * Sets the receive and send timeout of the socket connection.
	 * @param recvTimeout the receive timeout in milliseconds. Set this value to
	 * 0 to block forever, negative timeout value will be ignored.
	 * @param sendTimeout the send timeout in milliseconds.  Set this value to
	 * 0 to block forever, negative timeout value will be ignored.
	 */
	void setSocketTimeout(int recvTimeout, int sendTimeout) const;
	//-----------------------------------------------------------------
	/**
	 * Sends out a request, the message length is determined by the routing
	 * header associated with this object.
	 * @param sign true to sign the message, false otherwise
	 */
	void send(bool sign = false);
	/**
	 * Receives a request.
	 * @param sequenceNumber the expected sequence number (0 to ignore).
	 * @param verify true to enable message verification, false otherwise.
	 */
	void receive(unsigned int sequenceNumber = 0, bool verify = false);
	/**
	 * Executes a request: sends a request and receives the response.
	 * @param sign true to sign the outgoing request, false otherwise
	 * @param verify true to verify the incoming response, false otherwise
	 * @return true on success, false otherwise
	 */
	bool executeRequest(bool sign = false, bool verify = false);
	/**
	 * Waits for a ping and then responds with a pong (replays back the ping).
	 */
	void sendPong();
	//-----------------------------------------------------------------
	/**
	 * Establishes a socket connection with the given host and returns the
	 * socket file descriptor.
	 * @param ni the resource name of the host
	 * @param sa the object for storing the socket address on success
	 * @param timeoutMils the send and receive timeout value to set for the
	 * newly created socket file descriptor. Set this value to 0 to block
	 * forever, -1 to ignore.
	 * @return a socket file descriptor
	 */
	static int connect(const NameInfo &ni, SocketAddress &sa, int timeoutMils =
			-1);
	/**
	 * Sends a request. If a signing key is provided (not nullptr) then the
	 * outgoing request will be signed before it's dispatch.
	 * @param sfd the socket file descriptor
	 * @param packet the outgoing request
	 * @param pki the request signing key (nullptr to ignore)
	 */
	static void send(int sfd, Packet &packet, const PKI *pki = nullptr);
	/**
	 * Sends a request.  If a signing key is provided (not nullptr) then the
	 * outgoing request will be signed before it's dispatch.
	 * @param ssl pointer to a secure connection object
	 * @param packet the outgoing request
	 * @param pki the request signing key (nullptr to ignore)
	 */
	static void send(SSL *ssl, Packet &packet, const PKI *pki = nullptr);
	/**
	 * Receives a response. If a verification key is provided (not nullptr) then
	 * the response's digital signature will be verified after it's receipt. If
	 * an "expected" sequence number is provided then any response not matching
	 * the expected sequence number will be silently dropped.
	 * @param sfd the socket file descriptor
	 * @param packet object for storing the incoming response
	 * @param sequenceNumber expected sequence number (0 to ignore)
	 * @param pki the response verification key (set to nullptr to ignore)
	 */
	static void receive(int sfd, Packet &packet,
			unsigned int sequenceNumber = 0, const PKI *pki = nullptr);
	/**
	 * Receives a response. If a verification key is provided (not nullptr) then
	 * the response's digital signature will be verified after it's receipt. If
	 * an "expected" sequence number is provided then any response not matching
	 * the expected sequence number will be silently dropped.
	 * @param ssl pointer to a secure connection object
	 * @param packet object for storing the response
	 * @param sequenceNumber the expected sequence number (0 to ignore)
	 * @param pki the response verification key (set to nullptr to ignore)
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

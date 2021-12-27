/*
 * Endpoint.h
 *
 * Basic routines for protocol implementation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_ENDPOINT_H_
#define WH_UTIL_ENDPOINT_H_
#include "FlowControl.h"
#include "Message.h"
#include "Packet.h"
#include "TransactionKey.h"
#include "Trust.h"
#include "../base/Network.h"
#include "../base/security/SSLContext.h"

namespace wanhive {
/**
 * Utility class for protocol implementation
 * Expects blocking socket connection
 * Thread safe at class level
 */
class Endpoint: protected FlowControl, protected Packet {
public:
	Endpoint() noexcept;
	~Endpoint();
	//-----------------------------------------------------------------
	/**
	 * SSL and public key management
	 */
	//Set context for SSL connections
	void setSSLContext(SSLContext *ctx) noexcept;
	//Get the current SSL context associated with this object
	SSLContext* getSSLContext() const noexcept;
	//Set the keys used for message signing and verification
	void useKeyPair(const PKI *pki = nullptr) noexcept;
	//Current PKI object (can be nullptr)
	const PKI* getKeyPair() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Connection management
	 */

	/*
	 * Establish a new connection after terminating the existing connection.
	 * If the new connection is a unix domain socket connection then the SSL
	 * context is cleared. <timeoutMils> specifies the IO timeout value in
	 * milliseconds (0 to block forever, -1 to ignore).
	 */
	void connect(const NameInfo &ni, int timeoutMils = -1);
	//Terminate any existing connection
	void disconnect();

	//Return the socket associated with this object
	int getSocket() const noexcept;
	//Return the SSL connection associated with this object
	SSL* getSecureSocket() const noexcept;
	/*
	 * Close the existing connection and set a new one. Throws an exception on
	 * SSL/TLS connection error.
	 */
	void setSocket(int sfd);
	/*
	 * Close the existing connection and set a new one. Throws an exception
	 * on SSL/TLS connection error.
	 */
	void setSecureSocket(SSL *ssl);
	//Release the underlying connection and return it
	int releaseSocket() noexcept;
	//Release the underlying connection and return it
	SSL* releaseSecureSocket() noexcept;
	/*
	 * Swap the underlying socket with <sfd> and return it. Throws an
	 * exception on SSL/TLS connection error.
	 */
	int swapSocket(int sfd);
	/*
	 * Swap the underlying SSL object with <ssl> and return it. Throws an
	 * exception on SSL/TLS connection error.
	 */
	SSL* swapSecureSocket(SSL *ssl);
	//Set receive and send timeout (milliseconds)
	void setSocketTimeout(int recvTimeout, int sendTimeout) const;
	//-----------------------------------------------------------------
	//Verify that message's context is correctly set in the header
	bool checkContext(uint8_t command, uint8_t qualifier) const noexcept;
	//Verify that message's context is correctly set in the header
	bool checkContext(uint8_t command, uint8_t qualifier,
			uint8_t status) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Messaging over blocking socket connection
	 * NOTE: Assign a PKI key-pair for signing and verification.
	 */
	//Send out a message, message length is taken from the deserialized header
	void send(bool sign = false);
	//Receive a message matching the <sequenceNumber> (set to 0 to ignore)
	void receive(unsigned int sequenceNumber = 0, bool verify = false);
	//Execute a request and receive the response, returns true on success
	bool executeRequest(bool sign = false, bool verify = false);
	//Waits for receipt of a ping and responds with a pong (for testing)
	void sendPong();
	/*
	 * Establish a blocking TCP socket connection with the host <ni> and return
	 * the socket file descriptor. <timeoutMils> specifies the IO timeout value
	 * in milliseconds (0 to block forever, -1 to ignore).
	 */
	static int connect(const NameInfo &ni, SocketAddress &sa, int timeoutMils =
			-1);
	/*
	 * If <pki> is provided then the message will be signed with its
	 * private key. <sfd> should be configured for blocking IO.
	 */
	static void send(int sfd, unsigned char *buf, unsigned int length,
			const PKI *pki = nullptr);
	//Same as the above but uses SSL/TLS connection
	static void send(SSL *ssl, unsigned char *buf, unsigned int length,
			const PKI *pki = nullptr);
	/*
	 * If <pki> is provided then the message will be verified using it's public
	 * key. If <sequenceNumber> is 0 then received message's sequence number is
	 * not verified. <sfd> should be configured for blocking IO.
	 */
	static void receive(int sfd, unsigned char *buf, MessageHeader &header,
			unsigned int sequenceNumber = 0, const PKI *pki = nullptr);
	//Same as the above but uses a secure SSL/TLS connection
	static void receive(SSL *ssl, unsigned char *buf, MessageHeader &header,
			unsigned int sequenceNumber = 0, const PKI *pki = nullptr);
private:
	int sockfd; //The underlying socket
	SSL *ssl;  //The underlying SSL/TLS connection
	SSLContext *sslContext;
	Trust trust; //PKI for message signing and verification
};

} /* namespace wanhive */

#endif /* WH_UTIL_ENDPOINT_H_ */

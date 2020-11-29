/*
 * Endpoint.h
 *
 * Basic routines for Wanhive protocol implementation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_ENDPOINT_H_
#define WH_UTIL_ENDPOINT_H_
#include "Message.h"
#include "TransactionKey.h"
#include "../base/Network.h"
#include "../base/security/SSLContext.h"

namespace wanhive {
/**
 * Utility class for Wanhive protocol implementation
 * Expects blocking socket connection
 * Thread safe at class level
 */
class Endpoint {
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
	//Establish a new connection, old connection is terminated
	void connect(const NameInfo &ni, int timeoutMils = -1);
	//Terminate the existing connection
	void disconnect();

	//Return the socket associated with this object
	int getSocket() const noexcept;
	//Return the SSL connection associated with this object
	SSL* getSecureSocket() const noexcept;
	/*
	 * Close the connection in use and set a new one,
	 * throws exception on SSL/TLS connection error.
	 */
	void setSocket(int socket);
	/*
	 * Close the connection in use and set a new one,
	 * throws exception on SSL/TLS connection error.
	 */
	void setSecureSocket(SSL *ssl);
	//Release the underlying connection and return it
	int releaseSocket() noexcept;
	//Release the underlying connection and return it
	SSL* releaseSecureSocket() noexcept;
	/*
	 * Swap the underlying socket with <socket> and return it,
	 * throws exception on SSL/TLS connection error.
	 */
	int swapSocket(int socket);
	/*
	 * Swap the underlying connection with <ssl> and return it,
	 * throws exception on SSL/TLS connection error.
	 */
	SSL* swapSecureSocket(SSL *ssl);
	//Set receive and send timeout (milliseconds)
	void setSocketTimeout(int recvTimeout, int sendTimeout) const;
	//-----------------------------------------------------------------
	/**
	 * Header and message management
	 */
	//Message's source identifier will be set to <source>
	void setSource(uint64_t source) noexcept;
	//Current message source identifier
	uint64_t getSource() const noexcept;
	//Reset the sequence number counter to the given value
	void setSequenceNumber(uint16_t sequenceNumber = 0) noexcept;
	//Current sequence number set in the counter
	uint16_t getSequenceNumber() const noexcept;
	//Return and increment the sequence number
	uint16_t nextSequenceNumber() noexcept;
	//Set session identifier
	void setSession(uint8_t session = 0) noexcept;
	//Currently active session
	uint8_t getSession() const noexcept;
	const MessageHeader& getHeader() const noexcept;
	//Pointer to the offset within the IO buffer (nullptr on overflow)
	const unsigned char* getBuffer(unsigned int offset = 0) const noexcept;
	//Pointer to offset within the payload (nullptr on overflow)
	const unsigned char* getPayload(unsigned int offset = 0) const noexcept;

	//Copy and serialize the <header> and the <payload>, <payload> can be nullptr
	bool pack(const MessageHeader &header,
			const unsigned char *payload) noexcept;
	//Copy the full message consisting of serialized header and payload
	bool pack(const unsigned char *message) noexcept;
	//Message length is automatically calculated, <format> can be nullptr
	bool pack(const MessageHeader &header, const char *format, ...) noexcept;
	bool pack(const MessageHeader &header, const char *format,
			va_list ap) noexcept;
	//Append the data at the end of the message and update message's length
	bool append(const char *format, ...) noexcept;
	bool append(const char *format, va_list ap) noexcept;
	//Deserialize the message header from the buffer
	void unpackHeader(MessageHeader &header) const noexcept;
	//Deserialize the payload data
	bool unpack(const char *format, ...) const noexcept;
	bool unpack(const char *format, va_list ap) const noexcept;
	//Verify that <command> and <qualifier> fields are correctly set in the header
	bool checkCommand(uint8_t command, uint8_t qualifier) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Blocking message IO
	 * NOTE: Internal PKI object is used for both signing and verification.
	 */
	//Send out a message, message length is taken from the message header
	void send(bool sign = false);
	//Receive a message, if <sequenceNumber> is not 0 then messages are discarded until a match
	void receive(unsigned int sequenceNumber = 0, bool verify = false);
	//Execute a request and receive the response, returns true if the target accepted the request
	bool executeRequest(bool sign = false, bool verify = false);
	//Generates a pong for a ping (for testing purposes)
	void sendPong();
protected:
	/**
	 * Following two methods grant the derived classes direct access to the internal
	 * structures for efficient implementation of additional protocols. These methods
	 * violate encapsulation and must be used with great care because incorrect usage
	 * can introduce hard to debug issues into the application.
	 */
	//Deserialized message header
	MessageHeader& header() noexcept;
	//Serialized IO buffer
	unsigned char* buffer() noexcept;
public:
	/*
	 * Establish TCP connection with the host <ni>.
	 * if <blocking> is true then the socket is configured for blocking IO (default),
	 * IO will fail after <timeoutMils> miliseconds (0 to block forever, -1 to ignore).
	 * Otherwise connect will be completed in background,
	 * use Selector to check whether connect returned successfully or not.
	 */
	static int connect(const NameInfo &ni, SocketAddress &sa, int timeoutMils =
			-1);
	/*
	 * If PKI is provided then the message will be signed using PKI's private key
	 * <sockfd> should be blocking
	 */
	static void send(int sockfd, unsigned char *buf, unsigned int length,
			const PKI *pki = nullptr);
	//Same as above however uses SSL/TLS connection
	static void send(SSL *ssl, unsigned char *buf, unsigned int length,
			const PKI *pki = nullptr);
	/*
	 * If PKI is provided then the message will be verified using PKI's public key
	 * If <sequenceNumber> == 0 received message's sequence number is not verified
	 * <sockfd> should be blocking
	 */
	static void receive(int sockfd, unsigned char *buf, MessageHeader &header,
			unsigned int sequenceNumber = 0, const PKI *pki = nullptr);
	//Same as above however uses SSL/TLS connection
	static void receive(SSL *ssl, unsigned char *buf, MessageHeader &header,
			unsigned int sequenceNumber = 0, const PKI *pki = nullptr);
	//-----------------------------------------------------------------
	/**
	 * Following functions pack and unpack a message (header and payload) to and
	 * from the <buffer>. <format> specifies the message payload format. If the
	 * payload is empty then format must be nullptr. All functions return the number
	 * of bytes written/read to/from the <buffer>, 0 if <buffer> is nullptr.
	 * NOTE 1: <buffer> contains the full message (header+payload) in either case.
	 * NOTE 2: Caller must ensure that <buffer> points to valid memory region of
	 * sufficient size, none of the functions perform overflow check.
	 */
	//<header> is copied verbatim however if the length field is 0 then the message length is calculated
	static unsigned int pack(const MessageHeader &header, unsigned char *buffer,
			const char *format, ...) noexcept;
	static unsigned int pack(const MessageHeader &header, unsigned char *buffer,
			const char *format, va_list list) noexcept;
	//<header> is populated from the serialized header data in the buffer
	static unsigned int unpack(MessageHeader &header,
			const unsigned char *buffer, const char *format, ...) noexcept;
	static unsigned int unpack(MessageHeader &header,
			const unsigned char *buffer, const char *format,
			va_list list) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Message authentication functions, return true on success, false on error.
	 * <pki> can be nullptr in which case the functions are noop and always return true.
	 * Functions may fail if invalid length or null pointer detected.
	 */
	//<length> is a value-result argument, always returns true if <pki> is nullptr
	static bool sign(unsigned char *out, unsigned int &length,
			const PKI *pki) noexcept;
	static bool sign(Message *msg, const PKI *pki) noexcept;
	//Verifies a signed message, always returns true if <pki> is nullptr
	static bool verify(const unsigned char *in, unsigned int length,
			const PKI *pki) noexcept;
	static bool verify(Message *msg, const PKI *pki) noexcept;
	//-----------------------------------------------------------------
	/*
	 * Execute a request-response cycle over blocking socket.
	 * If <signer> is provided then it's private key will be used for signing
	 * the message. If <verifier> is provided then it's public key will be used
	 * for message verification. Returns true if the request was accepted.
	 */
	static bool executeRequest(int sfd, MessageHeader &header,
			unsigned char *buf, const PKI *signer = nullptr,
			const PKI *verifier = nullptr);
	//Same as above however uses SSL/TLS connection
	static bool executeRequest(SSL *ssl, MessageHeader &header,
			unsigned char *buf, const PKI *signer = nullptr,
			const PKI *verifier = nullptr);
private:
	int sockfd; //The underlying socket
	SSL *ssl;  //The underlying SSL/TLS connection
	SSLContext *sslContext;
	const PKI *pki; //PKI for message signing and verification
	uint64_t sourceId; //Default source identifier of the outgoing messages
	uint16_t sequenceNumber;
	uint8_t session;
	MessageHeader _header; //The deserialized message header
	unsigned char _buffer[Message::MTU]; //The IO buffer
};

} /* namespace wanhive */

#endif /* WH_UTIL_ENDPOINT_H_ */

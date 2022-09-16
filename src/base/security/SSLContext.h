/*
 * SSLContext.h
 *
 * Secure connection's context
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SECURITY_SSLCONTEXT_H_
#define WH_BASE_SECURITY_SSLCONTEXT_H_
#include <openssl/ssl.h>

namespace wanhive {
/**
 * Secure connection's context
 */
class SSLContext {
public:
	/**
	 * Default constructor: creates an empty context.
	 */
	SSLContext() noexcept;
	/**
	 * Constructor: initializes the SSL/TLS context.
	 * @param certificate certificate file's pathname
	 * @param privateKey private key file's pathname
	 */
	SSLContext(const char *certificate, const char *privateKey);
	/**
	 * Destructor
	 */
	~SSLContext();
	//-----------------------------------------------------------------
	/**
	 * Returns The current SSL/TLS context.
	 * @return pointer to SSL/TLS context, nullptr if not initialized.
	 */
	SSL_CTX* getContext() const noexcept;
	/**
	 * Initializes the SSL/TLS context.
	 * @param certificate certificate file's pathname
	 * @param privateKey private key file's pathname
	 */
	void initialize(const char *certificate, const char *privateKey);
	/**
	 * Sets the CA certificates' location. If neither value is provided then the
	 * default system values are used.
	 * @param file CA certificate file's pathname
	 * @param CA certificates' directory name
	 */
	void loadTrustedPaths(const char *file, const char *path);
	//-----------------------------------------------------------------
	/**
	 * Creates a new secure connection.
	 * @param fd secure connection's file descriptor
	 * @param server true for server-side configuration, false for client
	 * @return pointer to secure connection object
	 */
	SSL* create(int fd, bool server);
	/**
	 * Checks secure connection's context.
	 * @param ssl secure connection to check
	 * @return true if the connection is in context, false otherwise
	 */
	bool inContext(const SSL *ssl) const noexcept;
	/**
	 * Establishes secure connection with a server.
	 * @param fd socket file descriptor (configured for blocking IO)
	 * @return pointer to secure connection object
	 */
	SSL* connect(int fd);
	//-----------------------------------------------------------------
	/**
	 * Returns secure connection's file descriptor.
	 * @param ssl pointer to secure connection object
	 * @return file descriptor's value
	 */
	static int getSocket(const SSL *ssl) noexcept;
	/**
	 * Assigns a file descriptor to the given secure connection object.
	 * @param ssl pointer to secure connection object
	 * @param fd socket file descriptor
	 * @return true on success, false on error
	 */
	static bool setSocket(SSL *ssl, int fd) noexcept;
	/**
	 * Returns the peer certificate verification status.
	 * @param ssl pointer to secure connection object
	 * @return true if verification succeeded, false otherwise
	 */
	static bool verify(const SSL *ssl) noexcept;
	/**
	 * Destroys a secure connection object (Doesn't close the file descriptor).
	 * @param ssl pointer to secure connection object
	 */
	static void destroy(SSL *ssl) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads exactly the given number of bytes from a blocking SSL connection.
	 * @param ssl pointer to secure connection object
	 * @param buf buffer for storing the incoming data
	 * @param length number of bytes to read
	 * @return number of bytes read from the secure connection
	 */
	static size_t receiveStream(SSL *ssl, unsigned char *buf, size_t length);
	/**
	 * Writes exactly the given number bytes to a blocking SSL connection.
	 * @param ssl pointer to secure connection object
	 * @param buf outgoing data buffer
	 * @param length number of bytes to write
	 * @return number of bytes written to the secure connection
	 */
	static size_t sendStream(SSL *ssl, const unsigned char *buf, size_t length);
private:
	//Install certificate and private key
	bool installKeys(const char *certificate, const char *privateKey) noexcept;
	//Clear the context
	void clear() noexcept;
private:
	SSL_CTX *ctx;
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_SSLCONTEXT_H_ */

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
#include "../common/NonCopyable.h"
#include <openssl/ssl.h>

namespace wanhive {
/**
 * Secure connection's context
 */
class SSLContext: private NonCopyable {
public:
	/**
	 * Default constructor: creates an empty context.
	 */
	SSLContext() noexcept;
	/**
	 * Constructor: initializes the SSL/TLS context.
	 * @param certificate certificate file's path
	 * @param privateKey private key file's path
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
	 * @param certificate certificate file's path
	 * @param privateKey private key file's path
	 */
	void initialize(const char *certificate, const char *privateKey);
	/**
	 * Sets the CA certificates' location. If neither value is provided then the
	 * default system values are used.
	 * @param file CA certificate file's path
	 * @param CA certificates' directory name
	 */
	void loadTrustedPaths(const char *file, const char *path);
	//-----------------------------------------------------------------
	/**
	 * Creates a new secure connection.
	 * @param fd socket's descriptor
	 * @param server true for server-side configuration, false for client
	 * @return secure connection
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
	 * @param fd blocking socket's descriptor
	 * @return secure connection
	 */
	SSL* connect(int fd);
	//-----------------------------------------------------------------
	/**
	 * Returns secure connection's file descriptor.
	 * @param ssl secure connection
	 * @return socket's descriptor
	 */
	static int getSocket(const SSL *ssl) noexcept;
	/**
	 * Associates SSL with a socket.
	 * @param ssl secure connection
	 * @param fd socket's descriptor
	 * @return true on success, false on error
	 */
	static bool setSocket(SSL *ssl, int fd) noexcept;
	/**
	 * Returns the remote peer certificate verification status.
	 * @param ssl secure connection
	 * @return true if verification succeeded, false otherwise
	 */
	static bool verify(const SSL *ssl) noexcept;
	/**
	 * Frees a secure connection (Doesn't close the underlying socket).
	 * @param ssl secure connection
	 */
	static void destroy(SSL *ssl) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads specified number of bytes from a blocking SSL connection.
	 * @param ssl secure connection
	 * @param buf stores the incoming data
	 * @param bytes number of bytes to read
	 * @return number of bytes transferred
	 */
	static size_t receive(SSL *ssl, unsigned char *buf, size_t bytes);
	/**
	 * Writes specified number bytes to a blocking SSL connection.
	 * @param ssl secure connection
	 * @param buf outgoing data buffer
	 * @param bytes number of bytes to write
	 * @return number of bytes transferred
	 */
	static size_t send(SSL *ssl, const unsigned char *buf, size_t bytes);
private:
	bool installKeys(const char *certificate, const char *privateKey) noexcept;
	void clear() noexcept;
private:
	SSL_CTX *ctx { nullptr };
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_SSLCONTEXT_H_ */

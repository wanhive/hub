/**
 * @file SSLContext.h
 *
 * SSL/TLS context
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

/*! @namespace wanhive */
namespace wanhive {
/**
 * SSL/TLS context
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
	 * @param key private key file's path
	 */
	SSLContext(const char *certificate, const char *key);
	/**
	 * Destructor
	 */
	~SSLContext();
	//-----------------------------------------------------------------
	/**
	 * Initializes the context and loads the certificate and the private key
	 * from PEM-encoded files.
	 * @param certificate certificate file's path
	 * @param key private key file's path
	 */
	void setup(const char *certificate, const char *key);
	/**
	 * Sets the location of CA certificates. If no value is provided, the system
	 * defaults are used.
	 * @param file trusted CA certificate file's path
	 * @param path trusted CA certificates' directory name
	 */
	void trust(const char *file, const char *path);
	//-----------------------------------------------------------------
	/**
	 * Creates a new secure connection.
	 * @param fd socket file descriptor
	 * @param server true for server-side configuration, false for client
	 * @return secure connection
	 */
	SSL* create(int fd, bool server);
	/**
	 * Verifies that a secure connection was created from this context.
	 * @param ssl secure connection
	 * @return true if the connection belongs to this context, false otherwise
	 */
	bool linked(const SSL *ssl) const noexcept;
	/**
	 * Establishes secure connection with a server.
	 * @param fd socket file descriptor configured for blocking IO
	 * @return secure connection
	 */
	SSL* connect(int fd);
	//-----------------------------------------------------------------
	/**
	 * Returns secure connection's socket file descriptor.
	 * @param ssl secure connection
	 * @return file descriptor, -1 on error
	 */
	static int getSocket(const SSL *ssl) noexcept;
	/**
	 * Sets socket file descriptor for a secure connection.
	 * @param ssl secure connection
	 * @param fd file descriptor
	 * @return true on success, false on error
	 */
	static bool setSocket(SSL *ssl, int fd) noexcept;
	/**
	 * Returns the peer certificate verification status.
	 * @param ssl secure connection
	 * @return true on successful verification, false on error
	 */
	static bool verify(const SSL *ssl) noexcept;
	/**
	 * Shuts down a secure connection gracefully (Doesn't close the socket).
	 * @param ssl secure connection
	 * @return 0 if the shutdown process is ongoing, 1 on successful completion
	 * (or nullptr argument), negative value on error.
	 */
	static int shutdown(SSL *ssl) noexcept;
	/**
	 * Frees a secure connection (Doesn't close the socket).
	 * @param ssl secure connection
	 */
	static void destroy(SSL *ssl) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads specified number of bytes from a blocking secure connection.
	 * @param ssl secure connection
	 * @param buf stores the incoming data
	 * @param bytes number of bytes to read
	 * @return number of bytes transferred
	 */
	static size_t receive(SSL *ssl, unsigned char *buf, size_t bytes);
	/**
	 * Writes specified number bytes to a blocking secure connection.
	 * @param ssl secure connection
	 * @param buf outgoing data buffer
	 * @param bytes number of bytes to write
	 * @return number of bytes transferred
	 */
	static size_t send(SSL *ssl, const unsigned char *buf, size_t bytes);
protected:
	/**
	 * Returns The current SSL/TLS context.
	 * @return pointer to SSL/TLS context, nullptr if not initialized
	 */
	SSL_CTX* get() const noexcept;
private:
	bool install(const char *certificate, const char *key) noexcept;
	void clear() noexcept;
private:
	SSL_CTX *ctx { };
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_SSLCONTEXT_H_ */

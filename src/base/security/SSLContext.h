/*
 * SSLContext.h
 *
 * SSL/TLS context and settings
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
 * Context for SSL/TLS connections
 * The class has been structured for creation and re-use of a single context
 * throughout a program's life cycle.
 * Thread safe at class level.
 */
class SSLContext {
public:
	SSLContext() noexcept;
	SSLContext(const char *certificate, const char *privateKey);
	virtual ~SSLContext();

	//The current context (possibly nullptr if not initialized)
	SSL_CTX* getContext() const noexcept;
	//Re-initialize the object (updates an existing SSL_CTX structure, creates a new one only if required)
	void initialize(const char *certificate, const char *privateKey);
	/*
	 * Set up the trusted files and/or paths for verification of peer certificate.
	 * If both <file> and <path> are null then default system values are used.
	 */
	void loadTrustedPaths(const char *file, const char *path);
	//=================================================================
	/*
	 * Utilities for handling of SSL/TLS connections
	 */
	//Creates a new TLS/SSL connection
	SSL* create(int fd, bool server);
	//Returns true if the SSL is in this context
	bool inContext(const SSL *ssl) const noexcept;
	//Establishes secure connection via the given blocking socket
	SSL* connect(int fd);

	//Returns the file descriptor linked to the secure connection (-1 on error)
	static int getSocket(const SSL *ssl) noexcept;
	//Assigns a file descriptor to the given SSL/TLS connection
	static bool setSocket(SSL *ssl, int fd) noexcept;
	//Returns true if peer certificate verification succeeded, false otherwise
	static bool verify(const SSL *ssl) noexcept;
	//Destroys a TLS/SSL connection object (Doesn't close the file descriptor)
	static void destroy(SSL *ssl) noexcept;

	//Reads exactly <length> bytes from a blocking SSL connection
	static size_t receiveStream(SSL *ssl, unsigned char *buf, size_t length);
	//Writes exactly <length> bytes to a blocking SSL connection
	static size_t sendStream(SSL *ssl, const unsigned char *buf, size_t length);
private:
	//Install certificate and private key
	bool installKeys(const char *certificate, const char *privateKey) noexcept;
	void clear() noexcept;
private:
	SSL_CTX *ctx;
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_SSLCONTEXT_H_ */

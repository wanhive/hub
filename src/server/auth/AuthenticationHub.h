/*
 * AuthenticationHub.h
 *
 * Authentication hub
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_AUTH_AUTHENTICATIONHUB_H_
#define WH_SERVER_AUTH_AUTHENTICATIONHUB_H_
#include "../../hub/Hub.h"
#include "../../util/Authenticator.h"

namespace wanhive {
/**
 * Authentication hub (distributed authenticator)
 * @note Uses SRP-6a protocol for authentication
 */
class AuthenticationHub final: public Hub {
public:
	/**
	 * Constructor: creates a new hub.
	 * @param uid hub's identifier
	 * @param path configuration file's pathname (nullptr for default)
	 */
	AuthenticationHub(unsigned long long uid,
			const char *path = nullptr) noexcept;
	/**
	 * Destructor
	 */
	~AuthenticationHub();
private:
	void stop(Watcher *w) noexcept override;
	void configure(void *arg) override;
	void cleanup() noexcept override;
	void route(Message *message) noexcept override;
	//-----------------------------------------------------------------
	//User -> Host:  I, A; Host -> User:  s, B
	int handleIdentificationRequest(Message *message) noexcept;
	//User -> Host: proof; Host -> User: proof
	int handleAuthenticationRequest(Message *message) noexcept;
	int handleAuthorizationRequest(Message *message) noexcept;
	int handleInvalidRequest(Message *message) noexcept;
	//-----------------------------------------------------------------
	//Returns true if the given identity is banned
	bool isBanned(unsigned long long identity) const noexcept;
	//Load the identity from a database
	bool loadIdentity(Authenticator *authenticator, unsigned long long identity,
			const Data &nonce) noexcept;
	//Generates an identification response message
	int generateIdentificationResponse(Message *message, const Data &salt,
			const Data &nonce) noexcept;
	//-----------------------------------------------------------------
	//Establishes database connection
	void* getDatabaseConnection() noexcept;
	//Closes database connection
	void closeDatabaseConnection() noexcept;
	//Iterator for populating the database parameters
	static int loadDatabaseParams(const char *option, const char *value,
			void *arg) noexcept;
	//Iterator for cleaning up the lookup table during shut down
	static int deleteAuthenticators(unsigned int index, void *arg) noexcept;
private:
	//Hash table of pending authentication requests
	Kmap<unsigned long long, Authenticator*> waitlist;
	//For obfuscation of failed identification requests
	Authenticator fake;
	//For database connection management
	struct DbConnection {
		void *conn;
		const char *name;
		const char *query;
		struct {
			const char *keys[64];
			const char *values[64];
			unsigned int index;
		} params;
	};
	//Configuration data
	struct {
		DbConnection db;
		const unsigned char *salt;
		unsigned int saltLength;
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_SERVER_AUTH_AUTHENTICATIONHUB_H_ */

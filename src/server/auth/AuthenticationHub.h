/*
 * AuthenticationHub.h
 *
 * Wanhive host for distributed authentication
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
 * The authentication hub
 * Uses SRP-6a protocol
 */
class AuthenticationHub: public Hub {
public:
	AuthenticationHub(unsigned long long uid,
			const char *path = nullptr) noexcept;
	virtual ~AuthenticationHub();
private:
	void stop(Watcher *w) noexcept override final;
	void configure(void *arg) final;
	void cleanup() noexcept override final;
	void route(Message *message) noexcept override final;
	//-----------------------------------------------------------------
	//User -> Host:  I, A; Host -> User:  s, B
	int handleIdentificationRequest(Message *message) noexcept;
	//User -> Host: proof; Host -> User: proof
	int handleAuthenticationRequest(Message *message) noexcept;
	int handleAuthorizationRequest(Message *message) noexcept;
	int handleInvalidRequest(Message *message) noexcept;
	//Load the identity from a database
	bool loadIdentity(Authenticator *authenticator, unsigned long long identity,
			const unsigned char *nonce, unsigned int nonceLength) noexcept;
	//Returns true if the given identity is banned
	bool isBanned(unsigned long long identity) const noexcept;
	//-----------------------------------------------------------------
	//Helper function for <handleIdentificationRequest>
	int generateIdentificationResponse(Message *message,
			unsigned int saltLength, unsigned int nonceLength,
			const unsigned char *salt, const unsigned char *nonce) noexcept;
	//Iterator for cleaning up the lookup table during shut down
	static int _deleteAuthenticators(unsigned int index, void *arg) noexcept;
private:
	//Look up table of the authenticators
	Khash<unsigned long long, Authenticator*> session;
	//For obfuscating failed identification
	Authenticator fake;

	struct {
		const char *connInfo;
		const char *query;
		const unsigned char *salt;
		unsigned int saltLength;
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_SERVER_AUTH_AUTHENTICATIONHUB_H_ */

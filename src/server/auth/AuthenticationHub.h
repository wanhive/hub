/**
 * @file AuthenticationHub.h
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
#include "Things.h"
#include "../../hub/Hub.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Authentication hub implementation
 */
class AuthenticationHub final: public Hub {
public:
	/**
	 * Constructor: creates a new hub.
	 * @param uid hub's identifier
	 * @param path configuration file's path
	 */
	AuthenticationHub(unsigned long long uid,
			const char *path = nullptr) noexcept;
	/**
	 * Destructor
	 */
	~AuthenticationHub();
private:
	void expel(Watcher *w) noexcept override;
	void configure(void *arg) override;
	void cleanup() noexcept override;
	void maintain() noexcept override;
	void route(Message *message) noexcept override;
	//-----------------------------------------------------------------
	int handleIdentificationRequest(Message *message) noexcept;
	int handleAuthenticationRequest(Message *message) noexcept;
	int handleAuthorizationRequest(Message *message) noexcept;
	int handleInvalidRequest(Message *message) noexcept;
	//-----------------------------------------------------------------
	bool isBanned(unsigned long long identity) const noexcept;
	bool loadIdentity(Verifier *verifier, unsigned long long identity,
			const Data &nonce) noexcept;
	int generateIdentificationResponse(Message *message, const Data &salt,
			const Data &nonce) noexcept;
	void setup();
	void clear() noexcept;
	//-----------------------------------------------------------------
	static int loadDatabaseParams(const char *option, const char *value,
			void *arg) noexcept;
	static int deleteVerifiers(unsigned int index, void *arg) noexcept;
private:
	Kmap<unsigned long long, Verifier*> waitlist;
	Verifier fake { true };
	Things things;
	struct DBConnection {
		DBInfo info;
		unsigned int index { };
		const char *command { };
		Data seed { };
	} dbi;
};

} /* namespace wanhive */

#endif /* WH_SERVER_AUTH_AUTHENTICATIONHUB_H_ */

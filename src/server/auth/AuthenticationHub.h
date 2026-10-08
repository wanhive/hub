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
#include "../../hub/Hub.h"
#include "../../base/db/DataStore.h"
#include "../../util/Verifier.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Authentication hub implementation
 */
class AuthenticationHub final: public Hub, private DataStore {
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
	int handlePakeRequest(Message *message) noexcept;
	int handlePassKeyRequest(Message *message) noexcept;
	int handleIdentificationRequest(Message *message) noexcept;
	int handleAuthenticationRequest(Message *message) noexcept;
	int handleAuthorizationRequest(Message *message) noexcept;
	int handleResolutionRequest(Message *message) noexcept;
	int handleTokenRequest(Message *message) noexcept;
	int handleInvalidRequest(Message *message) noexcept;
	//-----------------------------------------------------------------
	bool isBanned(unsigned long long identity) const noexcept;
	bool loadIdentity(Verifier *verifier, unsigned long long identity,
			const Data &nonce) noexcept;
	bool loadIdentity(Message *message, unsigned int &group) noexcept;
	int generateIdentificationResponse(Message *message, const Data &salt,
			const Data &nonce) noexcept;
	int generateAuthorizationResponse(Message *message,
			unsigned long long identity, unsigned int group) noexcept;
	void resolve(unsigned long long identity, const Data &nonce,
			Verifier *verifier);
	unsigned int resolve(Message *message);
	void setup();
	void clear() noexcept;
	//-----------------------------------------------------------------
	static const Digest* getDigest(const Message *message, unsigned int index =
			0) noexcept;
	static int loadDatabaseParams(const char *option, const char *value,
			void *arg) noexcept;
	static int deleteVerifiers(unsigned int index, void *arg) noexcept;
private:
	struct Resolved {
		unsigned long long identity;
		unsigned char group;
	};

	struct DBConnection {
		DBInfo info;
		unsigned int index { };
		const char *command { };
		Data seed { };
	} dbi;

	Hash hash;
	Kmap<unsigned long long, Verifier*> pake;
	Kmap<unsigned long long, Resolved> pkey;
	Verifier dummy { true };
};

} /* namespace wanhive */

#endif /* WH_SERVER_AUTH_AUTHENTICATIONHUB_H_ */

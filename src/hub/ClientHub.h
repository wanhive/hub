/*
 * ClientHub.h
 *
 * The base class for the Wanhive clients
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_CLIENTHUB_H_
#define WH_HUB_CLIENTHUB_H_
#include "Hub.h"
#include "../util/Authenticator.h"

namespace wanhive {
/**
 * Wanhive client implementation
 * Thread safe at class level
 */
class ClientHub: public Hub {
public:
	ClientHub(unsigned long long uid, const char *path = nullptr) noexcept;
	virtual ~ClientHub();

	//Returns true if the client is connected with the network
	bool isConnected() const noexcept;
protected:
	//-----------------------------------------------------------------
	void stop(Watcher *w) noexcept override;
	void configure(void *arg) override;
	void cleanup() noexcept override;
	void route(Message *message) noexcept override;
	void maintain() noexcept override;
	//-----------------------------------------------------------------
	//Allow derived classes to override the values loaded during configuration
	void setPassword(const unsigned char *password, unsigned int length,
			unsigned int rounds) noexcept;
private:
	//Connect to the authentication node
	void connectToAuthenticator() noexcept;
	//Connect to the overlay network
	void connectToOverlay() noexcept;
	//Checks whether the current stage is taking longer than expected to finish
	bool checkStageTimeout(unsigned int milliseconds) const noexcept;
	//Called by processIdentificationResponse
	void initAuthentication() noexcept;
	//Called by processFindRootResponse
	void findRoot() noexcept;
	//Called by processGetKeyResponse
	void initAuthorization() noexcept;

	Message* createIdentificationRequest();
	void processIdentificationResponse(Message *msg) noexcept;

	Message* createAuthenticationRequest();
	void processAuthenticationResponse(Message *msg) noexcept;

	Message* createFindRootRequest();
	void processFindRootResponse(Message *msg) noexcept;

	void processGetKeyResponse(Message *msg) noexcept;

	Message* createRegistrationRequest(bool sign);
	void processRegistrationResponse(Message *msg) noexcept;

	void setStage(int stage) noexcept;
	int getStage() const noexcept;
	bool isStage(int stage) const noexcept;
	void loadIdentifiers(bool auth);
	void clearIdentifiers() noexcept;
	void clear() noexcept;
private:
	volatile bool connected;
	/**
	 * Client hub configuration
	 */
	struct {
		//Usually, clear text password
		const unsigned char *password;
		//Password length
		unsigned int passwordLength;
		//Password hash rounds
		unsigned int passwordHashRounds;
		//Communication timeout
		unsigned int timeOut;
		//Wait for these many milliseconds before reconnecting
		unsigned int retryInterval;
	} ctx;

	/**
	 * For bootstrapping and connection management
	 */
	static constexpr unsigned int NODES = 16;
	struct {
		StaticBuffer<unsigned long long, NODES> identifiers;
		unsigned long long root;

		Socket *auth;
		Socket *node;

		Hash hashFn;
		Digest nonce;
		Authenticator authenticator;

		Timer timer;
		int stage;
	} bs;
};

} /* namespace wanhive */

#endif /* WH_HUB_CLIENTHUB_H_ */

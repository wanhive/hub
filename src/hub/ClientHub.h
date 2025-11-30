/*
 * ClientHub.h
 *
 * The base class for wanhive clients
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
#include "../util/Verifier.h"

namespace wanhive {
/**
 * Client hub implementation
 */
class ClientHub: public Hub {
public:
	/**
	 * Constructor: creates a new client
	 * @param uid hub's unique identifier
	 * @param path pathname of the configuration file
	 */
	ClientHub(unsigned long long uid, const char *path = nullptr) noexcept;
	/**
	 * Destructor
	 */
	~ClientHub();
protected:
	//-----------------------------------------------------------------
	void expel(Watcher *w) noexcept override;
	void configure(void *arg) override;
	void cleanup() noexcept override;
	void route(Message *message) noexcept override;
	void maintain() noexcept override;
	//-----------------------------------------------------------------
	/**
	 * Checks the network connection.
	 * @return true if connected, false otherwise
	 */
	bool isConnected() const noexcept;
	/**
	 * Sets a secure password for authentication.
	 * @param password the secure password
	 * @param length password length in bytes
	 * @param rounds password hash rounds
	 */
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
	/*
	 * Configuration data
	 */
	struct {
		//Password for authentication
		unsigned char password[64];
		//Password length
		unsigned int passwordLength;
		//Password hash rounds
		unsigned int rounds;
		//Communication timeout
		unsigned int timeout;
		//Wait for these many milliseconds before reconnecting
		unsigned int pause;
	} ctx;

	/*
	 * Bootstrapping and connection management
	 */
	struct {
		StaticBuffer<unsigned long long, 16> identifiers;
		unsigned long long root;

		Socket *auth;
		Socket *node;

		Hash hashFn;
		Digest nonce;
		Verifier verifier;

		Timer timer;
		int stage;
		bool connected;
	} bs;
};

} /* namespace wanhive */

#endif /* WH_HUB_CLIENTHUB_H_ */

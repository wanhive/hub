/*
 * Agent.h
 *
 * Wanhive Agent
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_AGENT_H_
#define WH_HUB_AGENT_H_
#include "Hub.h"
#include "../util/Verifier.h"

namespace wanhive {
/**
 * Agent implementation
 */
class Agent: public Hub {
public:
	/**
	 * Constructor: creates a new client
	 * @param uid hub's unique identifier
	 * @param path pathname of the configuration file
	 */
	Agent(unsigned long long uid, const char *path = nullptr) noexcept;
	/**
	 * Destructor
	 */
	~Agent();
protected:
	//-----------------------------------------------------------------
	void expel(Watcher *w) noexcept override;
	void configure(void *arg) override;
	void cleanup() noexcept override;
	void maintain() noexcept override;
	void route(Message *message) noexcept override;
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
	//Connect to an authentication hub
	void connectToAuthenticator() noexcept;
	//Connect to the overlay network
	void connectToOverlay() noexcept;
	//Checks whether the current stage is taking longer than expected to finish
	bool overdue(unsigned int milliseconds) const noexcept;
	//Helper for identification routine
	void initAuthentication() noexcept;
	//Helper for bootstrapping routine
	void findRoot() noexcept;
	//Helper for session key routine
	void initAuthorization() noexcept;

	Message* createIdentificationRequest();
	void processIdentificationResponse(const Message *msg) noexcept;

	Message* createAuthenticationRequest();
	void processAuthenticationResponse(const Message *msg) noexcept;

	Message* createFindRootRequest();
	void processFindRootResponse(const Message *msg) noexcept;

	void processTokenResponse(const Message *msg) noexcept;

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

#endif /* WH_HUB_AGENT_H_ */

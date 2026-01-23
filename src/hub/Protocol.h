/*
 * Protocol.h
 *
 * Wanhive protocol implementation
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_PROTOCOL_H_
#define WH_HUB_PROTOCOL_H_
#include "../util/Endpoint.h"
#include "../util/FlowControl.h"
#include "../util/Hash.h"
#include "../util/Message.h"

namespace wanhive {
/**
 * Asymmetric key and nonce for handshaking
 */
struct Token {
	PKI *keys;
	Digest *nonce;
};
//-----------------------------------------------------------------
/**
 * Wanhive protocol implementation
 * @note Supports blocking IO only
 */
class Protocol: protected Endpoint, protected FlowControl {
public:
	Protocol() noexcept;
	~Protocol();
	//-----------------------------------------------------------------
	/**
	 * Creates an identification request.
	 * @param address message's address
	 * @param nonce the public ephemeral value
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createIdentificationRequest(const MessageAddress &address,
			const Data &nonce) noexcept;
	/**
	 * Processes the response to an identification request.
	 * @param salt object for storing the salt
	 * @param nonce object for storing the host's public ephemeral value
	 * @return message length on success, 0 if request denied or invalid response
	 */
	unsigned int processIdentificationResponse(Data &salt,
			Data &nonce) const noexcept;
	/**
	 * Executes an identification request, doesn't process the response, call
	 * Protocol::processIdentificationResponse() explicitly to process the
	 * response and extract data.
	 * @param address message's address
	 * @param nonce the public ephemeral value
	 * @return true on success, false on error (request denied by the host)
	 */
	bool identificationRequest(const MessageAddress &address,
			const Data &nonce);
	/**
	 * Creates an authentication request.
	 * @param address message's address
	 * @param proof caller's proof of identity
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createAuthenticationRequest(const MessageAddress &address,
			const Data &proof) noexcept;
	/**
	 * Processes the response to an authentication request.
	 * @param proof object for storing the host's proof of identity
	 * @return message length on success, 0 if request denied or invalid response
	 */
	unsigned int processAuthenticationResponse(Data &proof) const noexcept;
	/**
	 * Executes an authentication request, doesn't process the response, call
	 * Protocol::processAuthenticationResponse() explicitly to process the
	 * response and extract data.
	 * @param address message's address
	 * @param proof caller's proof of identity
	 * @return true on success, false on error (request denied by the host)
	 */
	bool authenticationRequest(const MessageAddress &address,
			const Data &proof);
	//-----------------------------------------------------------------
	/**
	 * Creates a registration request.
	 * @param address message's address fields
	 * @param hc the session key (see Protocol::createGetKeyRequest())
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createRegisterRequest(const MessageAddress &address,
			Digest *hc) noexcept;
	/**
	 * Processes the response to a registration request.
	 * @return message length on success, 0 if request denied or invalid response
	 */
	unsigned int processRegisterResponse() const noexcept;
	/**
	 * Executes and processes a registration request.
	 * @param address message's address
	 * @param hc the session key (see Protocol::createGetKeyRequest())
	 * @return true on success, false on error (request denied by the host)
	 */
	bool registerRequest(const MessageAddress &address, Digest *hc);
	/**
	 * Creates a session key request.
	 * @param address message's address
	 * @param hc the nonce for replay prevention (can be nullptr)
	 * @param verify true if host verification is desired, false otherwise
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createTokenRequest(const MessageAddress &address, Digest *hc,
			bool verify) noexcept;
	/**
	 * Processes the response to a session key request.
	 * @param hc object for storing the session key returned by the host
	 * @return message length on success, 0 if request denied or invalid response
	 */
	unsigned int processTokenResponse(Digest *hc) const noexcept;
	/**
	 * Executes and processes a session key request.
	 * @param address message's address
	 * @param hc a value-result argument that provides a nonce for the request
	 * (replay prevention) and stores the session key returned by the host.
	 * @param verify true if host verification is desired, false otherwise
	 * @return true on success, false on error (request denied by the host)
	 */
	bool tokenRequest(const MessageAddress &address, Digest *hc, bool verify);
	//-----------------------------------------------------------------
	/**
	 * Creates a find-root request to discover the given client's root host.
	 * @param host recipient's identifier (can be set to zero)
	 * @param identity client's identifier
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createFindRootRequest(uint64_t host,
			uint64_t identity) noexcept;
	/**
	 * Processes the response to a find-root request.
	 * @param identity client's identifier
	 * @param root object for storing the root's identifier
	 * @return message length on success, 0 if request denied or invalid response
	 */
	unsigned int processFindRootResponse(uint64_t identity,
			uint64_t &root) const noexcept;
	/**
	 * Executes and processes a find-root request to discover the given client's
	 * root host.
	 * @param host recipient's identifier (can be set to zero)
	 * @param identity client's identifier
	 * @param root object for storing the root's identifier
	 * @return true on success, false on error (request denied by the host)
	 */
	bool findRootRequest(uint64_t host, uint64_t identity, uint64_t &root);
	/**
	 * Creates a bootstrap request for finding a list of bootstrap nodes.
	 * @param host recipient's identifier (can be set to zero)
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createBootstrapRequest(uint64_t host) noexcept;
	/**
	 * Processes the response to a bootstrap request
	 * @param keys buffer for storing the bootstrap node identifiers
	 * @param limit a value-result argument which provides the buffer's capacity
	 * and stores the actual number of identifiers stored into the buffer.
	 * @return message length on success, 0 if request denied or invalid response
	 */
	unsigned int processBootstrapResponse(uint64_t keys[],
			uint32_t &limit) const noexcept;
	/**
	 * Executes and processes a bootstrap request for finding a list of bootstrap
	 * nodes.
	 * @param host recipient's identifier (can be set to zero)
	 * @param keys buffer for storing the bootstrap node identifiers
	 * @param limit a value-result argument which provides the buffer's capacity
	 * and receives the actual number of identifiers stored into the buffer.
	 * @return true on success, false on error (request denied by the host)
	 */
	bool bootstrapRequest(uint64_t host, uint64_t keys[], uint32_t &limit);
	//-----------------------------------------------------------------
	/**
	 * Creates a publish request for writing data to the given topic.
	 * @param host host's identifier (can be set to zero)
	 * @param topic topic identifier
	 * @param data data to write on the given topic
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createPublishRequest(uint64_t host, uint8_t topic,
			const Data &data) noexcept;
	/**
	 * Executes a publish request (writes data to the given topic).
	 * @param host host's identifier (can be set to zero)
	 * @param topic topic identifier
	 * @param data data to write on the given topic
	 * @return true on success, false on error (request denied by the host)
	 */
	bool publishRequest(uint64_t host, uint8_t topic, const Data &data);

	/**
	 * Creates a subscription request for subscribing to a given topic.
	 * @param host host's identifier (can be set to zero)
	 * @param topic topic identifier
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createSubscribeRequest(uint64_t host, uint8_t topic) noexcept;
	/**
	 * Processes the response to a subscription request.
	 * @param topic topic identifier (to validate the response)
	 * @return message length on success, 0 if request denied or invalid response
	 */
	unsigned int processSubscribeResponse(uint8_t topic) const noexcept;
	/**
	 * Executes and processes a subscription request for subscribing to a
	 * given topic.
	 * @param host host's identifier (can be set to zero)
	 * @param topic topic identifier
	 * @return true on success, false on error (request denied by the host)
	 */
	bool subscribeRequest(uint64_t host, uint8_t topic);

	/**
	 * Creates an un-subscription request to remove subscription from a
	 * given topic.
	 * @param host host's identifier (can be set to zero)
	 * @param topic topic identifier
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createUnsubscribeRequest(uint64_t host, uint8_t topic) noexcept;
	/**
	 * Processes the response to a un-subscription request.
	 * @param topic topic identifier
	 * @return message length on success, 0 if request denied or invalid response
	 */
	unsigned int processUnsubscribeResponse(uint8_t topic) const noexcept;
	/**
	 * Executes and processes an un-subscription request to remove subscription
	 * from a given topic.
	 * @param host host's identifier (can be set to zero)
	 * @param topic topic identifier
	 * @return true on success, false on error (request denied by the host)
	 */
	bool unsubscribeRequest(uint64_t host, uint8_t topic);
	//-----------------------------------------------------------------
	/**
	 * Creates message containing an identification request.
	 * @param address message's address
	 * @param nonce the public ephemeral value
	 * @param sequenceNumber message's sequence number
	 * @return a new message containing the identification request on success,
	 * nullptr on error (invalid request or could not create a new message).
	 */
	static Message* createIdentificationRequest(const MessageAddress &address,
			const Data &nonce, uint16_t sequenceNumber) noexcept;
	/**
	 * Processes a message containing the response to an identification request.
	 * @param msg the response
	 * @param salt object for storing the salt
	 * @param nonce object for storing the host's public ephemeral value
	 * @return message length on success, 0 if request denied or invalid response
	 */
	static unsigned int processIdentificationResponse(const Message *msg,
			Data &salt, Data &nonce);

	/**
	 * Creates message containing an authentication request.
	 * @param address message's address
	 * @param proof caller's proof of identity
	 * @param sequenceNumber message's sequence number
	 * @return a new message containing the authentication request on success,
	 * nullptr on error (invalid request or could not create a new message).
	 */
	static Message* createAuthenticationRequest(const MessageAddress &address,
			const Data &proof, uint16_t sequenceNumber) noexcept;
	/**
	 * Processes a message containing the response to an authentication request.
	 * @param msg the response
	 * @param proof object for storing the host's proof
	 * @return message length on success, 0 if request denied or invalid response
	 */
	static unsigned int processAuthenticationResponse(const Message *msg,
			Data &proof) noexcept;

	/**
	 * Creates a registration request and stores it in the given message.
	 * @param address message's address
	 * @param hc the session key (see Protocol::createGetKeyRequest())
	 * @param msg the message for storing the registration request, if nullptr
	 * then a new message will be created and returned.
	 * @return message containing the registration request on success, nullptr on
	 * error (invalid request or could not create a new message).
	 */
	static Message* createRegisterRequest(const MessageAddress &address,
			const Digest *hc, Message *msg) noexcept;
	/**
	 * Creates a session key request and stores it in the given message.
	 * @param address message's address
	 * @param tk cryptographic key-nonce pair
	 * @param msg the message for storing the session key request, if nullptr then
	 * a new message will be created and returned.
	 * @return message containing the session key request on success, nullptr on
	 * error (invalid request or could not create a new message).
	 */
	static Message* createTokenRequest(const MessageAddress &address,
			const Token &tk, Message *msg) noexcept;
	/**
	 * Processes a message containing the response to a session key request.
	 * @param msg the response
	 * @param hc object for storing the session key
	 * @return message length on success, 0 if request denied or invalid response
	 */
	static unsigned int processTokenResponse(const Message *msg,
			Digest *hc) noexcept;

	/**
	 * Creates message containing a find-root request to discover the given
	 * client's root host.
	 * @param address message's address
	 * @param identity client's identity
	 * @param sequenceNumber message's sequence number
	 * @return message containing the find-root request on success, nullptr on
	 * error (invalid request or could not create a new message).
	 */
	static Message* createFindRootRequest(const MessageAddress &address,
			uint64_t identity, uint16_t sequenceNumber) noexcept;
	/**
	 * Processes a message containing the response to a find-root request.
	 * @param msg the response
	 * @param identity client's identity (for response's validation)
	 * @param root object for storing the root's identifier
	 * @return message length on success, 0 if request denied or invalid response
	 */
	static unsigned int processFindRootResponse(const Message *msg,
			uint64_t identity, uint64_t &root) noexcept;
private:
	static unsigned int createIdentificationRequest(
			const MessageAddress &address, uint16_t sequenceNumber,
			const Data &nonce, Packet &packet) noexcept;
	static unsigned int processIdentificationResponse(const Packet &packet,
			Data &salt, Data &nonce) noexcept;
	static unsigned int createAuthenticationRequest(
			const MessageAddress &address, uint16_t sequenceNumber,
			const Data &proof, Packet &packet) noexcept;
	static unsigned int processAuthenticationResponse(const Packet &packet,
			Data &proof) noexcept;
	static unsigned int createRegisterRequest(const MessageAddress &address,
			uint16_t sequenceNumber, const Digest *hc, Packet &packet) noexcept;
	static unsigned int createTokenRequest(const MessageAddress &address,
			uint16_t sequenceNumber, const Token &tk, Packet &packet) noexcept;
	static unsigned int processTokenResponse(const Packet &packet,
			Digest *hc) noexcept;
	static unsigned int createFindRootRequest(const MessageAddress &address,
			uint64_t identity, uint16_t sequenceNumber, Packet &packet) noexcept;
	static unsigned int processFindRootResponse(const Packet &packet,
			uint64_t identity, uint64_t &root) noexcept;
};

} /* namespace wanhive */

#endif /* WH_HUB_PROTOCOL_H_ */

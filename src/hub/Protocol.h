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
	 * @param nonce public ephemeral value
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createIdentificationRequest(const MessageAddress &address,
			const Data &nonce) noexcept;
	/**
	 * Processes an identification response.
	 * @param salt stores the salt
	 * @param nonce stores host's public ephemeral value
	 * @return message length on success, 0 if request denied or invalid response
	 */
	unsigned int processIdentificationResponse(Data &salt,
			Data &nonce) const noexcept;
	/**
	 * Executes an identification request, doesn't process the response. Process
	 * the response explicitly to extract the data.
	 * @param address message's address
	 * @param nonce public ephemeral value
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
	 * Processes an authentication response.
	 * @param proof stores host's proof of identity
	 * @return message length on success, 0 if request denied or invalid response
	 */
	unsigned int processAuthenticationResponse(Data &proof) const noexcept;
	/**
	 * Executes an authentication request, doesn't process the response. Process
	 * the response explicitly to extract the data.
	 * @param address message's address
	 * @param proof caller's proof of identity
	 * @return true on success, false on error (request denied by the host)
	 */
	bool authenticationRequest(const MessageAddress &address,
			const Data &proof);
	//-----------------------------------------------------------------
	/**
	 * Creates a registration request.
	 * @param address message's address
	 * @param hc session key
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createRegisterRequest(const MessageAddress &address,
			Digest *hc) noexcept;
	/**
	 * Processes a registration response.
	 * @return message length on success, 0 on error
	 */
	unsigned int processRegisterResponse() const noexcept;
	/**
	 * Executes and processes a registration request.
	 * @param address message's address
	 * @param hc session key
	 * @return true on success, false on error (request denied by the host)
	 */
	bool registerRequest(const MessageAddress &address, Digest *hc);
	/**
	 * Creates a session key request.
	 * @param address message's address
	 * @param hc nonce for replay prevention (can be nullptr)
	 * @param verify true for host verification, false otherwise
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createTokenRequest(const MessageAddress &address, Digest *hc,
			bool verify) noexcept;
	/**
	 * Processes a session key response.
	 * @param hc stores the session key returned by the host
	 * @return message length on success, 0 on error
	 */
	unsigned int processTokenResponse(Digest *hc) const noexcept;
	/**
	 * Executes and processes a session key request.
	 * @param address message's address
	 * @param hc a value-result argument that provides a nonce for the request
	 * (replay prevention) and stores the session key returned by the host.
	 * @param verify true for host verification, false otherwise
	 * @return true on success, false on error (request denied by the host)
	 */
	bool tokenRequest(const MessageAddress &address, Digest *hc, bool verify);
	//-----------------------------------------------------------------
	/**
	 * Creates a find-root request to find the root host.
	 * @param host recipient's identifier (can be set to zero)
	 * @param identity client's identifier
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createFindRootRequest(uint64_t host,
			uint64_t identity) noexcept;
	/**
	 * Processes a find-root response.
	 * @param identity client's identifier
	 * @param root stores the root's identifier
	 * @return message length on success, 0 on error
	 */
	unsigned int processFindRootResponse(uint64_t identity,
			uint64_t &root) const noexcept;
	/**
	 * Executes and processes a find-root request to find the root host.
	 * @param host recipient's identifier (can be set to zero)
	 * @param identity client's identifier
	 * @param root stores the root's identifier
	 * @return true on success, false on error (request denied by the host)
	 */
	bool findRootRequest(uint64_t host, uint64_t identity, uint64_t &root);
	/**
	 * Creates a bootstrap request to find the bootstrap nodes.
	 * @param host recipient's identifier (can be set to zero)
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createBootstrapRequest(uint64_t host) noexcept;
	/**
	 * Processes a bootstrap response.
	 * @param keys stores the bootstrap node identifiers
	 * @param limit a value-result argument which provides the buffer's capacity
	 * and receives the actual number of identifiers stored in the buffer.
	 * @return message length on success, 0 on error
	 */
	unsigned int processBootstrapResponse(uint64_t keys[],
			uint32_t &limit) const noexcept;
	/**
	 * Executes and processes a bootstrap request to find the bootstrap nodes.
	 * @param host recipient's identifier (can be set to zero)
	 * @param keys stores the bootstrap node identifiers
	 * @param limit a value-result argument which provides the buffer's capacity
	 * and receives the actual number of identifiers stored in the buffer.
	 * @return true on success, false on error (request denied by the host)
	 */
	bool bootstrapRequest(uint64_t host, uint64_t keys[], uint32_t &limit);
	//-----------------------------------------------------------------
	/**
	 * Creates a publish request to a topic.
	 * @param host host's identifier (can be set to zero)
	 * @param topic topic identifier
	 * @param data data to publish
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createPublishRequest(uint64_t host, uint8_t topic,
			const Data &data) noexcept;
	/**
	 * Executes a publish request to a topic.
	 * @param host host's identifier (can be set to zero)
	 * @param topic topic identifier
	 * @param data data to publish
	 * @return true on success, false on error (request denied by the host)
	 */
	bool publishRequest(uint64_t host, uint8_t topic, const Data &data);

	/**
	 * Creates a subscription request to a topic.
	 * @param host host's identifier (can be set to zero)
	 * @param topic topic identifier
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createSubscribeRequest(uint64_t host, uint8_t topic) noexcept;
	/**
	 * Processes a subscription response to a topic.
	 * @param topic topic identifier (to validate the response)
	 * @return message length on success, 0 on error
	 */
	unsigned int processSubscribeResponse(uint8_t topic) const noexcept;
	/**
	 * Executes and processes a subscription request to a topic.
	 * @param host host's identifier (can be set to zero)
	 * @param topic topic identifier
	 * @return true on success, false on error (request denied by the host)
	 */
	bool subscribeRequest(uint64_t host, uint8_t topic);

	/**
	 * Creates an un-subscription request to a topic.
	 * @param host host's identifier (can be set to zero)
	 * @param topic topic identifier
	 * @return message length on success, 0 on error (invalid request)
	 */
	unsigned int createUnsubscribeRequest(uint64_t host, uint8_t topic) noexcept;
	/**
	 * Processes an un-subscription response to a topic.
	 * @param topic topic identifier
	 * @return message length on success, 0 on error
	 */
	unsigned int processUnsubscribeResponse(uint8_t topic) const noexcept;
	/**
	 * Executes and processes an un-subscription request to a topic.
	 * @param host host's identifier (can be set to zero)
	 * @param topic topic identifier
	 * @return true on success, false on error (request denied by the host)
	 */
	bool unsubscribeRequest(uint64_t host, uint8_t topic);
	//-----------------------------------------------------------------
	/**
	 * Creates an identification request.
	 * @param address message's address
	 * @param nonce public ephemeral value
	 * @param sequenceNumber message's sequence number
	 * @return identification request on success, nullptr on error
	 */
	static Message* createIdentificationRequest(const MessageAddress &address,
			const Data &nonce, uint16_t sequenceNumber) noexcept;
	/**
	 * Processes an identification response.
	 * @param msg identification response
	 * @param salt stores the salt
	 * @param nonce stores the host's public ephemeral value
	 * @return message length on success, 0 on error
	 */
	static unsigned int processIdentificationResponse(const Message *msg,
			Data &salt, Data &nonce);

	/**
	 * Creates an authentication request.
	 * @param address message's address
	 * @param proof caller's proof of identity
	 * @param sequenceNumber message's sequence number
	 * @return authentication request on success, nullptr on error
	 */
	static Message* createAuthenticationRequest(const MessageAddress &address,
			const Data &proof, uint16_t sequenceNumber) noexcept;
	/**
	 * Processes an authentication response.
	 * @param msg authentication response
	 * @param proof stores the host's proof
	 * @return message length on success, 0 on error
	 */
	static unsigned int processAuthenticationResponse(const Message *msg,
			Data &proof) noexcept;

	/**
	 * Creates a registration request.
	 * @param address message's address
	 * @param hc session key
	 * @param msg message to store the registration request, if nullptr then
	 * a new message will be created and returned.
	 * @return registration request on success, nullptr on error
	 */
	static Message* createRegisterRequest(const MessageAddress &address,
			const Digest *hc, Message *msg) noexcept;
	/**
	 * Creates a session key request.
	 * @param address message's address
	 * @param tk cryptographic key-nonce pair
	 * @param msg message to store the session key request, if nullptr then
	 * a new message will be created and returned.
	 * @return session key request on success, nullptr on error
	 */
	static Message* createTokenRequest(const MessageAddress &address,
			const Token &tk, Message *msg) noexcept;
	/**
	 * Processes a session key response.
	 * @param msg session key response
	 * @param hc stores the session key
	 * @return message length on success, 0 on error
	 */
	static unsigned int processTokenResponse(const Message *msg,
			Digest *hc) noexcept;

	/**
	 * Creates a find-root request to find the root host.
	 * @param address message's address
	 * @param identity client's identity
	 * @param sequenceNumber message's sequence number
	 * @return find-root request on success, nullptr on error
	 */
	static Message* createFindRootRequest(const MessageAddress &address,
			uint64_t identity, uint16_t sequenceNumber) noexcept;
	/**
	 * Processes a find-root response.
	 * @param msg find-root response
	 * @param identity client's identity (for response's validation)
	 * @param root stores the root's identifier
	 * @return message length on success, 0 on error
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

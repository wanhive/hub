/*
 * Protocol.h
 *
 * Bare minimum Wanhive protocol implementation
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

namespace wanhive {
/**
 * Bare minimum Wanhive protocol implementation
 * Expects blocking socket connection
 * Thread safe at class level
 */
class Protocol: protected Endpoint {
public:
	Protocol() noexcept;
	~Protocol();
	//-----------------------------------------------------------------
	/**
	 * Identification and authentication
	 * <id> = identifier of the remote host
	 */
	//Returns the message length on success, 0 on error
	unsigned int createIdentificationRequest(uint64_t id, uint64_t uid,
			const unsigned char *nonce, unsigned int nonceLength) noexcept;
	//Returns the message length on success, 0 on error
	unsigned int processIdentificationResponse(unsigned int &saltLength,
			const unsigned char *&salt, unsigned int &nonceLength,
			const unsigned char *&nonce) const noexcept;
	//Call processIdentificationResponse explicitly to process the response
	bool identificationRequest(uint64_t id, uint64_t uid,
			const unsigned char *nonce, unsigned int nonceLength);

	//Returns the message length on success, 0 on error
	unsigned int createAuthenticationRequest(uint64_t id,
			const unsigned char *proof, unsigned int length) noexcept;
	//Returns the message length on success, 0 on error
	unsigned int processAuthenticationResponse(unsigned int &length,
			const unsigned char *&proof) const noexcept;
	//Call processAuthenticationResponse explicitly to process the response
	bool authenticationRequest(uint64_t id, const unsigned char *proof,
			unsigned int length);
	//-----------------------------------------------------------------
	/**
	 * Registration and session creation
	 * <id> = identifier of the remote host
	 */
	//Returns the message length on success, 0 on error
	unsigned int createRegisterRequest(uint64_t id, uint64_t uid, Digest *hc =
			nullptr) noexcept;
	//Returns the message length on success, 0 on error
	unsigned int processRegisterResponse() const noexcept;
	//Returns true on success, false if request denied by the host
	bool registerRequest(uint64_t id, uint64_t uid, Digest *hc = nullptr);

	//Set <verify> to true if host verification is desired
	unsigned int createGetKeyRequest(uint64_t id, Digest *hc,
			bool verify) noexcept;
	//Returns the message length on success, 0 on error
	unsigned int processGetKeyResponse(Digest *hc) const noexcept;
	//Set <verify> to true if host verification is desired
	bool getKeyRequest(uint64_t id, Digest *hc, bool verify);
	//-----------------------------------------------------------------
	/**
	 * Bare minimum bootstrapping protocol
	 * <id> = identifier of the remote host
	 */
	//Returns the message length on success, 0 on error
	unsigned int createFindRootRequest(uint64_t id, uint64_t uid) noexcept;
	//Returns the message length on success, 0 on error
	unsigned int processFindRootResponse(uint64_t uid,
			uint64_t &root) const noexcept;
	//Returns true on success, false otherwise
	bool findRootRequest(uint64_t id, uint64_t uid, uint64_t &root);

	//Returns the message length on success, 0 on error
	unsigned int createBootstrapRequest(uint64_t id) noexcept;
	//Returns the message length on success, 0 on error
	unsigned int processBootstrapResponse(uint64_t keys[],
			uint32_t &limit) const noexcept;
	//Returns true on success, false otherwise
	bool bootstrapRequest(uint64_t id, uint64_t keys[], uint32_t &limit);
	//-----------------------------------------------------------------
	/**
	 * Bare minimum pub/sub protocol
	 * <id> = identifier of the remote host
	 */
	//Returns the message length on success, 0 on error
	unsigned int createPublishRequest(uint64_t id, uint8_t topic,
			const unsigned char *payload, unsigned int payloadLength) noexcept;
	//Always returns true
	bool publishRequest(uint64_t id, uint8_t topic,
			const unsigned char *payload, unsigned int payloadLength);

	//Returns the message length on success, 0 on error
	unsigned int createSubscribeRequest(uint64_t id, uint8_t topic) noexcept;
	//Returns the message length on success, 0 on error
	unsigned int processSubscribeResponse(uint8_t topic) const noexcept;
	//Returns true on success, false otherwise
	bool subscribeRequest(uint64_t id, uint8_t topic);

	//Returns the message length on success, 0 on error
	unsigned int createUnsubscribeRequest(uint64_t id, uint8_t topic) noexcept;
	//Returns the message length on success, 0 on error
	unsigned int processUnsubscribeResponse(uint8_t topic) const noexcept;
	//Returns true on success, false otherwise
	bool unsubscribeRequest(uint64_t id, uint8_t topic);
	//-----------------------------------------------------------------
	/**
	 * STATIC METHODS
	 * Bare minimum bootstrapping and registration protocol
	 * <host> is the identifier of the remote host.
	 * <uid> is the identifier of the local node.
	 */
	//Returns message length on success, 0 on failure
	static unsigned int createIdentificationRequest(uint64_t host, uint64_t uid,
			uint16_t sequenceNumber, const unsigned char *nonce,
			unsigned int nonceLength, MessageHeader &header,
			unsigned char *buf) noexcept;
	//Returns a new message on success, nullptr on failure
	static Message* createIdentificationRequest(uint64_t host, uint64_t uid,
			uint16_t sequenceNumber, const unsigned char *nonce,
			unsigned int nonceLength) noexcept;
	//Returns message length on success, 0 on failure
	static unsigned int processIdentificationResponse(
			const MessageHeader &header, const unsigned char *buf,
			unsigned int &saltLength, const unsigned char *&salt,
			unsigned int &nonceLength, const unsigned char *&nonce) noexcept;
	//Returns message length on success, 0 on failure
	static unsigned int processIdentificationResponse(const Message *msg,
			unsigned int &saltLength, const unsigned char *&salt,
			unsigned int &nonceLength, const unsigned char *&nonce) noexcept;

	//Returns message length on success, 0 on failure
	static unsigned int createAuthenticationRequest(uint64_t host,
			uint16_t sequenceNumber, const unsigned char *proof,
			unsigned int length, MessageHeader &header,
			unsigned char *buf) noexcept;
	//Returns a new message on success, nullptr on failure
	static Message* createAuthenticationRequest(uint64_t host,
			uint16_t sequenceNumber, const unsigned char *proof,
			unsigned int length) noexcept;
	//Returns message length on success, 0 on failure
	static unsigned int processAuthenticationResponse(
			const MessageHeader &header, const unsigned char *buf,
			unsigned int &length, const unsigned char *&proof) noexcept;
	//Returns message length on success, 0 on failure
	static unsigned int processAuthenticationResponse(const Message *msg,
			unsigned int &length, const unsigned char *&proof) noexcept;

	//Returns message length on success, 0 on failure
	static unsigned int createRegisterRequest(uint64_t host, uint64_t uid,
			uint16_t sequenceNumber, const Digest *hc, MessageHeader &header,
			unsigned char *buf) noexcept;
	//Returns <msg> (a new message if <msg> is nullptr) on success, nullptr otherwise
	static Message* createRegisterRequest(uint64_t host, uint64_t uid,
			const Digest *hc, Message *msg) noexcept;

	//Returns message length on success, 0 on failure
	static unsigned int createGetKeyRequest(uint64_t host,
			uint16_t sequenceNumber, const TransactionKey &tk,
			MessageHeader &header, unsigned char *buf) noexcept;
	//Returns <msg> (a new message if <msg> is nullptr) on success, nullptr otherwise
	static Message* createGetKeyRequest(uint64_t host, const TransactionKey &tk,
			Message *msg) noexcept;
	//Returns message length on success, 0 on failure (<hc> is the value-result argument)
	static unsigned int processGetKeyResponse(const MessageHeader &header,
			const unsigned char *buf, Digest *hc) noexcept;
	//Returns message length on success, 0 on failure (<hc> is the value-result argument)
	static unsigned int processGetKeyResponse(const Message *msg,
			Digest *hc) noexcept;

	//Returns message length on success, 0 on failure
	static unsigned int createFindRootRequest(uint64_t host, uint64_t uid,
			uint64_t identity, uint16_t sequenceNumber, MessageHeader &header,
			unsigned char *buf) noexcept;
	//Returns a new message on success, nullptr on failure
	static Message* createFindRootRequest(uint64_t host, uint64_t uid,
			uint64_t identity, uint16_t sequenceNumber) noexcept;
	//Returns message length on success, 0 on failure
	static unsigned int processFindRootResponse(const MessageHeader &header,
			const unsigned char *buf, uint64_t identity,
			uint64_t &root) noexcept;
	//Returns message length on success, 0 on failure
	static unsigned int processFindRootResponse(const Message *msg,
			uint64_t identity, uint64_t &root) noexcept;
};

} /* namespace wanhive */

#endif /* WH_HUB_PROTOCOL_H_ */

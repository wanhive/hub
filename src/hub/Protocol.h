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
class Protocol: protected Endpoint, protected FlowControl {
public:
	Protocol() noexcept;
	~Protocol();
	//-----------------------------------------------------------------
	/**
	 * Identification and authentication
	 * <host> = remote host's identifier
	 * <uid> = the local identifier
	 */
	//Returns the message length on success, 0 on error
	unsigned int createIdentificationRequest(const MessageAddress &address,
			const Data &nonce) noexcept;
	//Returns the message length on success, 0 on error
	unsigned int processIdentificationResponse(Data &salt,
			Data &nonce) const noexcept;
	//Call processIdentificationResponse explicitly to process the response
	bool identificationRequest(const MessageAddress &address,
			const Data &nonce);

	//Returns the message length on success, 0 on error
	unsigned int createAuthenticationRequest(const MessageAddress &address,
			const Data &proof) noexcept;
	//Returns the message length on success, 0 on error
	unsigned int processAuthenticationResponse(Data &proof) const noexcept;
	//Call processAuthenticationResponse explicitly to process the response
	bool authenticationRequest(const MessageAddress &address,
			const Data &proof);
	//-----------------------------------------------------------------
	/**
	 * Registration and session creation
	 * <host> = remote host's identifier
	 * <uid> = the local identifier
	 */
	//Returns the message length on success, 0 on error
	unsigned int createRegisterRequest(const MessageAddress &address,
			Digest *hc) noexcept;
	//Returns the message length on success, 0 on error
	unsigned int processRegisterResponse() const noexcept;
	//Returns true on success, false if request denied by the host
	bool registerRequest(const MessageAddress &address, Digest *hc);

	//Set <verify> to true if host verification is desired
	unsigned int createGetKeyRequest(const MessageAddress &address, Digest *hc,
			bool verify) noexcept;
	//Returns the message length on success, 0 on error
	unsigned int processGetKeyResponse(Digest *hc) const noexcept;
	//Set <verify> to true if host verification is desired
	bool getKeyRequest(const MessageAddress &address, Digest *hc, bool verify);
	//-----------------------------------------------------------------
	/**
	 * Bare minimum bootstrapping protocol
	 * <host> = remote host's identifier
	 * <uid> = the local identifier
	 */
	//Returns the message length on success, 0 on error
	unsigned int createFindRootRequest(uint64_t host,
			uint64_t identity) noexcept;
	//Returns the message length on success, 0 on error
	unsigned int processFindRootResponse(uint64_t identity,
			uint64_t &root) const noexcept;
	//Returns true on success, false otherwise
	bool findRootRequest(uint64_t host, uint64_t identity, uint64_t &root);

	//Returns the message length on success, 0 on error
	unsigned int createBootstrapRequest(uint64_t host) noexcept;
	//Returns the message length on success, 0 on error
	unsigned int processBootstrapResponse(uint64_t keys[],
			uint32_t &limit) const noexcept;
	//Returns true on success, false otherwise
	bool bootstrapRequest(uint64_t host, uint64_t keys[], uint32_t &limit);
	//-----------------------------------------------------------------
	/**
	 * Bare minimum pub/sub protocol
	 * <host> = remote host's identifier
	 * <uid> = the local identifier
	 */
	//Returns the message length on success, 0 on error
	unsigned int createPublishRequest(uint64_t host, uint8_t topic,
			const Data &data) noexcept;
	//Always returns true
	bool publishRequest(uint64_t host, uint8_t topic, const Data &data);

	//Returns the message length on success, 0 on error
	unsigned int createSubscribeRequest(uint64_t host, uint8_t topic) noexcept;
	//Returns the message length on success, 0 on error
	unsigned int processSubscribeResponse(uint8_t topic) const noexcept;
	//Returns true on success, false otherwise
	bool subscribeRequest(uint64_t host, uint8_t topic);

	//Returns the message length on success, 0 on error
	unsigned int createUnsubscribeRequest(uint64_t host, uint8_t topic) noexcept;
	//Returns the message length on success, 0 on error
	unsigned int processUnsubscribeResponse(uint8_t topic) const noexcept;
	//Returns true on success, false otherwise
	bool unsubscribeRequest(uint64_t host, uint8_t topic);
	//-----------------------------------------------------------------
	/**
	 * STATIC METHODS
	 * Bare minimum bootstrapping and registration protocol
	 * <host> = identifier of the remote host
	 * <uid> = the local identifier
	 */
	//Returns a new message on success, nullptr on failure
	static Message* createIdentificationRequest(const MessageAddress &address,
			const Data &nonce, uint16_t sequenceNumber) noexcept;
	//Returns message length on success, 0 on failure
	static unsigned int processIdentificationResponse(const Message *msg,
			Data &salt, Data &nonce);

	//Returns a new message on success, nullptr on failure
	static Message* createAuthenticationRequest(const MessageAddress &address,
			const Data &proof, uint16_t sequenceNumber) noexcept;
	//Returns message length on success, 0 on failure
	static unsigned int processAuthenticationResponse(const Message *msg,
			Data &proof) noexcept;

	//Returns <msg> (a new message if <msg> is nullptr) on success, nullptr otherwise
	static Message* createRegisterRequest(const MessageAddress &address,
			const Digest *hc, Message *msg) noexcept;

	//Returns <msg> (a new message if <msg> is nullptr) on success, nullptr otherwise
	static Message* createGetKeyRequest(const MessageAddress &address,
			const TransactionKey &tk, Message *msg) noexcept;
	//Returns message length on success, 0 on failure (<hc> is the value-result argument)
	static unsigned int processGetKeyResponse(const Message *msg,
			Digest *hc) noexcept;

	//Returns a new message on success, nullptr on failure
	static Message* createFindRootRequest(const MessageAddress &address,
			uint64_t identity, uint16_t sequenceNumber) noexcept;
	//Returns message length on success, 0 on failure
	static unsigned int processFindRootResponse(const Message *msg,
			uint64_t identity, uint64_t &root) noexcept;
private:
	//-----------------------------------------------------------------
	/**
	 * Helper functions
	 * <host> = identifier of the remote host
	 * <uid> = the local identifier
	 */
	//Returns message length on success, 0 on failure
	static unsigned int createIdentificationRequest(
			const MessageAddress &address, uint16_t sequenceNumber,
			const Data &nonce, Packet &packet) noexcept;
	//Returns message length on success, 0 on failure
	static unsigned int processIdentificationResponse(const Packet &packet,
			Data &salt, Data &nonce) noexcept;
	//Returns message length on success, 0 on failure
	static unsigned int createAuthenticationRequest(
			const MessageAddress &address, uint16_t sequenceNumber,
			const Data &proof, Packet &packet) noexcept;
	//Returns message length on success, 0 on failure
	static unsigned int processAuthenticationResponse(const Packet &packet,
			Data &proof) noexcept;
	//Returns message length on success, 0 on failure
	static unsigned int createRegisterRequest(const MessageAddress &address,
			uint16_t sequenceNumber, const Digest *hc, Packet &packet) noexcept;
	//Returns message length on success, 0 on failure
	static unsigned int createGetKeyRequest(const MessageAddress &address,
			uint16_t sequenceNumber, const TransactionKey &tk,
			Packet &packet) noexcept;
	//Returns message length on success, 0 on failure (<hc> is the value-result argument)
	static unsigned int processGetKeyResponse(const Packet &packet,
			Digest *hc) noexcept;
	//Returns message length on success, 0 on failure
	static unsigned int createFindRootRequest(const MessageAddress &address,
			uint64_t identity, uint16_t sequenceNumber, Packet &packet) noexcept;
	//Returns message length on success, 0 on failure
	static unsigned int processFindRootResponse(const Packet &packet,
			uint64_t identity, uint64_t &root) noexcept;
};

} /* namespace wanhive */

#endif /* WH_HUB_PROTOCOL_H_ */

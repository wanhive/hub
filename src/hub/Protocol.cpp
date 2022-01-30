/*
 * Protocol.cpp
 *
 * Bare minimum Wanhive protocol implementation
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Protocol.h"
#include "../base/ds/Serializer.h"
#include "../util/commands.h"

namespace wanhive {

Protocol::Protocol() noexcept {

}

Protocol::~Protocol() {

}

unsigned int Protocol::createIdentificationRequest(uint64_t host, uint64_t uid,
		const unsigned char *nonce, unsigned int nonceLength) noexcept {
	return createIdentificationRequest(host, uid, nextSequenceNumber(), nonce,
			nonceLength, *this);
}

unsigned int Protocol::processIdentificationResponse(unsigned int &saltLength,
		const unsigned char *&salt, unsigned int &nonceLength,
		const unsigned char *&nonce) const noexcept {
	return processIdentificationResponse(*this, saltLength, salt, nonceLength,
			nonce);
}

bool Protocol::identificationRequest(uint64_t host, uint64_t uid,
		const unsigned char *nonce, unsigned int nonceLength) {
	/*
	 * HEADER: SRC=<identity>, DEST=X, ....CMD=0, QLF=1, AQLF=0/1/127
	 * BODY: variable in Request and Response
	 * TOTAL: at least 32 bytes in Request and Response
	 */
	return createIdentificationRequest(host, uid, nonce, nonceLength)
			&& executeRequest();
}

unsigned int Protocol::createAuthenticationRequest(uint64_t host,
		const unsigned char *proof, unsigned int length) noexcept {
	return createAuthenticationRequest(host, nextSequenceNumber(), proof,
			length, *this);
}

unsigned int Protocol::processAuthenticationResponse(unsigned int &length,
		const unsigned char *&proof) const noexcept {
	return processAuthenticationResponse(*this, length, proof);
}

bool Protocol::authenticationRequest(uint64_t host, const unsigned char *proof,
		unsigned int length) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=0, QLF=2, AQLF=0/1/127
	 * BODY: variable in Request and Response
	 * TOTAL: at least 32 bytes in Request and Response
	 */
	return createAuthenticationRequest(host, proof, length) && executeRequest();
}

unsigned int Protocol::createRegisterRequest(uint64_t host, uint64_t uid,
		Digest *hc) noexcept {
	return createRegisterRequest(host, uid, nextSequenceNumber(), hc, *this);
}

unsigned int Protocol::processRegisterResponse() const noexcept {
	if (!validate()) {
		return 0;
	} else if (!checkContext(WH_CMD_BASIC, WH_QLF_REGISTER)) {
		return 0;
	} else if (header().getLength() != HEADER_SIZE) {
		return 0;
	} else {
		return header().getLength();
	}
}

bool Protocol::registerRequest(uint64_t host, uint64_t uid, Digest *hc) {
	/*
	 * HEADER: SRC=<REQUESTED ID>, DEST=IGN, ....CMD=1, QLF=0, AQLF=0/1/127
	 * BODY: 64-byte CHALLANGE CODE in Request (optional); nothing in Response
	 * TOTAL: 32+64=96 bytes in Request; 32 bytes in Response
	 */
	return createRegisterRequest(host, uid, hc) && executeRequest(true, false)
			&& processRegisterResponse();
}

unsigned int Protocol::createGetKeyRequest(uint64_t host, Digest *hc,
		bool verify) noexcept {
	return createGetKeyRequest(host, nextSequenceNumber(),
			{ verify ? getKeyPair() : nullptr, hc }, *this);
}

unsigned int Protocol::processGetKeyResponse(Digest *hc) const noexcept {
	return processGetKeyResponse(*this, hc);
}

bool Protocol::getKeyRequest(uint64_t host, Digest *hc, bool verify) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=1, QLF=1, AQLF=0/1/127
	 * BODY: 512/8=64 Bytes in Request (optional), (512/8)*2=128 Bytes in Response
	 * TOTAL: 32+64=96 bytes in Request; 32+128=160 bytes in Response
	 */
	return createGetKeyRequest(host, hc, verify)
			&& executeRequest(false, verify) && processGetKeyResponse(hc);
}

unsigned int Protocol::createFindRootRequest(uint64_t host,
		uint64_t uid) noexcept {
	return createFindRootRequest(host, getSource(), uid, nextSequenceNumber(),
			*this);
}

unsigned int Protocol::processFindRootResponse(uint64_t uid,
		uint64_t &root) const noexcept {
	return processFindRootResponse(*this, uid, root);
}

bool Protocol::findRootRequest(uint64_t host, uint64_t uid, uint64_t &root) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=1, QLF=2, AQLF=0/1/127
	 * BODY: 8 bytes as <id> in Request; 8 bytes as <id> and 8 bytes as
	 * <successor> in Response
	 * TOTAL: 32+8=40 bytes in Request; 32+8+8=48 bytes in Response
	 */
	return createFindRootRequest(host, uid) && executeRequest()
			&& processFindRootResponse(uid, root);
}

unsigned int Protocol::createBootstrapRequest(uint64_t host) noexcept {
	clear();
	header().load(getSource(), host, HEADER_SIZE, nextSequenceNumber(), 0,
			WH_CMD_BASIC, WH_QLF_BOOTSTRAP, WH_AQLF_REQUEST);
	packHeader();
	return header().getLength();
}

unsigned int Protocol::processBootstrapResponse(uint64_t keys[],
		uint32_t &limit) const noexcept {
	if (!validate()) {
		return 0;
	} else if (!keys || !limit) {
		return 0;
	} else if (!checkContext(WH_CMD_BASIC, WH_QLF_BOOTSTRAP)) {
		return 0;
	} else if (getPayloadLength() <= sizeof(uint32_t)) {
		return 0;
	} else {
		auto n = Serializer::unpacku32(payload());
		limit = Twiddler::min(n, limit);
		auto offset = sizeof(uint32_t);
		for (unsigned int i = 0; i < limit; i++) {
			keys[i] = Serializer::unpacku64(payload(offset));
			offset += sizeof(uint64_t);
		}
		return header().getLength();
	}
}

bool Protocol::bootstrapRequest(uint64_t host, uint64_t keys[],
		uint32_t &limit) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=1, QLF=3, AQLF=0/1/127
	 * BODY: 0 in Request; 4 bytes as count + 8*NODECACHE_SIZE bytes as IDs
	 * in Response
	 * TOTAL: 32 bytes in Request; 32+4+8*NODECACHE_SIZE bytes in Response
	 */
	return createBootstrapRequest(host) && executeRequest()
			&& processBootstrapResponse(keys, limit);
}

unsigned int Protocol::createPublishRequest(uint64_t host, uint8_t topic,
		const unsigned char *data, unsigned int dataLength) noexcept {
	if ((dataLength && !data) || dataLength > PAYLOAD_SIZE) {
		return 0;
	} else {
		clear();
		header().load(getSource(), host, HEADER_SIZE + dataLength,
				nextSequenceNumber(), topic, WH_CMD_MULTICAST, WH_QLF_PUBLISH,
				WH_AQLF_REQUEST);
		packHeader();
		if (data) {
			Serializer::packib(payload(), data, dataLength);
		}
		return header().getLength();
	}
}

bool Protocol::publishRequest(uint64_t host, uint8_t topic,
		const unsigned char *data, unsigned int dataLength) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=2, QLF=0, AQLF=0/1/127
	 * BODY: variable in Request; no Response
	 * TOTAL: at least 32 bytes in Request; no Response
	 */
	return createPublishRequest(host, topic, data, dataLength) && (send(), true);
}

unsigned int Protocol::createSubscribeRequest(uint64_t host,
		uint8_t topic) noexcept {
	clear();
	header().load(getSource(), host, HEADER_SIZE, nextSequenceNumber(), topic,
			WH_CMD_MULTICAST, WH_QLF_SUBSCRIBE, WH_AQLF_REQUEST);
	packHeader();
	return header().getLength();
}

unsigned int Protocol::processSubscribeResponse(uint8_t topic) const noexcept {
	if (!validate()) {
		return 0;
	} else if (!checkContext(WH_CMD_MULTICAST, WH_QLF_SUBSCRIBE)) {
		return 0;
	} else if (header().getLength() == HEADER_SIZE
			&& header().getSession() == topic) {
		return header().getLength();
	} else {
		return 0;
	}
}

bool Protocol::subscribeRequest(uint64_t host, uint8_t topic) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=2, QLF=1, AQLF=0/1/127
	 * BODY: 0 in Request; 0 in Response
	 * TOTAL: 32 bytes in Request; 32 bytes in Response
	 */
	return createSubscribeRequest(host, topic) && executeRequest()
			&& processSubscribeResponse(topic);
}

unsigned int Protocol::createUnsubscribeRequest(uint64_t host,
		uint8_t topic) noexcept {
	clear();
	header().load(getSource(), host, HEADER_SIZE, nextSequenceNumber(), topic,
			WH_CMD_MULTICAST, WH_QLF_UNSUBSCRIBE, WH_AQLF_REQUEST);
	packHeader();
	return header().getLength();
}

unsigned int Protocol::processUnsubscribeResponse(uint8_t topic) const noexcept {
	if (!validate()) {
		return 0;
	} else if (!checkContext(WH_CMD_MULTICAST, WH_QLF_UNSUBSCRIBE)) {
		return 0;
	} else if (header().getLength() == HEADER_SIZE
			&& header().getSession() == topic) {
		return header().getLength();
	} else {
		return 0;
	}
}

bool Protocol::unsubscribeRequest(uint64_t host, uint8_t topic) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=2, QLF=2, AQLF=0/1/127
	 * BODY: 0 in Request; 0 in Response
	 * TOTAL: 32 bytes in Request; 32 bytes in Response
	 */
	return createUnsubscribeRequest(host, topic) && executeRequest()
			&& processUnsubscribeResponse(topic);
}

//-----------------------------------------------------------------

Message* Protocol::createIdentificationRequest(uint64_t host, uint64_t uid,
		uint16_t sequenceNumber, const unsigned char *nonce,
		unsigned int nonceLength) noexcept {
	auto msg = Message::create();
	if (!msg) {
		return nullptr;
	} else if (!createIdentificationRequest(host, uid, sequenceNumber, nonce,
			nonceLength, *msg)) {
		Message::recycle(msg);
		return nullptr;
	} else {
		return msg;
	}
}

unsigned int Protocol::processIdentificationResponse(const Message *msg,
		unsigned int &saltLength, const unsigned char *&salt,
		unsigned int &nonceLength, const unsigned char *&nonce) noexcept {
	return msg
			&& processIdentificationResponse(*msg, saltLength, salt,
					nonceLength, nonce);
}

Message* Protocol::createAuthenticationRequest(uint64_t host,
		uint16_t sequenceNumber, const unsigned char *proof,
		unsigned int length) noexcept {
	auto msg = Message::create();
	if (!msg) {
		return nullptr;
	} else if (!createAuthenticationRequest(host, sequenceNumber, proof, length,
			*msg)) {
		Message::recycle(msg);
		return nullptr;
	} else {
		return msg;
	}
}

unsigned int Protocol::processAuthenticationResponse(const Message *msg,
		unsigned int &length, const unsigned char *&proof) noexcept {
	return msg && processAuthenticationResponse(*msg, length, proof);
}

Message* Protocol::createRegisterRequest(uint64_t host, uint64_t uid,
		const Digest *hc, Message *msg) noexcept {
	if (msg || (msg = Message::create())) {
		msg->clear();
		createRegisterRequest(host, uid, 0, hc, *msg);
	}
	return msg;
}

Message* Protocol::createGetKeyRequest(uint64_t host, const TransactionKey &tk,
		Message *msg) noexcept {
	if (msg || (msg = Message::create())) {
		msg->clear();
		createGetKeyRequest(host, 0, tk, *msg);
	}
	return msg;
}

unsigned int Protocol::processGetKeyResponse(const Message *msg,
		Digest *hc) noexcept {
	return msg && hc && processGetKeyResponse(*msg, hc);
}

Message* Protocol::createFindRootRequest(uint64_t host, uint64_t uid,
		uint64_t identity, uint16_t sequenceNumber) noexcept {
	auto msg = Message::create();
	if (!msg) {
		return nullptr;
	} else if (!createFindRootRequest(host, uid, identity, sequenceNumber,
			*msg)) {
		Message::recycle(msg);
		return nullptr;
	} else {
		return msg;
	}
}

unsigned int Protocol::processFindRootResponse(const Message *msg,
		uint64_t identity, uint64_t &root) noexcept {
	return msg && processFindRootResponse(*msg, identity, root);
}

//-----------------------------------------------------------------

unsigned int Protocol::createIdentificationRequest(uint64_t host, uint64_t uid,
		uint16_t sequenceNumber, const unsigned char *nonce,
		unsigned int nonceLength, Packet &packet) noexcept {
	if (!nonce || !nonceLength || nonceLength > PAYLOAD_SIZE) {
		return 0;
	} else {
		packet.clear();
		auto len = HEADER_SIZE + nonceLength;
		packet.header().load(uid, host, len, sequenceNumber, 0, WH_CMD_NULL,
				WH_QLF_IDENTIFY, WH_AQLF_REQUEST);
		packet.packHeader();
		Serializer::packib(packet.payload(), nonce, nonceLength);
		return len;
	}
}

unsigned int Protocol::processIdentificationResponse(const Packet &packet,
		unsigned int &saltLength, const unsigned char *&salt,
		unsigned int &nonceLength, const unsigned char *&nonce) noexcept {
	if (!packet.validate()) {
		return 0;
	} else if (!packet.checkContext(WH_CMD_NULL, WH_QLF_IDENTIFY,
			WH_AQLF_ACCEPTED)) {
		return 0;
	} else if (packet.getPayloadLength() <= 2 * sizeof(uint16_t)) {
		return 0;
	} else {
		saltLength = Serializer::unpacku16(packet.payload());
		nonceLength = Serializer::unpacku16(packet.payload(sizeof(uint16_t)));

		if (!saltLength || !nonceLength
				|| (saltLength + nonceLength + 2 * sizeof(uint16_t))
						> PAYLOAD_SIZE) {
			return 0;
		} else {
			salt = packet.payload(2 * sizeof(uint16_t));
			nonce = packet.payload(2 * sizeof(uint16_t) + saltLength);
			return packet.header().getLength();
		}
	}
}

unsigned int Protocol::createAuthenticationRequest(uint64_t host,
		uint16_t sequenceNumber, const unsigned char *proof,
		unsigned int length, Packet &packet) noexcept {
	if (!proof || !length || length > PAYLOAD_SIZE) {
		return 0;
	} else {
		packet.clear();
		auto len = HEADER_SIZE + length;
		packet.header().load(0, host, len, sequenceNumber, 0, WH_CMD_NULL,
				WH_QLF_AUTHENTICATE, WH_AQLF_REQUEST);
		packet.packHeader();
		Serializer::packib(packet.payload(), proof, length);
		return len;
	}
}

unsigned int Protocol::processAuthenticationResponse(const Packet &packet,
		unsigned int &length, const unsigned char *&proof) noexcept {
	if (!packet.validate()) {
		return 0;
	} else if (!packet.checkContext(WH_CMD_NULL, WH_QLF_AUTHENTICATE,
			WH_AQLF_ACCEPTED)) {
		return 0;
	} else if (!packet.getPayloadLength()) {
		return 0;
	} else {
		length = packet.getPayloadLength();
		proof = packet.payload();
		return packet.header().getLength();
	}
}

unsigned int Protocol::createRegisterRequest(uint64_t host, uint64_t uid,
		uint16_t sequenceNumber, const Digest *hc, Packet &packet) noexcept {
	packet.clear();
	auto length = HEADER_SIZE;
	if (hc) {
		//Append the challenge key
		Serializer::packib(packet.payload(), (unsigned char*) hc, Hash::SIZE);
		length += Hash::SIZE;
	}

	packet.header().load(uid, host, length, sequenceNumber, 0, WH_CMD_BASIC,
			WH_QLF_REGISTER, WH_AQLF_REQUEST);
	packet.packHeader();
	return packet.header().getLength();
}

unsigned int Protocol::createGetKeyRequest(uint64_t host,
		uint16_t sequenceNumber, const TransactionKey &tk,
		Packet &packet) noexcept {
	auto length = HEADER_SIZE;
	packet.clear();
	if (tk.nonce && tk.keys) {
		PKIEncryptedData challenge;
		memset(&challenge, 0, sizeof(challenge));
		//Ignore the encryption error (Public key is used for encryption)
		tk.keys->encrypt(tk.nonce, Hash::SIZE, &challenge);
		//Append the challenge at the start of the message
		Serializer::packib(packet.payload(), (unsigned char*) challenge,
				PKI::ENCRYPTED_LENGTH);
		length += PKI::ENCRYPTED_LENGTH;
	} else if (tk.nonce) {
		Serializer::packib(packet.payload(), (unsigned char*) tk.nonce,
				Hash::SIZE);
		length += Hash::SIZE;
	}

	packet.header().load(0, host, length, sequenceNumber, 0, WH_CMD_BASIC,
			WH_QLF_GETKEY, WH_AQLF_REQUEST);
	packet.packHeader();
	return packet.header().getLength();
}

unsigned int Protocol::processGetKeyResponse(const Packet &packet,
		Digest *hc) noexcept {
	if (!packet.validate()) {
		return 0;
	} else if (!packet.checkContext(WH_CMD_BASIC, WH_QLF_GETKEY,
			WH_AQLF_ACCEPTED)) {
		return 0;
	} else if (!hc || packet.getPayloadLength() < (2 * Hash::SIZE)) {
		return 0;
	} else if (!memcmp(packet.payload(), hc, Hash::SIZE)) {
		//Verify the response, if OK then store the key
		Serializer::unpackib((unsigned char*) hc, packet.payload(Hash::SIZE),
				Hash::SIZE);
		return packet.header().getLength();
	} else {
		return 0;
	}
}

unsigned int Protocol::createFindRootRequest(uint64_t host, uint64_t uid,
		uint64_t identity, uint16_t sequenceNumber, Packet &packet) noexcept {
	packet.clear();
	auto len = HEADER_SIZE + sizeof(uint64_t);
	packet.header().load(uid, host, len, sequenceNumber, 0, WH_CMD_BASIC,
			WH_QLF_FINDROOT, WH_AQLF_REQUEST);
	packet.packHeader();
	Serializer::pack(packet.payload(), "Q", identity);
	return len;
}

unsigned int Protocol::processFindRootResponse(const Packet &packet,
		uint64_t identity, uint64_t &root) noexcept {
	if (!packet.validate()) {
		return 0;
	} else if (!packet.checkContext(WH_CMD_BASIC, WH_QLF_FINDROOT,
			WH_AQLF_ACCEPTED)) {
		return 0;
	} else if (packet.getPayloadLength() != (2 * sizeof(uint64_t))) {
		return 0;
	} else {
		uint64_t v[2] = { 0, 0 };
		Serializer::unpack(packet.payload(), "QQ", &v[0], &v[1]);
		if (v[0] == identity) {
			root = v[1];
			return packet.header().getLength();
		} else {
			root = 0;
			return 0;
		}
	}
}

} /* namespace wanhive */

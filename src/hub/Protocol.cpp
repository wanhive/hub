/*
 * Protocol.cpp
 *
 * Bare minimum protocol implementation
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

unsigned int Protocol::createIdentificationRequest(
		const MessageAddress &address, const Data &nonce) noexcept {
	return createIdentificationRequest(address, nextSequenceNumber(), nonce,
			*this);
}

unsigned int Protocol::processIdentificationResponse(Data &salt,
		Data &nonce) const noexcept {
	return processIdentificationResponse(*this, salt, nonce);
}

bool Protocol::identificationRequest(const MessageAddress &address,
		const Data &nonce) {
	/*
	 * HEADER: SRC=<identity>, DEST=X, ....CMD=0, QLF=1, AQLF=0/1/127
	 * BODY: variable in Request and Response
	 * TOTAL: at least 32 bytes in Request and Response
	 */
	return createIdentificationRequest(address, nonce) && executeRequest();
}

unsigned int Protocol::createAuthenticationRequest(
		const MessageAddress &address, const Data &proof) noexcept {
	return createAuthenticationRequest(address, nextSequenceNumber(), proof,
			*this);
}

unsigned int Protocol::processAuthenticationResponse(Data &proof) const noexcept {
	return processAuthenticationResponse(*this, proof);
}

bool Protocol::authenticationRequest(const MessageAddress &address,
		const Data &proof) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=0, QLF=2, AQLF=0/1/127
	 * BODY: variable in Request and Response
	 * TOTAL: at least 32 bytes in Request and Response
	 */
	return createAuthenticationRequest(address, proof) && executeRequest();
}

unsigned int Protocol::createRegisterRequest(const MessageAddress &address,
		Digest *hc) noexcept {
	return createRegisterRequest(address, nextSequenceNumber(), hc, *this);
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

bool Protocol::registerRequest(const MessageAddress &address, Digest *hc) {
	/*
	 * HEADER: SRC=<REQUESTED ID>, DEST=IGN, ....CMD=1, QLF=0, AQLF=0/1/127
	 * BODY: 64-byte CHALLANGE CODE in Request (optional); nothing in Response
	 * TOTAL: 32+64=96 bytes in Request; 32 bytes in Response
	 */
	return createRegisterRequest(address, hc) && executeRequest(true, false)
			&& processRegisterResponse();
}

unsigned int Protocol::createGetKeyRequest(const MessageAddress &address,
		Digest *hc, bool verify) noexcept {
	return createGetKeyRequest(address, nextSequenceNumber(),
			{ verify ? getKeyPair() : nullptr, hc }, *this);
}

unsigned int Protocol::processGetKeyResponse(Digest *hc) const noexcept {
	return processGetKeyResponse(*this, hc);
}

bool Protocol::getKeyRequest(const MessageAddress &address, Digest *hc,
		bool verify) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=1, QLF=1, AQLF=0/1/127
	 * BODY: 512/8=64 Bytes in Request (optional), (512/8)*2=128 Bytes in Response
	 * TOTAL: 32+64=96 bytes in Request; 32+128=160 bytes in Response
	 */
	return createGetKeyRequest(address, hc, verify)
			&& executeRequest(false, verify) && processGetKeyResponse(hc);
}

unsigned int Protocol::createFindRootRequest(uint64_t host,
		uint64_t identity) noexcept {
	return createFindRootRequest( { getSource(), host }, identity,
			nextSequenceNumber(), *this);
}

unsigned int Protocol::processFindRootResponse(uint64_t identity,
		uint64_t &root) const noexcept {
	return processFindRootResponse(*this, identity, root);
}

bool Protocol::findRootRequest(uint64_t host, uint64_t identity,
		uint64_t &root) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=1, QLF=2, AQLF=0/1/127
	 * BODY: 8 bytes as <id> in Request; 8 bytes as <id> and 8 bytes as
	 * <successor> in Response
	 * TOTAL: 32+8=40 bytes in Request; 32+8+8=48 bytes in Response
	 */
	return createFindRootRequest(host, identity) && executeRequest()
			&& processFindRootResponse(identity, root);
}

unsigned int Protocol::createBootstrapRequest(uint64_t host) noexcept {
	clear();
	header().setAddress(getSource(), host);
	header().setControl(HEADER_SIZE, nextSequenceNumber(), 0);
	header().setContext(WH_CMD_BASIC, WH_QLF_BOOTSTRAP, WH_AQLF_REQUEST);
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
		const Data &data) noexcept {
	if ((data.length && !data.base) || data.length > PAYLOAD_SIZE) {
		return 0;
	} else {
		clear();
		header().setAddress(getSource(), host);
		header().setControl(HEADER_SIZE + data.length, nextSequenceNumber(),
				topic);
		header().setContext(WH_CMD_MULTICAST, WH_QLF_PUBLISH, WH_AQLF_REQUEST);
		packHeader();
		if (data.base) {
			Serializer::packib(payload(), data.base, data.length);
		}
		return header().getLength();
	}
}

bool Protocol::publishRequest(uint64_t host, uint8_t topic, const Data &data) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=2, QLF=0, AQLF=0/1/127
	 * BODY: variable in Request; no Response
	 * TOTAL: at least 32 bytes in Request; no Response
	 */
	return createPublishRequest(host, topic, data) && (send(), true);
}

unsigned int Protocol::createSubscribeRequest(uint64_t host,
		uint8_t topic) noexcept {
	clear();
	header().setAddress(getSource(), host);
	header().setControl(HEADER_SIZE, nextSequenceNumber(), topic);
	header().setContext(WH_CMD_MULTICAST, WH_QLF_SUBSCRIBE, WH_AQLF_REQUEST);
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
	header().setAddress(getSource(), host);
	header().setControl(HEADER_SIZE, nextSequenceNumber(), topic);
	header().setContext(WH_CMD_MULTICAST, WH_QLF_UNSUBSCRIBE, WH_AQLF_REQUEST);
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
Message* Protocol::createIdentificationRequest(const MessageAddress &address,
		const Data &nonce, uint16_t sequenceNumber) noexcept {
	auto msg = Message::create();
	if (!msg) {
		return nullptr;
	} else if (!createIdentificationRequest(address, sequenceNumber, nonce,
			*msg)) {
		Message::recycle(msg);
		return nullptr;
	} else {
		return msg;
	}
}

unsigned int Protocol::processIdentificationResponse(const Message *msg,
		Data &salt, Data &nonce) {
	return msg && processIdentificationResponse(*msg, salt, nonce);
}

Message* Protocol::createAuthenticationRequest(const MessageAddress &address,
		const Data &proof, uint16_t sequenceNumber) noexcept {
	auto msg = Message::create();
	if (!msg) {
		return nullptr;
	} else if (!createAuthenticationRequest(address, sequenceNumber, proof,
			*msg)) {
		Message::recycle(msg);
		return nullptr;
	} else {
		return msg;
	}
}

unsigned int Protocol::processAuthenticationResponse(const Message *msg,
		Data &proof) noexcept {
	return msg && processAuthenticationResponse(*msg, proof);
}

Message* Protocol::createRegisterRequest(const MessageAddress &address,
		const Digest *hc, Message *msg) noexcept {
	if (msg || (msg = Message::create())) {
		createRegisterRequest(address, 0, hc, *msg);
	}
	return msg;
}

Message* Protocol::createGetKeyRequest(const MessageAddress &address,
		const TransactionKey &tk, Message *msg) noexcept {
	if (msg || (msg = Message::create())) {
		createGetKeyRequest(address, 0, tk, *msg);
	}
	return msg;
}

unsigned int Protocol::processGetKeyResponse(const Message *msg,
		Digest *hc) noexcept {
	return msg && hc && processGetKeyResponse(*msg, hc);
}

Message* Protocol::createFindRootRequest(const MessageAddress &address,
		uint64_t identity, uint16_t sequenceNumber) noexcept {
	auto msg = Message::create();
	if (!msg) {
		return nullptr;
	} else if (!createFindRootRequest(address, identity, sequenceNumber,
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

unsigned int Protocol::createIdentificationRequest(
		const MessageAddress &address, uint16_t sequenceNumber,
		const Data &nonce, Packet &packet) noexcept {
	if (!nonce.base || !nonce.length || nonce.length > PAYLOAD_SIZE) {
		return 0;
	} else {
		packet.clear();
		auto len = HEADER_SIZE + nonce.length;
		packet.header().setAddress(address.getSource(),
				address.getDestination());
		packet.header().setControl(len, sequenceNumber, 0);
		packet.header().setContext(WH_CMD_NULL, WH_QLF_IDENTIFY,
				WH_AQLF_REQUEST);
		packet.packHeader();
		Serializer::packib(packet.payload(), nonce.base, nonce.length);
		return len;
	}
}

unsigned int Protocol::processIdentificationResponse(const Packet &packet,
		Data &salt, Data &nonce) noexcept {
	if (!packet.validate()) {
		return 0;
	} else if (!packet.checkContext(WH_CMD_NULL, WH_QLF_IDENTIFY,
			WH_AQLF_ACCEPTED)) {
		return 0;
	} else if (packet.getPayloadLength() <= 2 * sizeof(uint16_t)) {
		return 0;
	} else {
		salt.length = Serializer::unpacku16(packet.payload());
		nonce.length = Serializer::unpacku16(packet.payload(sizeof(uint16_t)));

		if (!salt.length || !nonce.length
				|| (salt.length + nonce.length + 2 * sizeof(uint16_t))
						> PAYLOAD_SIZE) {
			return 0;
		} else {
			salt.base = packet.payload(2 * sizeof(uint16_t));
			nonce.base = packet.payload(2 * sizeof(uint16_t) + salt.length);
			return packet.header().getLength();
		}
	}
}

unsigned int Protocol::createAuthenticationRequest(
		const MessageAddress &address, uint16_t sequenceNumber,
		const Data &proof, Packet &packet) noexcept {
	if (!proof.base || !proof.length || proof.length > PAYLOAD_SIZE) {
		return 0;
	} else {
		packet.clear();
		auto len = HEADER_SIZE + proof.length;
		packet.header().setAddress(address.getSource(),
				address.getDestination());
		packet.header().setControl(len, sequenceNumber, 0);
		packet.header().setContext(WH_CMD_NULL, WH_QLF_AUTHENTICATE,
				WH_AQLF_REQUEST);
		packet.packHeader();
		Serializer::packib(packet.payload(), proof.base, proof.length);
		return len;
	}
}

unsigned int Protocol::processAuthenticationResponse(const Packet &packet,
		Data &proof) noexcept {
	if (!packet.validate()) {
		return 0;
	} else if (!packet.checkContext(WH_CMD_NULL, WH_QLF_AUTHENTICATE,
			WH_AQLF_ACCEPTED)) {
		return 0;
	} else if (!packet.getPayloadLength()) {
		return 0;
	} else {
		proof.length = packet.getPayloadLength();
		proof.base = packet.payload();
		return packet.header().getLength();
	}
}

unsigned int Protocol::createRegisterRequest(const MessageAddress &address,
		uint16_t sequenceNumber, const Digest *hc, Packet &packet) noexcept {
	packet.clear();
	auto length = HEADER_SIZE;
	if (hc) {
		//Append the challenge key
		Serializer::packib(packet.payload(), (unsigned char*) hc, Hash::SIZE);
		length += Hash::SIZE;
	}

	packet.header().setAddress(address.getSource(), address.getDestination());
	packet.header().setControl(length, sequenceNumber, 0);
	packet.header().setContext(WH_CMD_BASIC, WH_QLF_REGISTER, WH_AQLF_REQUEST);
	packet.packHeader();
	return packet.header().getLength();
}

unsigned int Protocol::createGetKeyRequest(const MessageAddress &address,
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

	packet.header().setAddress(address.getSource(), address.getDestination());
	packet.header().setControl(length, sequenceNumber, 0);
	packet.header().setContext(WH_CMD_BASIC, WH_QLF_GETKEY, WH_AQLF_REQUEST);
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

unsigned int Protocol::createFindRootRequest(const MessageAddress &address,
		uint64_t identity, uint16_t sequenceNumber, Packet &packet) noexcept {
	packet.clear();
	auto len = HEADER_SIZE + sizeof(uint64_t);
	packet.header().setAddress(address.getSource(), address.getDestination());
	packet.header().setControl(len, sequenceNumber, 0);
	packet.header().setContext(WH_CMD_BASIC, WH_QLF_FINDROOT, WH_AQLF_REQUEST);
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

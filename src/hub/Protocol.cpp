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
			nonceLength, header(), buffer());
}

unsigned int Protocol::processIdentificationResponse(unsigned int &saltLength,
		const unsigned char *&salt, unsigned int &nonceLength,
		const unsigned char *&nonce) const noexcept {
	return processIdentificationResponse(header(), buffer(), saltLength, salt,
			nonceLength, nonce);
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
			length, header(), buffer());
}

unsigned int Protocol::processAuthenticationResponse(unsigned int &length,
		const unsigned char *&proof) const noexcept {
	return processAuthenticationResponse(header(), buffer(), length, proof);
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
	return createRegisterRequest(host, uid, nextSequenceNumber(), hc, header(),
			buffer());
}

unsigned int Protocol::processRegisterResponse() const noexcept {
	if (!checkContext(WH_CMD_BASIC, WH_QLF_REGISTER)) {
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
			{ verify ? getKeyPair() : nullptr, hc }, header(), buffer());
}

unsigned int Protocol::processGetKeyResponse(Digest *hc) const noexcept {
	return processGetKeyResponse(header(), buffer(), hc);
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
			header(), buffer());
}

unsigned int Protocol::processFindRootResponse(uint64_t uid,
		uint64_t &root) const noexcept {
	return processFindRootResponse(header(), buffer(), uid, root);
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
	header().load(getSource(), host, HEADER_SIZE, nextSequenceNumber(), 0,
			WH_CMD_BASIC, WH_QLF_BOOTSTRAP, WH_AQLF_REQUEST);
	header().serialize(buffer());
	return header().getLength();
}

unsigned int Protocol::processBootstrapResponse(uint64_t keys[],
		uint32_t &limit) const noexcept {
	if (!keys || !limit) {
		return 0;
	} else if (!checkContext(WH_CMD_BASIC, WH_QLF_BOOTSTRAP)) {
		return 0;
	} else if (header().getLength() <= HEADER_SIZE + sizeof(uint32_t)) {
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
		const unsigned char *payload, unsigned int payloadLength) noexcept {
	if ((payloadLength && !payload) || payloadLength > PAYLOAD_SIZE) {
		return 0;
	} else {
		header().load(getSource(), host, HEADER_SIZE + payloadLength,
				nextSequenceNumber(), topic, WH_CMD_MULTICAST, WH_QLF_PUBLISH,
				WH_AQLF_REQUEST);
		auto len = header().serialize(buffer());
		if (payload) {
			Serializer::packib(buffer(len), payload, payloadLength);
		}
		return header().getLength();
	}
}

bool Protocol::publishRequest(uint64_t host, uint8_t topic,
		const unsigned char *payload, unsigned int payloadLength) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=2, QLF=0, AQLF=0/1/127
	 * BODY: variable in Request; no Response
	 * TOTAL: at least 32 bytes in Request; no Response
	 */
	return createPublishRequest(host, topic, payload, payloadLength)
			&& (send(), true);
}

unsigned int Protocol::createSubscribeRequest(uint64_t host,
		uint8_t topic) noexcept {
	header().load(getSource(), host, HEADER_SIZE, nextSequenceNumber(), topic,
			WH_CMD_MULTICAST, WH_QLF_SUBSCRIBE, WH_AQLF_REQUEST);
	header().serialize(buffer());
	return header().getLength();
}

unsigned int Protocol::processSubscribeResponse(uint8_t topic) const noexcept {
	if (!checkContext(WH_CMD_MULTICAST, WH_QLF_SUBSCRIBE)) {
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
	header().load(getSource(), host, HEADER_SIZE, nextSequenceNumber(), topic,
			WH_CMD_MULTICAST, WH_QLF_UNSUBSCRIBE, WH_AQLF_REQUEST);
	header().serialize(buffer());
	return header().getLength();
}

unsigned int Protocol::processUnsubscribeResponse(uint8_t topic) const noexcept {
	if (!checkContext(WH_CMD_MULTICAST, WH_QLF_UNSUBSCRIBE)) {
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
			nonceLength, msg->header(), msg->buffer())) {
		Message::recycle(msg);
		return nullptr;
	} else {
		msg->updateLength(msg->header().getLength());
		return msg;
	}
}

unsigned int Protocol::processIdentificationResponse(const Message *msg,
		unsigned int &saltLength, const unsigned char *&salt,
		unsigned int &nonceLength, const unsigned char *&nonce) noexcept {
	if (!msg || !msg->validate()) {
		return 0;
	} else {
		return processIdentificationResponse(msg->header(), msg->buffer(),
				saltLength, salt, nonceLength, nonce);
	}
}

Message* Protocol::createAuthenticationRequest(uint64_t host,
		uint16_t sequenceNumber, const unsigned char *proof,
		unsigned int length) noexcept {
	auto msg = Message::create();
	if (!msg) {
		return nullptr;
	} else if (!createAuthenticationRequest(host, sequenceNumber, proof, length,
			msg->header(), msg->buffer())) {
		Message::recycle(msg);
		return nullptr;
	} else {
		msg->updateLength(msg->header().getLength());
		return msg;
	}
}

unsigned int Protocol::processAuthenticationResponse(const Message *msg,
		unsigned int &length, const unsigned char *&proof) noexcept {
	if (!msg || !msg->validate()) {
		return 0;
	} else {
		return processAuthenticationResponse(msg->header(), msg->buffer(),
				length, proof);
	}
}

Message* Protocol::createRegisterRequest(uint64_t host, uint64_t uid,
		const Digest *hc, Message *msg) noexcept {
	if (msg == nullptr) {
		msg = Message::create();
	} else {
		msg->clear();
	}

	if (msg) {
		MessageHeader &header = msg->header();
		createRegisterRequest(host, uid, 0, hc, header, msg->buffer());
		msg->updateLength(header.getLength());
	}
	return msg;
}

Message* Protocol::createGetKeyRequest(uint64_t host, const TransactionKey &tk,
		Message *msg) noexcept {
	if (msg == nullptr) {
		msg = Message::create();
	} else {
		msg->clear();
	}

	if (msg) {
		MessageHeader &header = msg->header();
		createGetKeyRequest(host, 0, tk, header, msg->buffer());
		msg->updateLength(header.getLength());
	}
	return msg;
}

unsigned int Protocol::processGetKeyResponse(const Message *msg,
		Digest *hc) noexcept {
	if (!msg || !msg->validate() || !hc) {
		return 0;
	} else {
		return processGetKeyResponse(msg->header(), msg->buffer(), hc);
	}
}

Message* Protocol::createFindRootRequest(uint64_t host, uint64_t uid,
		uint64_t identity, uint16_t sequenceNumber) noexcept {
	auto msg = Message::create();
	if (!msg) {
		return nullptr;
	} else if (!createFindRootRequest(host, uid, identity, sequenceNumber,
			msg->header(), msg->buffer())) {
		Message::recycle(msg);
		return nullptr;
	} else {
		msg->updateLength(msg->header().getLength());
		return msg;
	}
}

unsigned int Protocol::processFindRootResponse(const Message *msg,
		uint64_t identity, uint64_t &root) noexcept {
	if (!msg || !msg->validate()) {
		return 0;
	} else {
		return processFindRootResponse(msg->header(), msg->buffer(), identity,
				root);
	}
}

//-----------------------------------------------------------------

unsigned int Protocol::createIdentificationRequest(uint64_t host, uint64_t uid,
		uint16_t sequenceNumber, const unsigned char *nonce,
		unsigned int nonceLength, MessageHeader &header,
		unsigned char *buf) noexcept {
	if (!buf) {
		return 0;
	} else if (!nonce || !nonceLength || nonceLength > PAYLOAD_SIZE) {
		return 0;
	} else {
		auto len = HEADER_SIZE + nonceLength;
		header.load(uid, host, len, sequenceNumber, 0, WH_CMD_NULL,
				WH_QLF_IDENTIFY, WH_AQLF_REQUEST);
		header.serialize(buf);
		Serializer::packib((buf + HEADER_SIZE), nonce, nonceLength);
		return len;
	}
}

unsigned int Protocol::processIdentificationResponse(
		const MessageHeader &header, const unsigned char *buf,
		unsigned int &saltLength, const unsigned char *&salt,
		unsigned int &nonceLength, const unsigned char *&nonce) noexcept {
	if (!(header.getCommand() == WH_CMD_NULL
			&& header.getQualifier() == WH_QLF_IDENTIFY
			&& header.getStatus() == WH_AQLF_ACCEPTED)) {
		return 0;
	} else if (!buf
			|| header.getLength() <= HEADER_SIZE + 2 * sizeof(uint16_t)) {
		return 0;
	} else {
		saltLength = Serializer::unpacku16(buf + HEADER_SIZE);
		nonceLength = Serializer::unpacku16(
				buf + HEADER_SIZE + sizeof(uint16_t));

		if (!saltLength || !nonceLength
				|| (saltLength + nonceLength + 2 * sizeof(uint16_t))
						> PAYLOAD_SIZE) {
			return 0;
		} else {
			salt = buf + HEADER_SIZE + (2 * sizeof(uint16_t));
			nonce = buf + HEADER_SIZE + (2 * sizeof(uint16_t) + saltLength);
			return header.getLength();
		}
	}
}

unsigned int Protocol::createAuthenticationRequest(uint64_t host,
		uint16_t sequenceNumber, const unsigned char *proof,
		unsigned int length, MessageHeader &header, unsigned char *buf) noexcept {
	if (!buf) {
		return 0;
	} else if (!proof || !length || length > PAYLOAD_SIZE) {
		return 0;
	} else {
		auto len = HEADER_SIZE + length;
		header.load(0, host, len, sequenceNumber, 0, WH_CMD_NULL,
				WH_QLF_AUTHENTICATE, WH_AQLF_REQUEST);
		header.serialize(buf);
		Serializer::packib((buf + HEADER_SIZE), proof, length);
		return len;
	}
}

unsigned int Protocol::processAuthenticationResponse(
		const MessageHeader &header, const unsigned char *buf,
		unsigned int &length, const unsigned char *&proof) noexcept {
	if (!(header.getCommand() == WH_CMD_NULL
			&& header.getQualifier() == WH_QLF_AUTHENTICATE
			&& header.getStatus() == WH_AQLF_ACCEPTED)) {
		return 0;
	} else if (!buf || header.getLength() <= HEADER_SIZE) {
		return 0;
	} else {
		length = header.getLength() - HEADER_SIZE;
		proof = buf + HEADER_SIZE;
		return header.getLength();
	}
}

unsigned int Protocol::createRegisterRequest(uint64_t host, uint64_t uid,
		uint16_t sequenceNumber, const Digest *hc, MessageHeader &header,
		unsigned char *buf) noexcept {
	auto length = HEADER_SIZE;
	if (!buf) {
		return 0;
	} else if (hc) {
		//Append the challenge key
		Serializer::packib((buf + HEADER_SIZE), (unsigned char*) hc,
				Hash::SIZE);
		length += Hash::SIZE;
	}

	header.load(uid, host, length, sequenceNumber, 0, WH_CMD_BASIC,
			WH_QLF_REGISTER, WH_AQLF_REQUEST);
	header.serialize(buf);
	return length;
}

unsigned int Protocol::createGetKeyRequest(uint64_t host,
		uint16_t sequenceNumber, const TransactionKey &tk,
		MessageHeader &header, unsigned char *buf) noexcept {
	auto length = HEADER_SIZE;
	if (!buf) {
		return 0;
	} else if (tk.nonce && tk.keys) {
		PKIEncryptedData challenge;
		memset(&challenge, 0, sizeof(challenge));
		//Ignore the encryption error (Public key is used for encryption)
		tk.keys->encrypt(tk.nonce, Hash::SIZE, &challenge);
		//Append the challenge at the start of the message
		Serializer::packib((buf + HEADER_SIZE), (unsigned char*) challenge,
				PKI::ENCRYPTED_LENGTH);
		length += PKI::ENCRYPTED_LENGTH;
	} else if (tk.nonce) {
		Serializer::packib((buf + HEADER_SIZE), (unsigned char*) tk.nonce,
				Hash::SIZE);
		length += Hash::SIZE;
	}

	header.load(0, host, length, sequenceNumber, 0, WH_CMD_BASIC, WH_QLF_GETKEY,
			WH_AQLF_REQUEST);
	header.serialize(buf);
	return length;
}

unsigned int Protocol::processGetKeyResponse(const MessageHeader &header,
		const unsigned char *buf, Digest *hc) noexcept {
	if (!(header.getCommand() == WH_CMD_BASIC
			&& header.getQualifier() == WH_QLF_GETKEY
			&& header.getStatus() == WH_AQLF_ACCEPTED)) {
		return 0;
	} else if (!buf || !hc
			|| header.getLength() < (HEADER_SIZE + 2 * Hash::SIZE)) {
		return 0;
	} else if (!memcmp(buf + HEADER_SIZE, hc, Hash::SIZE)) {
		//Verify the response, if OK then store the key
		Serializer::unpackib((unsigned char*) hc,
				buf + HEADER_SIZE + Hash::SIZE, Hash::SIZE);
		return header.getLength();
	} else {
		return 0;
	}
}

unsigned int Protocol::createFindRootRequest(uint64_t host, uint64_t uid,
		uint64_t identity, uint16_t sequenceNumber, MessageHeader &header,
		unsigned char *buf) noexcept {
	if (!buf) {
		return 0;
	} else {
		auto len = HEADER_SIZE + sizeof(uint64_t);
		header.load(uid, host, len, sequenceNumber, 0, WH_CMD_BASIC,
				WH_QLF_FINDROOT, WH_AQLF_REQUEST);
		header.serialize(buf);
		Serializer::pack(buf + HEADER_SIZE, "Q", identity);
		return len;
	}
}

unsigned int Protocol::processFindRootResponse(const MessageHeader &header,
		const unsigned char *buf, uint64_t identity, uint64_t &root) noexcept {
	if (!(header.getCommand() == WH_CMD_BASIC
			&& header.getQualifier() == WH_QLF_FINDROOT
			&& header.getStatus() == WH_AQLF_ACCEPTED)) {
		return 0;
	} else if (!buf
			|| header.getLength() != HEADER_SIZE + 2 * sizeof(uint64_t)) {
		return 0;
	} else {
		uint64_t v[2] = { 0, 0 };
		Serializer::unpack(buf + HEADER_SIZE, "QQ", &v[0], &v[1]);
		if (v[0] == identity) {
			root = v[1];
			return header.getLength();
		} else {
			root = 0;
			return 0;
		}
	}
}

} /* namespace wanhive */

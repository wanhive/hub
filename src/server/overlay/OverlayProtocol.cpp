/*
 * OverlayProtocol.cpp
 *
 * Protocol extension for the overlay network clients
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "OverlayProtocol.h"
#include "commands.h"
#include "../../base/ds/Serializer.h"
#include "../../base/ds/Twiddler.h"

namespace wanhive {

OverlayProtocol::OverlayProtocol() noexcept {

}

OverlayProtocol::~OverlayProtocol() {

}

unsigned int OverlayProtocol::createDescribeRequest(uint64_t host) noexcept {
	Packet::clear();
	header().setAddress(getSource(), host);
	header().setControl(HEADER_SIZE, nextSequenceNumber(), getSession());
	header().setContext(WH_DHT_CMD_NULL, WH_DHT_QLF_DESCRIBE,
			WH_DHT_AQLF_REQUEST);
	packHeader();
	return header().getLength();
}

unsigned int OverlayProtocol::processDescribeResponse(
		OverlayHubInfo &info) const noexcept {
	if (!checkContext(WH_DHT_CMD_NULL, WH_DHT_QLF_DESCRIBE)) {
		return 0;
	} else if (!info.unpack(payload(), getPayloadLength())) {
		return 0;
	} else {
		return header().getLength();;
	}
}

bool OverlayProtocol::describeRequest(uint64_t host, OverlayHubInfo &info) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=0, QLF=127, AQLF=0/1/127
	 * BODY: 0 bytes in Request; 84+25*Node::TABLESIZE bytes in Response
	 * TOTAL: 32 bytes in Request; 116+25*Node::TABLESIZE bytes in Response
	 */
	return createDescribeRequest(host) && executeRequest()
			&& processDescribeResponse(info);
}

unsigned int OverlayProtocol::createGetPredecessorRequest(
		uint64_t host) noexcept {
	Packet::clear();
	header().setAddress(getSource(), host);
	header().setControl(HEADER_SIZE, nextSequenceNumber(), getSession());
	header().setContext(WH_DHT_CMD_NODE, WH_DHT_QLF_GETPREDECESSOR,
			WH_DHT_AQLF_REQUEST);
	packHeader();
	return header().getLength();
}

unsigned int OverlayProtocol::processGetPredecessorResponse(
		uint64_t &key) const noexcept {
	if (!checkContext(WH_DHT_CMD_NODE, WH_DHT_QLF_GETPREDECESSOR)) {
		return 0;
	} else if (getPayloadLength() != sizeof(uint64_t)) {
		return 0;
	} else {
		uint64_t v;
		Serializer::unpack(payload(), (char*) "Q", &v);
		key = v;
		return header().getLength();
	}
}

bool OverlayProtocol::getPredecessorRequest(uint64_t host, uint64_t &key) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=0, AQLF=0/1/127
	 * BODY: 0 bytes in Request; 8 bytes as <predecessor> in Response
	 * TOTAL: 32 bytes in Request; 32+8=40 bytes in Response
	 */
	return createGetPredecessorRequest(host) && executeRequest()
			&& processGetPredecessorResponse(key);
}

unsigned int OverlayProtocol::createSetPredecessorRequest(uint64_t host,
		uint64_t key) noexcept {
	Packet::clear();
	header().setAddress(getSource(), host);
	header().setControl((HEADER_SIZE + sizeof(uint64_t)), nextSequenceNumber(),
			getSession());
	header().setContext(WH_DHT_CMD_NODE, WH_DHT_QLF_SETPREDECESSOR,
			WH_DHT_AQLF_REQUEST);
	packHeader();
	Serializer::pack(payload(), "Q", key);
	return header().getLength();
}

unsigned int OverlayProtocol::processSetPredecessorResponse(
		uint64_t key) const noexcept {
	if (!checkContext(WH_DHT_CMD_NODE, WH_DHT_QLF_SETPREDECESSOR)) {
		return 0;
	} else if (getPayloadLength() != sizeof(uint64_t)) {
		return 0;
	} else {
		uint64_t v;
		Serializer::unpack(payload(), (char*) "Q", &v);
		if (v == key) {
			return header().getLength();
		} else {
			return 0;
		}
	}
}

bool OverlayProtocol::setPredecessorRequest(uint64_t host, uint64_t key) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=1, AQLF=0/1/127
	 * BODY: 8 bytes as <predecessor> in Request; 8 bytes as <predecessor> in Response
	 * TOTAL: 32+8=40 bytes
	 */
	return createSetPredecessorRequest(host, key) && executeRequest()
			&& processSetPredecessorResponse(key);
}

unsigned int OverlayProtocol::createGetSuccessorRequest(uint64_t host) noexcept {
	Packet::clear();
	header().setAddress(getSource(), host);
	header().setControl(HEADER_SIZE, nextSequenceNumber(), getSession());
	header().setContext(WH_DHT_CMD_NODE, WH_DHT_QLF_GETSUCCESSOR,
			WH_DHT_AQLF_REQUEST);
	packHeader();
	return header().getLength();
}

unsigned int OverlayProtocol::processGetSuccessorResponse(
		uint64_t &key) const noexcept {
	if (!checkContext(WH_DHT_CMD_NODE, WH_DHT_QLF_GETSUCCESSOR)) {
		return 0;
	} else if (getPayloadLength() != sizeof(uint64_t)) {
		return 0;
	} else {
		uint64_t v;
		Serializer::unpack(payload(), (char*) "Q", &v);
		key = v;
		return header().getLength();
	}
}

bool OverlayProtocol::getSuccessorRequest(uint64_t host, uint64_t &key) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=2, AQLF=0/1/127
	 * BODY: 0 bytes in Request; 8 bytes as <successor> in Response
	 * TOTAL: 32 bytes in Request; 32+8=40 bytes in Response
	 */
	return createGetSuccessorRequest(host) && executeRequest()
			&& processGetSuccessorResponse(key);
}

unsigned int OverlayProtocol::createSetSuccessorRequest(uint64_t host,
		uint64_t key) noexcept {
	Packet::clear();
	header().setAddress(getSource(), host);
	header().setControl((HEADER_SIZE + sizeof(uint64_t)), nextSequenceNumber(),
			getSession());
	header().setContext(WH_DHT_CMD_NODE, WH_DHT_QLF_SETSUCCESSOR,
			WH_DHT_AQLF_REQUEST);
	packHeader();
	Serializer::pack(payload(), "Q", key);
	return header().getLength();
}

unsigned int OverlayProtocol::processSetSuccessorResponse(
		uint64_t key) const noexcept {
	if (!checkContext(WH_DHT_CMD_NODE, WH_DHT_QLF_SETSUCCESSOR)) {
		return 0;
	} else if (getPayloadLength() != sizeof(uint64_t)) {
		return 0;
	} else {
		uint64_t v = key;
		Serializer::unpack(payload(), (char*) "Q", &v);
		if (v == key) {
			return header().getLength();
		} else {
			return 0;
		}
	}
}

bool OverlayProtocol::setSuccessorRequest(uint64_t host, uint64_t key) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=3, AQLF=0/1/127
	 * BODY: 8 bytes as <successor> in Request; 8 bytes as <successor> in response
	 * TOTAL: 32+8=40 bytes
	 */
	return createSetSuccessorRequest(host, key) && executeRequest()
			&& processSetSuccessorResponse(key);
}

unsigned int OverlayProtocol::createGetFingerRequest(uint64_t host,
		uint32_t index) noexcept {
	Packet::clear();
	header().setAddress(getSource(), host);
	header().setControl((HEADER_SIZE + sizeof(uint32_t)), nextSequenceNumber(),
			getSession());
	header().setContext(WH_DHT_CMD_NODE, WH_DHT_QLF_GETFINGER,
			WH_DHT_AQLF_REQUEST);
	packHeader();
	Serializer::pack(payload(), "L", index);
	return header().getLength();
}

unsigned int OverlayProtocol::processGetFingerResponse(uint32_t index,
		uint64_t &key) const noexcept {
	if (!checkContext(WH_DHT_CMD_NODE, WH_DHT_QLF_GETFINGER)) {
		return 0;
	} else if (getPayloadLength() != sizeof(uint32_t) + sizeof(uint64_t)) {
		return 0;
	} else {
		uint32_t v0 = index;
		uint64_t v1 = 0;
		Serializer::unpack(payload(), (char*) "LQ", &v0, &v1);
		if (v0 == index) {
			key = v1;
			return header().getLength();
		} else {
			key = 0;
			return 0;
		}
	}
}

bool OverlayProtocol::getFingerRequest(uint64_t host, uint32_t index,
		uint64_t &key) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=4, AQLF=0/1/127
	 * BODY: 4 bytes in Request as <index>; 4 bytes as <index> + 8 bytes
	 * as <finger> in Response
	 * TOTAL: 32+4=36 bytes in Request; 32+4+8=44 bytes in Response
	 */
	return createGetFingerRequest(host, index) && executeRequest()
			&& processGetFingerResponse(index, key);
}

unsigned int OverlayProtocol::createSetFingerRequest(uint64_t host,
		uint32_t index, uint64_t key) noexcept {
	Packet::clear();
	header().setAddress(getSource(), host);
	header().setControl((HEADER_SIZE + sizeof(uint32_t) + sizeof(uint64_t)),
			nextSequenceNumber(), getSession());
	header().setContext(WH_DHT_CMD_NODE, WH_DHT_QLF_SETFINGER,
			WH_DHT_AQLF_REQUEST);
	packHeader();
	Serializer::pack(payload(), "LQ", index, key);
	return header().getLength();
}

unsigned int OverlayProtocol::processSetFingerResponse(uint32_t index,
		uint64_t key) const noexcept {
	if (!checkContext(WH_DHT_CMD_NODE, WH_DHT_QLF_SETFINGER)) {
		return 0;
	} else if (getPayloadLength() != sizeof(uint32_t) + sizeof(uint64_t)) {
		return 0;
	} else {
		uint32_t v0 = index;
		uint64_t v1 = key;
		Serializer::unpack(payload(), (char*) "LQ", &v0, &v1);
		if (v0 == index && v1 == key) {
			return header().getLength();
		} else {
			return 0;
		}
	}
}

bool OverlayProtocol::setFingerRequest(uint64_t host, uint32_t index,
		uint64_t key) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=5, AQLF=0/1/127
	 * BODY: 4 bytes as <index> and 8 bytes as <finger> in Request and in Response
	 * TOTAL: 32+4+8=44 bytes
	 */
	return createSetFingerRequest(host, index, key) && executeRequest()
			&& processSetFingerResponse(index, key);
}

unsigned int OverlayProtocol::createGetNeighboursRequest(uint64_t host) noexcept {
	Packet::clear();
	header().setAddress(getSource(), host);
	header().setControl(HEADER_SIZE, nextSequenceNumber(), getSession());
	header().setContext(WH_DHT_CMD_NODE, WH_DHT_QLF_GETNEIGHBOURS,
			WH_DHT_AQLF_REQUEST);
	packHeader();
	return header().getLength();
}

unsigned int OverlayProtocol::processGetNeighboursResponse(
		uint64_t &predecessor, uint64_t &successor) const noexcept {
	if (!checkContext(WH_DHT_CMD_NODE, WH_DHT_QLF_GETNEIGHBOURS)) {
		return 0;
	} else if (getPayloadLength() != (2 * sizeof(uint64_t))) {
		return 0;
	} else {
		uint64_t v[2];
		Serializer::unpack(payload(), (char*) "QQ", &v[0], &v[1]);
		predecessor = v[0];
		successor = v[1];
		return header().getLength();
	}
}

bool OverlayProtocol::getNeighboursRequest(uint64_t host, uint64_t &predecessor,
		uint64_t &successor) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=6, AQLF=0/1/127
	 * BODY: 0 bytes in REQ; 8 bytes as <predecessor> + 8 bytes
	 * as <successor> in Response
	 * TOTAL: 32 bytes in Request; 32+8+8=48 bytes in Response
	 */
	return createGetNeighboursRequest(host) && executeRequest()
			&& processGetNeighboursResponse(predecessor, successor);
}

unsigned int OverlayProtocol::createNotifyRequest(uint64_t host,
		uint64_t predecessor) noexcept {
	Packet::clear();
	header().setAddress(getSource(), host);
	header().setControl((HEADER_SIZE + sizeof(uint64_t)), nextSequenceNumber(),
			getSession());
	header().setContext(WH_DHT_CMD_NODE, WH_DHT_QLF_NOTIFY,
			WH_DHT_AQLF_REQUEST);
	packHeader();
	Serializer::pack(payload(), "Q", predecessor);
	return header().getLength();
}

unsigned int OverlayProtocol::processNotifyResponse() const noexcept {
	if (!validate()) {
		return 0;
	} else if (!checkContext(WH_DHT_CMD_NODE, WH_DHT_QLF_NOTIFY)) {
		return 0;
	} else if (header().getLength() != HEADER_SIZE) {
		return 0;
	} else {
		return header().getLength();
	}
}

bool OverlayProtocol::notifyRequest(uint64_t host, uint64_t predecessor) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=7, AQLF=0/1/127
	 * BODY: 8 bytes as <predecessor> in Request; 0 bytes in Response
	 * TOTAL: 32+8=40 bytes in Request; 32 bytes in Response
	 */
	return createNotifyRequest(host, predecessor) && executeRequest()
			&& processNotifyResponse();
}

unsigned int OverlayProtocol::createFindSuccessorRequest(uint64_t host,
		uint64_t uid) noexcept {
	Packet::clear();
	header().setAddress(getSource(), host);
	header().setControl((HEADER_SIZE + sizeof(uint64_t)), nextSequenceNumber(),
			getSession());
	header().setContext(WH_DHT_CMD_OVERLAY, WH_DHT_QLF_FINDSUCCESSOR,
			WH_DHT_AQLF_REQUEST);
	packHeader();
	Serializer::pack(payload(), "Q", uid);
	return header().getLength();
}

unsigned int OverlayProtocol::processFindSuccessorResponse(uint64_t uid,
		uint64_t &key) const noexcept {
	if (!checkContext(WH_DHT_CMD_OVERLAY, WH_DHT_QLF_FINDSUCCESSOR)) {
		return 0;
	} else if (getPayloadLength() != (2 * sizeof(uint64_t))) {
		return 0;
	} else {
		uint64_t v[2] = { uid, 0 };
		Serializer::unpack(payload(), (char*) "QQ", &v[0], &v[1]);
		if (v[0] == uid) {
			key = v[1];
			return header().getLength();
		} else {
			key = 0;
			return 0;
		}
	}
}

bool OverlayProtocol::findSuccessorRequest(uint64_t host, uint64_t uid,
		uint64_t &key) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=4, QLF=0, AQLF=0/1/127
	 * BODY: 8 bytes as <id> in Request; 8 bytes as <id> and 8 bytes
	 * as <successor> in Response
	 * TOTAL: 32+8=40 bytes in Request; 32+8+8=48 bytes in Response
	 */
	return createFindSuccessorRequest(host, uid) && executeRequest()
			&& processFindSuccessorResponse(uid, key);
}

unsigned int OverlayProtocol::createPingRequest(uint64_t host) noexcept {
	Packet::clear();
	header().setAddress(getSource(), host);
	header().setControl(HEADER_SIZE, nextSequenceNumber(), getSession());
	header().setContext(WH_DHT_CMD_OVERLAY, WH_DHT_QLF_PING,
			WH_DHT_AQLF_REQUEST);
	packHeader();
	return header().getLength();
}

unsigned int OverlayProtocol::processPingRequest() const noexcept {
	if (!validate()) {
		return 0;
	} else if (!checkContext(WH_DHT_CMD_OVERLAY, WH_DHT_QLF_PING)) {
		return 0;
	} else {
		return header().getLength();
	}
}

bool OverlayProtocol::pingRequest(uint64_t host) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=4, QLF=1, AQLF=0/1/127
	 * BODY: 0 bytes in Request and Response
	 * TOTAL: 32 bytes in Request and Response
	 */
	return createPingRequest(host) && executeRequest() && processPingRequest();
}

unsigned int OverlayProtocol::createMapRequest(uint64_t host) noexcept {
	Packet::clear();
	header().setAddress(getSource(), host);
	header().setControl(HEADER_SIZE, nextSequenceNumber(), getSession());
	header().setContext(WH_DHT_CMD_OVERLAY, WH_DHT_QLF_MAP,
			WH_DHT_AQLF_REQUEST);
	packHeader();
	return header().getLength();
}

unsigned int OverlayProtocol::processMapRequest() const noexcept {
	if (!validate()) {
		return 0;
	} else if (!checkContext(WH_DHT_CMD_OVERLAY, WH_DHT_QLF_MAP)) {
		return 0;
	} else {
		return header().getLength();
	}
}

bool OverlayProtocol::mapRequest(uint64_t host) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=4, QLF=2, AQLF=0/1/127
	 * BODY: variable in Request; variable in Response
	 * TOTAL: at least 32 bytes in Request and Response
	 */
	return createMapRequest(host) && executeRequest() && processMapRequest();
}

} /* namespace wanhive */

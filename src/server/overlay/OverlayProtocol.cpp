/*
 * OverlayProtocol.cpp
 *
 * Protocol extension for the Wanhive overlay network clients
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

unsigned int OverlayProtocol::createDescribeRequest(uint64_t id) noexcept {
	header().load(getSource(), id, Message::HEADER_SIZE, nextSequenceNumber(),
			getSession(), WH_DHT_CMD_NULL, WH_DHT_QLF_DESCRIBE,
			WH_DHT_AQLF_REQUEST);
	header().serialize(buffer());
	return header().getLength();
}

unsigned int OverlayProtocol::processDescribeResponse(
		OverlayHubInfo &info) const noexcept {
	if (!checkCommand(WH_DHT_CMD_NULL, WH_DHT_QLF_DESCRIBE)) {
		return 0;
	} else if (getHeader().getLength() < Message::HEADER_SIZE + 84) {
		return 0;
	}
	//-----------------------------------------------------------------
	//ID(8) -> MTU(2) -> MAX_CONN(4) -> CONN(4) -> MAX_MSGS(4) -> MSGS(4) -> UPTIME (Lf)
	unsigned int index = 0;
	info.uid = Serializer::unpacku64(getPayload(index)); //KEY
	index += sizeof(uint64_t);
	info.mtu = Serializer::unpacku16(getPayload(index));
	index += sizeof(uint16_t);
	info.resource.maxConnections = Serializer::unpacku32(getPayload(index));
	index += sizeof(uint32_t);
	info.resource.connections = Serializer::unpacku32(getPayload(index));
	index += sizeof(uint32_t);
	info.resource.maxMessages = Serializer::unpacku32(getPayload(index));
	index += sizeof(uint32_t);
	info.resource.messages = Serializer::unpacku32(getPayload(index));
	index += sizeof(uint32_t);
	info.stat.uptime = Serializer::unpackf64(getPayload(index));
	index += sizeof(uint64_t);
	//-----------------------------------------------------------------
	//IN_PACKETS(8) -> IN_BYTES(8) -> DROPPED_PACKETS(8) -> DROPPED_BYTES(8)
	info.stat.receivedPackets = Serializer::unpacku64(getPayload(index));
	index += sizeof(uint64_t);
	info.stat.receivedBytes = Serializer::unpacku64(getPayload(index));
	index += sizeof(uint64_t);
	info.stat.droppedPackets = Serializer::unpacku64(getPayload(index));
	index += sizeof(uint64_t);
	info.stat.droppedBytes = Serializer::unpacku64(getPayload(index));
	index += sizeof(uint64_t);
	//-----------------------------------------------------------------
	//Predecessor(8) -> Successor(8) -> STABLE_FLAG(1) -> TABLE_SIZE(1)
	info.predecessor = Serializer::unpacku64(getPayload(index)); //Predecessor
	index += sizeof(uint64_t);
	info.successor = Serializer::unpacku64(getPayload(index)); //Successor
	index += sizeof(uint64_t);
	info.stable = Serializer::unpacku8(getPayload(index)); //Routing Table Status
	index += sizeof(uint8_t);
	info.routes = Serializer::unpacku8(getPayload(index)); //Routing Table Size
	index += sizeof(uint8_t);
	//-----------------------------------------------------------------
	if (info.routes > DHT::IDENTIFIER_LENGTH
			|| getHeader().getLength()
					!= (Message::HEADER_SIZE + 84 + (info.routes * 25))) {
		return 0;
	}
	for (unsigned int i = 0; i < info.routes; i++) {
		//START(8) -> ID(8) -> OLD_ID(8) -> CONNECTED(1)
		info.route[i].start = Serializer::unpacku64(getPayload(index));
		index += sizeof(uint64_t);
		info.route[i].current = Serializer::unpacku64(getPayload(index));
		index += sizeof(uint64_t);
		info.route[i].old = Serializer::unpacku64(getPayload(index));
		index += sizeof(uint64_t);
		info.route[i].connected = Serializer::unpacku8(getPayload(index));
		index += sizeof(uint8_t);
	}
	//-----------------------------------------------------------------
	return getHeader().getLength();
}

bool OverlayProtocol::describeRequest(uint64_t id, OverlayHubInfo &info) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=0, QLF=127, AQLF=0/1/127
	 * BODY: 0 bytes in Request; 84+25*Node::TABLESIZE bytes in Response
	 * TOTAL: 32 bytes in Request; 116+25*Node::TABLESIZE bytes in Response
	 */
	return createDescribeRequest(id) && executeRequest()
			&& processDescribeResponse(info);
}

unsigned int OverlayProtocol::createGetPredecessorRequest(uint64_t id) noexcept {
	header().load(getSource(), id, Message::HEADER_SIZE, nextSequenceNumber(),
			getSession(), WH_DHT_CMD_NODE, WH_DHT_QLF_GETPREDECESSOR,
			WH_DHT_AQLF_REQUEST);
	header().serialize(buffer());
	return header().getLength();
}

unsigned int OverlayProtocol::processGetPredecessorResponse(
		uint64_t &key) const noexcept {
	if (!checkCommand(WH_DHT_CMD_NODE, WH_DHT_QLF_GETPREDECESSOR)) {
		return 0;
	} else if (getHeader().getLength()
			!= Message::HEADER_SIZE + sizeof(uint64_t)) {
		return 0;
	} else {
		uint64_t v;
		Serializer::unpack(getPayload(), (char*) "Q", &v);
		key = v;
		return getHeader().getLength();
	}
}

bool OverlayProtocol::getPredecessorRequest(uint64_t id, uint64_t &key) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=0, AQLF=0/1/127
	 * BODY: 0 bytes in Request; 8 bytes as <predecessor> in Response
	 * TOTAL: 32 bytes in Request; 32+8=40 bytes in Response
	 */
	return createGetPredecessorRequest(id) && executeRequest()
			&& processGetPredecessorResponse(key);
}

unsigned int OverlayProtocol::createSetPredecessorRequest(uint64_t id,
		uint64_t key) noexcept {
	header().load(getSource(), id, (Message::HEADER_SIZE + sizeof(uint64_t)),
			nextSequenceNumber(), getSession(), WH_DHT_CMD_NODE,
			WH_DHT_QLF_SETPREDECESSOR, WH_DHT_AQLF_REQUEST);
	header().serialize(buffer());
	Serializer::pack(buffer() + Message::HEADER_SIZE, "Q", key);
	return header().getLength();
}

unsigned int OverlayProtocol::processSetPredecessorResponse(
		uint64_t key) const noexcept {
	if (!checkCommand(WH_DHT_CMD_NODE, WH_DHT_QLF_SETPREDECESSOR)) {
		return 0;
	} else if (getHeader().getLength()
			!= Message::HEADER_SIZE + sizeof(uint64_t)) {
		return 0;
	} else {
		uint64_t v;
		Serializer::unpack(getPayload(), (char*) "Q", &v);
		if (v == key) {
			return getHeader().getLength();
		} else {
			return 0;
		}
	}
}

bool OverlayProtocol::setPredecessorRequest(uint64_t id, uint64_t key) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=1, AQLF=0/1/127
	 * BODY: 8 bytes as <predecessor> in Request; 8 bytes as <predecessor> in Response
	 * TOTAL: 32+8=40 bytes
	 */
	return createSetPredecessorRequest(id, key) && executeRequest()
			&& processSetPredecessorResponse(key);
}

unsigned int OverlayProtocol::createGetSuccessorRequest(uint64_t id) noexcept {
	header().load(getSource(), id, Message::HEADER_SIZE, nextSequenceNumber(),
			getSession(), WH_DHT_CMD_NODE, WH_DHT_QLF_GETSUCCESSOR,
			WH_DHT_AQLF_REQUEST);
	header().serialize(buffer());
	return header().getLength();
}

unsigned int OverlayProtocol::processGetSuccessorResponse(
		uint64_t &key) const noexcept {
	if (!checkCommand(WH_DHT_CMD_NODE, WH_DHT_QLF_GETSUCCESSOR)) {
		return 0;
	} else if (getHeader().getLength()
			!= Message::HEADER_SIZE + sizeof(uint64_t)) {
		return 0;
	} else {
		uint64_t v;
		Serializer::unpack(getPayload(), (char*) "Q", &v);
		key = v;
		return getHeader().getLength();
	}
}

bool OverlayProtocol::getSuccessorRequest(uint64_t id, uint64_t &key) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=2, AQLF=0/1/127
	 * BODY: 0 bytes in Request; 8 bytes as <successor> in Response
	 * TOTAL: 32 bytes in Request; 32+8=40 bytes in Response
	 */
	return createGetSuccessorRequest(id) && executeRequest()
			&& processGetSuccessorResponse(key);
}

unsigned int OverlayProtocol::createSetSuccessorRequest(uint64_t id,
		uint64_t key) noexcept {
	header().load(getSource(), id, (Message::HEADER_SIZE + sizeof(uint64_t)),
			nextSequenceNumber(), getSession(), WH_DHT_CMD_NODE,
			WH_DHT_QLF_SETSUCCESSOR, WH_DHT_AQLF_REQUEST);
	header().serialize(buffer());
	Serializer::pack(buffer() + Message::HEADER_SIZE, "Q", key);
	return header().getLength();
}

unsigned int OverlayProtocol::processSetSuccessorResponse(
		uint64_t key) const noexcept {
	if (!checkCommand(WH_DHT_CMD_NODE, WH_DHT_QLF_SETSUCCESSOR)) {
		return 0;
	} else if (getHeader().getLength()
			!= Message::HEADER_SIZE + sizeof(uint64_t)) {
		return 0;
	} else {
		uint64_t v = key;
		Serializer::unpack(getPayload(), (char*) "Q", &v);
		if (v == key) {
			return getHeader().getLength();
		} else {
			return 0;
		}
	}
}

bool OverlayProtocol::setSuccessorRequest(uint64_t id, uint64_t key) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=3, AQLF=0/1/127
	 * BODY: 8 bytes as <successor> in Request; 8 bytes as <successor> in response
	 * TOTAL: 32+8=40 bytes
	 */
	return createSetSuccessorRequest(id, key) && executeRequest()
			&& processSetSuccessorResponse(key);
}

unsigned int OverlayProtocol::createGetFingerRequest(uint64_t id,
		uint32_t index) noexcept {
	header().load(getSource(), id, (Message::HEADER_SIZE + sizeof(uint32_t)),
			nextSequenceNumber(), getSession(), WH_DHT_CMD_NODE,
			WH_DHT_QLF_GETFINGER, WH_DHT_AQLF_REQUEST);
	header().serialize(buffer());
	Serializer::pack(buffer() + Message::HEADER_SIZE, "L", index);
	return header().getLength();
}

unsigned int OverlayProtocol::processGetFingerResponse(uint32_t index,
		uint64_t &key) const noexcept {
	if (!checkCommand(WH_DHT_CMD_NODE, WH_DHT_QLF_GETFINGER)) {
		return 0;
	} else if (getHeader().getLength()
			!= Message::HEADER_SIZE + sizeof(uint32_t) + sizeof(uint64_t)) {
		return 0;
	} else {
		uint32_t v0 = index;
		uint64_t v1 = 0;
		Serializer::unpack(getPayload(), (char*) "LQ", &v0, &v1);
		if (v0 == index) {
			key = v1;
			return getHeader().getLength();
		} else {
			key = 0;
			return 0;
		}
	}
}

bool OverlayProtocol::getFingerRequest(uint64_t id, uint32_t index,
		uint64_t &key) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=4, AQLF=0/1/127
	 * BODY: 4 bytes in Request as <index>; 4 bytes as <index> + 8 bytes as <finger> in Response
	 * TOTAL: 32+4=36 bytes in Request; 32+4+8=44 bytes in Response
	 */
	return createGetFingerRequest(id, index) && executeRequest()
			&& processGetFingerResponse(index, key);
}

unsigned int OverlayProtocol::createSetFingerRequest(uint64_t id,
		uint32_t index, uint64_t key) noexcept {
	header().load(getSource(), id,
			(Message::HEADER_SIZE + sizeof(uint32_t) + sizeof(uint64_t)),
			nextSequenceNumber(), getSession(), WH_DHT_CMD_NODE,
			WH_DHT_QLF_SETFINGER, WH_DHT_AQLF_REQUEST);
	header().serialize(buffer());
	Serializer::pack(buffer() + Message::HEADER_SIZE, "LQ", index, key);
	return header().getLength();
}

unsigned int OverlayProtocol::processSetFingerResponse(uint32_t index,
		uint64_t key) const noexcept {
	if (!checkCommand(WH_DHT_CMD_NODE, WH_DHT_QLF_SETFINGER)) {
		return 0;
	} else if (getHeader().getLength()
			!= Message::HEADER_SIZE + sizeof(uint32_t) + sizeof(uint64_t)) {
		return 0;
	} else {
		uint32_t v0 = index;
		uint64_t v1 = key;
		Serializer::unpack(getPayload(), (char*) "LQ", &v0, &v1);
		if (v0 == index && v1 == key) {
			return getHeader().getLength();
		} else {
			return 0;
		}
	}
}

bool OverlayProtocol::setFingerRequest(uint64_t id, uint32_t index,
		uint64_t key) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=5, AQLF=0/1/127
	 * BODY: 4 bytes as <index> and 8 bytes as <finger> in Request and in Response
	 * TOTAL: 32+4+8=44 bytes
	 */
	return createSetFingerRequest(id, index, key) && executeRequest()
			&& processSetFingerResponse(index, key);
}

unsigned int OverlayProtocol::createGetNeighboursRequest(uint64_t id) noexcept {
	header().load(getSource(), id, Message::HEADER_SIZE, nextSequenceNumber(),
			getSession(), WH_DHT_CMD_NODE, WH_DHT_QLF_GETNEIGHBOURS,
			WH_DHT_AQLF_REQUEST);
	header().serialize(buffer());
	return header().getLength();
}

unsigned int OverlayProtocol::processGetNeighboursResponse(
		uint64_t &predecessor, uint64_t &successor) const noexcept {
	if (!checkCommand(WH_DHT_CMD_NODE, WH_DHT_QLF_GETNEIGHBOURS)) {
		return 0;
	} else if (getHeader().getLength()
			!= Message::HEADER_SIZE + 2 * sizeof(uint64_t)) {
		return 0;
	} else {
		uint64_t v[2];
		Serializer::unpack(getPayload(), (char*) "QQ", &v[0], &v[1]);
		predecessor = v[0];
		successor = v[1];
		return getHeader().getLength();
	}
}

bool OverlayProtocol::getNeighboursRequest(uint64_t id, uint64_t &predecessor,
		uint64_t &successor) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=6, AQLF=0/1/127
	 * BODY: 0 bytes in REQ; 8 bytes as <predecessor> + 8 bytes as <successor> in Response
	 * TOTAL: 32 bytes in Request; 32+8+8=48 bytes in Response
	 */
	return createGetNeighboursRequest(id) && executeRequest()
			&& processGetNeighboursResponse(predecessor, successor);
}

unsigned int OverlayProtocol::createNotifyRequest(uint64_t id,
		uint64_t predecessor) noexcept {
	header().load(getSource(), id, (Message::HEADER_SIZE + sizeof(uint64_t)),
			nextSequenceNumber(), getSession(), WH_DHT_CMD_NODE,
			WH_DHT_QLF_NOTIFY, WH_DHT_AQLF_REQUEST);
	header().serialize(buffer());
	Serializer::pack(buffer() + Message::HEADER_SIZE, "Q", predecessor);
	return header().getLength();
}

unsigned int OverlayProtocol::processNotifyResponse() const noexcept {
	if (!checkCommand(WH_DHT_CMD_NODE, WH_DHT_QLF_NOTIFY)) {
		return 0;
	} else if (getHeader().getLength() != Message::HEADER_SIZE) {
		return 0;
	} else {
		return getHeader().getLength();
	}
}

bool OverlayProtocol::notifyRequest(uint64_t id, uint64_t predecessor) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=7, AQLF=0/1/127
	 * BODY: 8 bytes as <predecessor> in Request; 0 bytes in Response
	 * TOTAL: 32+8=40 bytes in Request; 32 bytes in Response
	 */
	return createNotifyRequest(id, predecessor) && executeRequest()
			&& processNotifyResponse();
}

unsigned int OverlayProtocol::createFindSuccessorRequest(uint64_t id,
		uint64_t uid) noexcept {
	header().load(getSource(), id, (Message::HEADER_SIZE + sizeof(uint64_t)),
			nextSequenceNumber(), getSession(), WH_DHT_CMD_OVERLAY,
			WH_DHT_QLF_FINDSUCCESSOR, WH_DHT_AQLF_REQUEST);
	header().serialize(buffer());
	Serializer::pack(buffer() + Message::HEADER_SIZE, "Q", uid);
	return header().getLength();
}

unsigned int OverlayProtocol::processFindSuccessorResponse(uint64_t uid,
		uint64_t &key) const noexcept {
	if (!checkCommand(WH_DHT_CMD_OVERLAY, WH_DHT_QLF_FINDSUCCESSOR)) {
		return 0;
	} else if (getHeader().getLength()
			!= Message::HEADER_SIZE + 2 * sizeof(uint64_t)) {
		return 0;
	} else {
		uint64_t v[2] = { uid, 0 };
		Serializer::unpack(getPayload(), (char*) "QQ", &v[0], &v[1]);
		if (v[0] == uid) {
			key = v[1];
			return getHeader().getLength();
		} else {
			key = 0;
			return 0;
		}
	}
}

bool OverlayProtocol::findSuccessorRequest(uint64_t id, uint64_t uid,
		uint64_t &key) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=4, QLF=0, AQLF=0/1/127
	 * BODY: 8 bytes as <id> in Request; 8 bytes as <id> and 8 bytes as <successor> in Response
	 * TOTAL: 32+8=40 bytes in Request; 32+8+8=48 bytes in Response
	 */
	return createFindSuccessorRequest(id, uid) && executeRequest()
			&& processFindSuccessorResponse(uid, key);
}

unsigned int OverlayProtocol::createPingRequest(uint64_t id) noexcept {
	header().load(getSource(), id, Message::HEADER_SIZE, nextSequenceNumber(),
			getSession(), WH_DHT_CMD_OVERLAY, WH_DHT_QLF_PING,
			WH_DHT_AQLF_REQUEST);
	header().serialize(buffer());
	return header().getLength();
}

unsigned int OverlayProtocol::processPingRequest() const noexcept {
	if (!checkCommand(WH_DHT_CMD_OVERLAY, WH_DHT_QLF_PING)) {
		return 0;
	} else {
		return getHeader().getLength();
	}
}

bool OverlayProtocol::pingRequest(uint64_t id) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=4, QLF=1, AQLF=0/1/127
	 * BODY: 0 bytes in Request and Response
	 * TOTAL: 32 bytes in Request and Response
	 */
	return createPingRequest(id) && executeRequest() && processPingRequest();
}

unsigned int OverlayProtocol::createMapRequest(uint64_t id) noexcept {
	header().load(getSource(), id, Message::HEADER_SIZE, nextSequenceNumber(),
			getSession(), WH_DHT_CMD_OVERLAY, WH_DHT_QLF_MAP,
			WH_DHT_AQLF_REQUEST);
	header().serialize(buffer());
	return header().getLength();
}

unsigned int OverlayProtocol::processMapRequest() const noexcept {
	if (!checkCommand(WH_DHT_CMD_OVERLAY, WH_DHT_QLF_MAP)) {
		return 0;
	} else {
		return getHeader().getLength();
	}
}

bool OverlayProtocol::mapRequest(uint64_t id) {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=4, QLF=2, AQLF=0/1/127
	 * BODY: variable in Request; variable in Response
	 * TOTAL: at least 32 bytes in Request and Response
	 */
	return createMapRequest(id) && executeRequest() && processMapRequest();
}

} /* namespace wanhive */

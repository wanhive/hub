/*
 * OverlayTool.cpp
 *
 * Overlay network test-suite
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "OverlayTool.h"
#include "commands.h"
#include "../../base/common/CommandLine.h"
#include "../../base/common/Exception.h"
#include "../../base/common/Logger.h"
#include "../../base/ds/Serializer.h"
#include "../../hub/Topic.h"
#include "../../util/Random.h"
#include <iostream>

namespace wanhive {

OverlayTool::OverlayTool(const char *path, unsigned int timeout) noexcept :
		Identity(path), timeout(timeout), hostId(0), destinationId(0), auth(
				false) {
}

OverlayTool::~OverlayTool() {

}

void OverlayTool::run() noexcept {
	try {
		setup();
		execute();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
	}
}

void OverlayTool::setup() {
	Identity::initialize();
}

void OverlayTool::execute() noexcept {
	unsigned int command = 255;
	unsigned int qualifier = 255;
	bool running = true;
	while (running) {
		std::cout << "[" << getSource() << "@" << hostId << ":" << destinationId
				<< "] " << "Enter a command: ";
		std::cin >> command;
		if (CommandLine::inputError()) {
			break;
		}
		if (command <= 4) {
			std::cout << "[" << getSource() << "@" << hostId << ":"
					<< destinationId << "] " << "Enter a qualifier: ";
			std::cin >> qualifier;
			if (CommandLine::inputError()) {
				continue;
			}
		}
		try {
			switch (command) {
			case WH_DHT_CMD_NULL:
				if (qualifier == WH_DHT_QLF_IDENTIFY) {
					identifyCmd();
				} else if (qualifier == WH_DHT_QLF_AUTHENTICATE) {
					authenticateCmd();
				} else if (qualifier == 3) {
					authorizeCmd();
				} else if (qualifier == WH_DHT_QLF_DESCRIBE) {
					describeCmd();
				} else {
					std::cout << "Invalid command" << std::endl;
				}
				break;
			case WH_DHT_CMD_BASIC:
				if (qualifier == WH_DHT_QLF_REGISTER) {
					registerCmd();
				} else if (qualifier == WH_DHT_QLF_GETKEY) {
					getKeyCmd();
				} else if (qualifier == WH_DHT_QLF_FINDROOT) {
					findRoot();
				} else if (qualifier == WH_DHT_QLF_BOOTSTRAP) {
					findBootstrapNode();
				} else {
					std::cout << "Invalid command" << std::endl;
				}
				break;
			case WH_DHT_CMD_MULTICAST:
				if (qualifier == WH_DHT_QLF_PUBLISH) {
					publishCmd();
				} else if (qualifier == WH_DHT_QLF_SUBSCRIBE) {
					subscribeCmd();
				} else if (qualifier == WH_DHT_QLF_UNSUBSCRIBE) {
					unsubscribeCmd();
				} else {
					std::cout << "Invalid command" << std::endl;
				}
				break;
			case WH_DHT_CMD_NODE:
				if (qualifier == WH_DHT_QLF_GETPREDECESSOR) {
					getPredecessorCmd();
				} else if (qualifier == WH_DHT_QLF_SETPREDECESSOR) {
					setPredecessorCmd();
				} else if (qualifier == WH_DHT_QLF_GETSUCCESSOR) {
					getSuccessorCmd();
				} else if (qualifier == WH_DHT_QLF_SETSUCCESSOR) {
					setSuccessorCmd();
				} else if (qualifier == WH_DHT_QLF_GETFINGER) {
					getFingerCmd();
				} else if (qualifier == WH_DHT_QLF_SETFINGER) {
					setFingerCmd();
				} else if (qualifier == WH_DHT_QLF_GETNEIGHBOURS) {
					getNeighboursCmd();
				} else if (qualifier == WH_DHT_QLF_NOTIFY) {
					notifyCmd();
				} else {
					std::cout << "Invalid command" << std::endl;
				}
				break;
			case WH_DHT_CMD_OVERLAY:
				if (qualifier == WH_DHT_QLF_FINDSUCCESSOR) {
					findSuccessorCmd();
				} else if (qualifier == WH_DHT_QLF_PING) {
					pingCmd();
				} else if (qualifier == WH_DHT_QLF_MAP) {
					mapCmd();
				} else {
					std::cout << "Invalid command" << std::endl;
				}
				break;
			case 5:
				std::cout << "Target's identity: ";
				std::cin >> destinationId;
				if (CommandLine::inputError()) {
					continue;
				}
				break;
			case 6:
				useKeyPair(Identity::getPKI());
				std::cout << "Keys loaded" << std::endl;
				break;
			case 7:
				useKeyPair(nullptr);
				std::cout << "Keys discarded" << std::endl;
				break;
			case 8:
				connect();
				break;
			case 9:
				disconnect();
				break;
			default:
				running = false;
				break;
			}
		} catch (const BaseException &e) {
			disconnect();
			WH_LOG_EXCEPTION(e);
		}
	}

	std::cout << "Good bye" << std::endl;
}

void OverlayTool::connect() {
	unsigned long long host = 0;
	std::cout << "Host's identity: ";
	std::cin >> host;
	if (CommandLine::inputError()) {
		return;
	}

	try {
		NameInfo ni;
		Identity::getAddress(host, ni);
		Endpoint::setSSLContext(Identity::getSSLContext());
		Endpoint::connect(ni, timeout);
		std::cout << "Connected with the host: " << host << std::endl;
		hostId = host;
		destinationId = host;
	} catch (const BaseException &e) {
		std::cout << "Connection denied by the host: " << host << std::endl;
		throw;
	}
}

void OverlayTool::disconnect() noexcept {
	if (getSocket() != -1) {
		Endpoint::disconnect();
		std::cout << "Disconnected from the host: " << hostId << std::endl;
		hostId = 0;
		destinationId = 0;
		setSource(0);
	} else {
		std::cout << "Not connected" << std::endl;
	}
}

void OverlayTool::identifyCmd() {
	std::cout << "CMD: [IDENTIFY]" << std::endl;
	uint64_t id = destinationId;
	unsigned long long identity = 0;
	char password[64];
	unsigned int rounds = 1;

	std::cout << "Identity: ";
	std::cin >> identity;
	if (CommandLine::inputError()) {
		return;
	}

	std::cout << "Password: ";
	std::cin.ignore();
	std::cin.getline(password, sizeof(password));
	if (CommandLine::inputError()) {
		return;
	}

	std::cout << "Password hashing rounds: ";
	std::cin >> rounds;
	if (CommandLine::inputError()) {
		return;
	}

	try {
		Data salt, nonce;
		bool success = auth.generateNonce(nonce.base, nonce.length)
				&& identificationRequest( { identity, id }, nonce)
				&& processIdentificationResponse(salt, nonce)
				&& auth.createIdentity(identity,
						(const unsigned char*) password, strlen(password),
						salt.base, salt.length, nonce.base, nonce.length,
						rounds);

		if (success) {
			std::cout << "IDENTIFY SUCCEEDED" << std::endl;
		} else {
			std::cout << "IDENTIFY FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "IDENTIFY FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::authenticateCmd() {
	std::cout << "CMD: [AUTHENTICATE]" << std::endl;
	uint64_t id = destinationId;

	try {
		Data proof;
		bool success = auth.generateUserProof(proof.base, proof.length)
				&& authenticationRequest( { 0, id }, proof)
				&& processAuthenticationResponse(proof)
				&& auth.authenticateHost(proof.base, proof.length);
		if (success) {
			std::cout << "AUTHENTICATE SUCCEEDED" << std::endl;
		} else {
			std::cout << "AUTHENTICATE FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "AUTHENTICATE FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::authorizeCmd() {
	std::cout << "CMD: [AUTHORIZE]" << std::endl;
	Packet::clear();
	header().setAddress(0, 0);
	header().setControl(Message::HEADER_SIZE, 0, 0);
	header().setContext(WH_DHT_CMD_BASIC, WH_DHT_QLF_REGISTER,
			WH_DHT_AQLF_REQUEST);
	packHeader();
	try {
		executeRequest(false, true);
		if (header().getStatus() != WH_AQLF_REJECTED) {
			std::cout << "AUTHORIZE SUCCEEDED WITH GROUP ID "
					<< (int) header().getSession() << std::endl;
		} else {
			std::cout << "AUTHORIZE FAILED" << std::endl;
		}
	} catch (...) {
		std::cout << "AUTHORIZE FAILED" << std::endl;
	}
}

void OverlayTool::describeCmd() {
	std::cout << "CMD: [DESCRIBE]" << std::endl;
	uint64_t id = destinationId;
	OverlayHubInfo info;
	try {
		if (describeRequest(id, info)) {
			std::cout << "DESCRIBE SUCCEEDED" << std::endl;
			info.print();
		} else {
			std::cout << "DESCRIBE FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "DESCRIBE FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::registerCmd() {
	std::cout << "CMD: [REGISTER]" << std::endl;
	uint64_t id = destinationId;
	uint64_t newId = 0;
	std::cout << "Identity: ";
	std::cin >> newId;
	if (CommandLine::inputError()) {
		return;
	}

	try {
		Digest hc;
		Random().bytes(&hc, Hash::SIZE);
		if (getKeyRequest( { 0, id }, &hc, verifyHost()) && registerRequest( {
				newId, id }, &hc)) {
			std::cout << "REGISTER SUCCEEDED FOR ID: " << newId << std::endl;
			//Message source will be set to this
			setSource(newId);
		} else {
			std::cout << "REGISTER FAILED FOR ID: " << newId << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "REGISTER FAILED FOR ID: " << newId << std::endl;
		throw;
	}
}

void OverlayTool::getKeyCmd() {
	std::cout << "CMD: [GETKEY]" << std::endl;
	uint64_t id = destinationId;

	try {
		Digest hc;
		Random().bytes(&hc, Hash::SIZE);
		if (getKeyRequest( { 0, id }, &hc, verifyHost())) {
			EncodedDigest encodedKey;
			unsigned int encLen = Hash::encode(&hc, &encodedKey);
			std::cout << "GETKEY RETURNED: " << "[" << encLen << "] "
					<< encodedKey << std::endl;
		} else {
			std::cout << "GETKEY FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "GETKEY FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::findRoot() {
	std::cout << "CMD: [FINDROOT]" << std::endl;
	uint64_t startNode = destinationId;
	uint64_t qid = 0;
	std::cout << "Identity: ";
	std::cin >> qid;
	if (CommandLine::inputError()) {
		return;
	}

	try {
		uint64_t successorId = 0;
		if (findRootRequest(startNode, qid, successorId)) {
			std::cout << "FINDROOT SUCCEEDED FOR ID: " << qid << " WITH VALUE: "
					<< successorId << std::endl;
		} else {
			std::cout << "FINDROOT FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "FINDROOT FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::findBootstrapNode() {
	std::cout << "CMD: [BOOTSTRAP]" << std::endl;
	uint64_t id = destinationId;

	try {
		uint64_t key[64];
		uint32_t limit = 64;
		if (bootstrapRequest(id, key, limit)) {
			std::cout << "BOOTSTRAP NODES: ";
			for (uint32_t i = 0; i < limit; i++) {
				std::cout << key[i] << " ";
			}
			std::cout << std::endl;
		} else {
			std::cout << "BOOTSTRAP FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "BOOTSTRAP FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::publishCmd() {
	std::cout << "CMD: [PUBLISH]" << std::endl;
	uint64_t id = destinationId;

	unsigned int topic = 0;
	std::cout << "Topic [" << Topic::MIN_ID << "-" << Topic::MAX_ID << "]: ";
	std::cin >> topic;
	if (CommandLine::inputError()) {
		return;
	}

	if (topic > Topic::MAX_ID) {
		std::cout << "Invalid topic" << std::endl;
		return;
	}

	char s[128];
	std::cout << "Message (max 100 characters): ";
	std::cin.ignore();
	std::cin.getline(s, sizeof(s));
	if (CommandLine::inputError()) {
		return;
	}

	try {
		if (publishRequest(id, topic, { (const unsigned char*) s,
				(unsigned int) strlen(s) })) {
			std::cout << "PUBLISH SUCCEEDED" << std::endl;
		} else {
			std::cout << "PUBLISH FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "PUBLISH FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::subscribeCmd() {
	std::cout << "CMD: [SUBSCRIBE]" << std::endl;
	uint64_t id = destinationId;
	unsigned int topic = 0;
	std::cout << "Topic [" << Topic::MIN_ID << "-" << Topic::MAX_ID << "]: ";
	std::cin >> topic;
	if (CommandLine::inputError()) {
		return;
	}

	if (topic > Topic::MAX_ID) {
		std::cout << "Invalid topic" << std::endl;
		return;
	}

	try {
		if (subscribeRequest(id, topic)) {
			std::cout << "SUBSCRIBE SUCCEEDED" << std::endl;
		} else {
			std::cout << "SUBSCRIBE FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "SUBSCRIBE FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::unsubscribeCmd() {
	std::cout << "CMD: [UNSUBSCRIBE]" << std::endl;
	uint64_t id = destinationId;
	unsigned int topic = 0;
	std::cout << "Topic [" << Topic::MIN_ID << "-" << Topic::MAX_ID << "]: ";
	std::cin >> topic;
	if (CommandLine::inputError()) {
		return;
	}

	if (topic > Topic::MAX_ID) {
		std::cout << "Invalid topic" << std::endl;
		return;
	}

	try {
		if (unsubscribeRequest(id, topic)) {
			std::cout << "UNSUBSCRIBE SUCCEEDED" << std::endl;
		} else {
			std::cout << "UNSUBSCRIBE FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "UNSUBSCRIBE FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::getPredecessorCmd() {
	std::cout << "CMD: [GETPREDECESSOR]" << std::endl;
	uint64_t id = destinationId;
	try {
		uint64_t key = 0;
		if (getPredecessorRequest(id, key)) {
			std::cout << "GETPREDECESSOR RETURNED: " << key << std::endl;
		} else {
			std::cout << "GETPREDECESSOR FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "GETPREDECESSOR FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::setPredecessorCmd() {
	std::cout << "CMD: [SETPREDECESSOR]" << std::endl;
	uint64_t id = destinationId;
	uint64_t newPredecessor = 0;
	std::cout << "Predecessor's identity: ";
	std::cin >> newPredecessor;
	if (CommandLine::inputError()) {
		return;
	}

	try {
		if (setPredecessorRequest(id, newPredecessor)) {
			std::cout << "SETPREDECESSOR SUCCEEDED WITH: " << newPredecessor
					<< std::endl;
		} else {
			std::cout << "SETPREDECESSOR FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "SETPREDECESSOR FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::getSuccessorCmd() {
	std::cout << "CMD: [GETSUCCESSOR]" << std::endl;
	uint64_t id = destinationId;
	try {
		uint64_t key = 0;
		if (getSuccessorRequest(id, key)) {
			std::cout << "GETSUCCESSOR RETURNED: " << key << std::endl;
		} else {
			std::cout << "GETSUCCESSOR FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "GETSUCCESSOR FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::setSuccessorCmd() {
	std::cout << "CMD: [SETSUCCESSOR]" << std::endl;
	uint64_t id = destinationId;
	uint64_t newSuccessor = 0;
	std::cout << "Successor's identity: ";
	std::cin >> newSuccessor;
	if (CommandLine::inputError()) {
		return;
	}

	try {
		if (setSuccessorRequest(id, newSuccessor)) {
			std::cout << "SETSUCCESSOR SUCCEEDED WITH: " << newSuccessor
					<< std::endl;
		} else {
			std::cout << "SETSUCCESSOR FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "SETSUCCESSOR FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::getFingerCmd() {
	std::cout << "CMD: [GETFINGER]" << std::endl;
	uint64_t id = destinationId;
	uint32_t index = 0;
	std::cout << "Finger's index: ";
	std::cin >> index;
	if (CommandLine::inputError()) {
		return;
	}

	try {
		uint64_t key = 0;
		if (getFingerRequest(id, index, key)) {
			std::cout << "GETFINGER RETURNED NODE: " << key << " AT INDEX: "
					<< index << std::endl;
		} else {
			std::cout << "GETFINGER FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "GETFINGER FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::setFingerCmd() {
	std::cout << "CMD: [SETFINGER]" << std::endl;
	uint64_t id = destinationId;
	uint32_t index = 0;
	uint64_t finger = 0;

	std::cout << "Finger's index: ";
	std::cin >> index;
	if (CommandLine::inputError()) {
		return;
	}

	std::cout << "Finger's identity: ";
	std::cin >> finger;
	if (CommandLine::inputError()) {
		return;
	}

	try {
		if (setFingerRequest(id, index, finger)) {
			std::cout << "SETFINGER SUCCEEDED WITH NODE: " << finger
					<< " AT INDEX: " << index << std::endl;
		} else {
			std::cout << "SETFINGER FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "SETFINGER FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::getNeighboursCmd() {
	std::cout << "CMD: [GETNEIGHBOURS]" << std::endl;
	uint64_t id = destinationId;
	try {
		uint64_t predecessor = 0;
		uint64_t successor = 0;
		if (getNeighboursRequest(id, predecessor, successor)) {
			std::cout << "GETNEIGHBOURS RETURNED: [" << predecessor << ", "
					<< successor << "]" << std::endl;
		} else {
			std::cout << "GETNEIGHBOURS FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "GETNEIGHBOURS FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::notifyCmd() {
	std::cout << "CMD: [NOTIFY]" << std::endl;
	uint64_t id = destinationId;
	uint64_t newPredecessor = 0;
	std::cout << "Predecessor's identity: ";
	std::cin >> newPredecessor;
	if (CommandLine::inputError()) {
		return;
	}

	try {
		if (notifyRequest(id, newPredecessor)) {
			std::cout << "NOTIFY SUCCEEDED" << std::endl;
		} else {
			std::cout << "NOTIFY FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "NOTIFY FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::findSuccessorCmd() {
	std::cout << "CMD: [FINDSUCCESSOR]" << std::endl;
	uint64_t startNode = destinationId;
	uint64_t id = 0;
	std::cout << "Identity: ";
	std::cin >> id;
	if (CommandLine::inputError()) {
		return;
	}
	try {
		uint64_t successorId = 0;
		if (findSuccessorRequest(startNode, id, successorId)) {
			std::cout << "FINDSUCCESSOR SUCCEEDED FOR ID: " << id
					<< " WITH VALUE: " << successorId << std::endl;
		} else {
			std::cout << "FINDSUCCESSOR FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "FINDSUCCESSOR FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::pingCmd() {
	std::cout << "CMD: [PING]" << std::endl;
	uint64_t id = destinationId;
	try {
		if (pingRequest(id)) {
			std::cout << "PING SUCCEEDED" << std::endl;
		} else {
			std::cout << "PING FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "PING FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::mapCmd() {
	std::cout << "CMD: [MAP]" << std::endl;
	uint64_t id = destinationId;
	try {
		if (mapRequest(id)) {
			std::cout << "MAP SUCCEEDED" << std::endl;
		} else {
			std::cout << "MAP FAILED" << std::endl;
		}
	} catch (const BaseException &e) {
		std::cout << "MAP FAILED" << std::endl;
		throw;
	}
}

} /* namespace wanhive */

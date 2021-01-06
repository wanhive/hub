/*
 * OverlayTool.cpp
 *
 * Command line tool for testing of the overlay network
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "OverlayTool.h"
#include "commands.h"
#include "../../base/Logger.h"
#include "../../base/common/CommandLine.h"
#include "../../base/common/Exception.h"
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
	} catch (BaseException &e) {
		WH_LOG_EXCEPTION(e);
	}
}

void OverlayTool::setup() {
	Identity::initialize();
	Endpoint::setSSLContext(Identity::getSSLContext());
}

void OverlayTool::execute() noexcept {
	unsigned int command = 255;
	unsigned int qualifier = 255;
	bool running = true;
	while (running) {
		std::cout << "[" << getSource() << "@" << hostId << ":" << destinationId
				<< "] " << "Enter command: ";
		std::cin >> command;
		if (CommandLine::inputError()) {
			break;
		}
		if (command <= 4) {
			std::cout << "[" << getSource() << "@" << hostId << ":"
					<< destinationId << "] " << "Enter qualifier: ";
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
				std::cout << "Enter target's identifier: ";
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
		} catch (BaseException &e) {
			disconnect();
			WH_LOG_EXCEPTION(e);
		}
	}

	std::cout << "Good bye" << std::endl;
}

void OverlayTool::connect() {
	unsigned long long host = 0;
	std::cout << "Enter host's identifier: ";
	std::cin >> host;
	if (CommandLine::inputError()) {
		return;
	}

	try {
		NameInfo ni;
		Identity::getAddress(host, ni);
		Endpoint::connect(ni, timeout);
		std::cout << "Connected with the host: " << host << std::endl;
		hostId = host;
		destinationId = host;
	} catch (BaseException &e) {
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

	std::cout << "Enter identity: ";
	std::cin >> identity;
	if (CommandLine::inputError()) {
		return;
	}

	std::cout << "Enter password: ";
	std::cin.ignore();
	std::cin.getline(password, sizeof(password));
	if (CommandLine::inputError()) {
		return;
	}

	std::cout << "Enter password hashing rounds: ";
	std::cin >> rounds;
	if (CommandLine::inputError()) {
		return;
	}

	try {
		const unsigned char *binary;
		unsigned int bytes;
		bool success = auth.generateNonce(binary, bytes)
				&& identificationRequest(id, identity, binary, bytes);

		if (!success) {
			std::cout << "IDENTIFY FAILED" << std::endl;
			return;
		}

		unsigned int saltLength, nonceLength;
		const unsigned char *salt, *nonce;

		success = processIdentificationResponse(saltLength, salt, nonceLength,
				nonce)
				&& auth.createIdentity(identity,
						(const unsigned char*) password, strlen(password), salt,
						saltLength, nonce, nonceLength, rounds);
		if (success) {
			std::cout << "IDENTIFY SUCCEEDED" << std::endl;
		} else {
			std::cout << "IDENTIFY FAILED" << std::endl;
		}
	} catch (BaseException &e) {
		std::cout << "IDENTIFY FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::authenticateCmd() {
	std::cout << "CMD: [AUTHENTICATE]" << std::endl;
	uint64_t id = destinationId;

	try {
		const unsigned char *proof;
		unsigned int length;
		bool success = auth.generateUserProof(proof, length)
				&& authenticationRequest(id, proof, length)
				&& auth.authenticateHost(getPayload(),
						getHeader().getLength() - Message::HEADER_SIZE);
		if (success) {
			std::cout << "AUTHENTICATE SUCCEEDED" << std::endl;
		} else {
			std::cout << "AUTHENTICATE FAILED" << std::endl;
		}
	} catch (BaseException &e) {
		std::cout << "AUTHENTICATE FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::authorizeCmd() {
	std::cout << "CMD: [AUTHORIZE]" << std::endl;

	MessageHeader header;
	header.load(0, 0, Message::HEADER_SIZE, 0, 0, WH_DHT_CMD_BASIC,
			WH_DHT_QLF_REGISTER, WH_DHT_AQLF_REQUEST);
	pack(header, (const char*) nullptr);
	try {
		executeRequest(false, true);
		if (getHeader().getStatus() != WH_AQLF_REJECTED) {
			std::cout << "AUTHORIZE SUCCEEDED WITH GROUP ID "
					<< (int) getHeader().getSession() << std::endl;
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
			fprintf(stdout, "DESCRIBE SUCCEEDED, received %u bytes\n",
					getHeader().getLength());
			fprintf(stdout, "\n=======================================\n");
			fprintf(stdout, "NODE SETTINGS\n");
			fprintf(stdout, "KEY: %llu\n"
					"PREDECESSOR: %llu, SUCCESSOR: %llu\n"
					"FINGER TABLE [STATUS= %s]:\n", info.uid, info.predecessor,
					info.successor, (info.stable ? "STABLE" : "UNSTABLE"));
			fprintf(stdout, "------------------------------------\n");
			fprintf(stdout, " SN    START  CURRENT  HISTORY   CONN\n");
			for (unsigned int i = 0; i < info.routes; i++) {
				fprintf(stdout, "%3u%9llu%9llu%9llu%7s\n", (i + 1),
						info.route[i].start, info.route[i].current,
						info.route[i].old,
						(info.route[i].connected ? "Y" : "N"));
			}

			{
				fprintf(stdout, "\n=======================================\n");
				fprintf(stdout, "Maximum Message Size (MTU): %u bytes\n",
						info.mtu);
				fprintf(stdout, "\n=======================================\n");

				fprintf(stdout, "RESOURCE INFO\n");
				fprintf(stdout, "-------------\n");
				fprintf(stdout, "Connections (%%): [%u/%u] %.2f\n",
						info.resource.connections, info.resource.maxConnections,
						(((double) info.resource.connections) * 100
								/ info.resource.maxConnections));
				fprintf(stdout, "Messages (%%):    [%u/%u] %.2f\n",
						info.resource.messages, info.resource.maxMessages,
						(((double) info.resource.messages) * 100
								/ info.resource.maxMessages));

				fprintf(stdout, "Uptime: ");
				if ((uint64_t) info.stat.uptime < 60) {
					fprintf(stdout, "%.2fs", info.stat.uptime);
				} else if ((uint64_t) info.stat.uptime < 3600) {
					fprintf(stdout, "%.2fmin", (info.stat.uptime / 60));
				} else {
					fprintf(stdout, "%.2fhrs", (info.stat.uptime / 3600));
				}

				fprintf(stdout, "\n\nPERFORMANCE DATA\n");
				fprintf(stdout, "----------------\n");
				fprintf(stdout, "Incoming (Packets/s): %12llu\n",
						(unsigned long long) (info.stat.receivedPackets
								/ info.stat.uptime));
				fprintf(stdout, "Incoming (KB/s):      %12llu\n",
						(unsigned long long) (info.stat.receivedBytes
								/ info.stat.uptime / 1024));
				fprintf(stdout, "Outgoing (Packets/s): %12llu\n",
						(unsigned long long) ((info.stat.receivedPackets
								- info.stat.droppedPackets) / info.stat.uptime));
				fprintf(stdout, "Outgoing (KB/s):      %12llu\n",
						(unsigned long long) ((info.stat.receivedBytes
								- info.stat.droppedBytes) / info.stat.uptime
								/ 1024));
				fprintf(stdout, "Loss (Packets):       %12.2f%%\n",
						((((double) (info.stat.droppedPackets))
								/ info.stat.receivedPackets) * 100));
				fprintf(stdout, "Loss (KB):            %12.2f%%\n",
						((((double) info.stat.droppedBytes)
								/ info.stat.receivedBytes) * 100));
			}
		} else {
			std::cout << "DESCRIBE FAILED" << std::endl;
		}
	} catch (BaseException &e) {
		std::cout << "DESCRIBE FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::registerCmd() {
	std::cout << "CMD: [REGISTER]" << std::endl;
	uint64_t id = destinationId;
	uint64_t newId = 0;
	std::cout << "Enter identifier: ";
	std::cin >> newId;
	if (CommandLine::inputError()) {
		return;
	}

	try {
		Digest hc;
		Random().bytes(&hc, Hash::SIZE);
		if (getKeyRequest(id, &hc, verifyHost())
				&& registerRequest(id, newId, &hc)) {
			std::cout << "REGISTER SUCCEEDED FOR ID: " << newId << std::endl;
			//Message source will be set to this
			setSource(newId);
		} else {
			std::cout << "REGISTER FAILED FOR ID: " << newId << std::endl;
		}
	} catch (BaseException &e) {
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
		if (getKeyRequest(id, &hc, verifyHost())) {
			EncodedDigest encodedKey;
			unsigned int encLen = Hash::encode(&hc, &encodedKey);
			std::cout << "GETKEY RETURNED: " << "[" << encLen << "] "
					<< encodedKey << std::endl;
		} else {
			std::cout << "GETKEY FAILED" << std::endl;
		}
	} catch (BaseException &e) {
		std::cout << "GETKEY FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::findRoot() {
	std::cout << "CMD: [FINDROOT]" << std::endl;
	uint64_t startNode = destinationId;
	uint64_t id = 0;
	std::cout << "Enter identifier: ";
	std::cin >> id;
	if (CommandLine::inputError()) {
		return;
	}

	try {
		uint64_t successorId = 0;
		if (findRootRequest(startNode, id, successorId)) {
			std::cout << "FINDROOT SUCCEEDED FOR ID: " << id << " WITH VALUE: "
					<< successorId << std::endl;
		} else {
			std::cout << "FINDROOT FAILED" << std::endl;
		}
	} catch (BaseException &e) {
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
	} catch (BaseException &e) {
		std::cout << "BOOTSTRAP FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::publishCmd() {
	std::cout << "CMD: [PUBLISH]" << std::endl;
	uint64_t id = destinationId;

	unsigned int topic = 0;
	std::cout << "Enter topic [0-255]: ";
	std::cin >> topic;
	if (CommandLine::inputError()) {
		return;
	}

	if (topic > Topic::MAX_ID) {
		std::cout << "Invalid topic" << std::endl;
		return;
	}

	char s[128];
	std::cout << "Enter message (max 100 characters): ";
	std::cin.ignore();
	std::cin.getline(s, sizeof(s));
	if (CommandLine::inputError()) {
		return;
	}

	try {
		if (publishRequest(id, topic, (const unsigned char*) s, strlen(s))) {
			std::cout << "PUBLISH SUCCEEDED" << std::endl;
		} else {
			std::cout << "PUBLISH FAILED" << std::endl;
		}
	} catch (BaseException &e) {
		std::cout << "PUBLISH FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::subscribeCmd() {
	std::cout << "CMD: [SUBSCRIBE]" << std::endl;
	uint64_t id = destinationId;
	unsigned int topic = 0;
	std::cout << "Enter topic [0-255]: ";
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
	} catch (BaseException &e) {
		std::cout << "SUBSCRIBE FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::unsubscribeCmd() {
	std::cout << "CMD: [UNSUBSCRIBE]" << std::endl;
	uint64_t id = destinationId;
	unsigned int topic = 0;
	std::cout << "Enter topic [0-255]: ";
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
	} catch (BaseException &e) {
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
	} catch (BaseException &e) {
		std::cout << "GETPREDECESSOR FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::setPredecessorCmd() {
	std::cout << "CMD: [SETPREDECESSOR]" << std::endl;
	uint64_t id = destinationId;
	uint64_t newPredecessor = 0;
	std::cout << "Enter predecessor's identifier: ";
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
	} catch (BaseException &e) {
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
	} catch (BaseException &e) {
		std::cout << "GETSUCCESSOR FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::setSuccessorCmd() {
	std::cout << "CMD: [SETSUCCESSOR]" << std::endl;
	uint64_t id = destinationId;
	uint64_t newSuccessor = 0;
	std::cout << "Enter successor's identifier: ";
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
	} catch (BaseException &e) {
		std::cout << "SETSUCCESSOR FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::getFingerCmd() {
	std::cout << "CMD: [GETFINGER]" << std::endl;
	uint64_t id = destinationId;
	uint32_t index = 0;
	std::cout << "Enter index: ";
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
	} catch (BaseException &e) {
		std::cout << "GETFINGER FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::setFingerCmd() {
	std::cout << "CMD: [SETFINGER]" << std::endl;
	uint64_t id = destinationId;
	uint32_t index = 0;
	uint64_t finger = 0;

	std::cout << "Enter index: ";
	std::cin >> index;
	if (CommandLine::inputError()) {
		return;
	}

	std::cout << "Enter identifier: ";
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
	} catch (BaseException &e) {
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
	} catch (BaseException &e) {
		std::cout << "GETNEIGHBOURS FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::notifyCmd() {
	std::cout << "CMD: [NOTIFY]" << std::endl;
	uint64_t id = destinationId;
	uint64_t newPredecessor = 0;
	std::cout << "Enter predecessor's identifier: ";
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
	} catch (BaseException &e) {
		std::cout << "NOTIFY FAILED" << std::endl;
		throw;
	}
}

void OverlayTool::findSuccessorCmd() {
	std::cout << "CMD: [FINDSUCCESSOR]" << std::endl;
	uint64_t startNode = destinationId;
	uint64_t id = 0;
	std::cout << "Enter identifier: ";
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
	} catch (BaseException &e) {
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
	} catch (BaseException &e) {
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
	} catch (BaseException &e) {
		std::cout << "MAP FAILED" << std::endl;
		throw;
	}
}

} /* namespace wanhive */

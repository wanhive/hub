/*
 * NetworkTest.cpp
 *
 * For stress testing of the Wanhive hubs
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "NetworkTest.h"
#include "../../base/common/BaseException.h"
#include "../../base/common/CommandLine.h"
#include "../../base/common/Logger.h"
#include "../../base/Thread.h"
#include "../../base/Timer.h"
#include <cstring>
#include <iostream>

#define WH_64_BYTE_MSG   "LZXCVBNMQWERTYUIOP##@@@@@@@@@@"
#define WH_128_BYTE_MSG  "LZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##@@@@@@@@@@@@@@@@@@"
#define WH_256_BYTE_MSG  "LZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##@@@@@@"
#define WH_512_BYTE_MSG  "LZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##@@@@@@@@@@"
#define WH_1024_BYTE_MSG "LZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##@@@@@@@@@@@@@@@@@@"
#define WH_2048_BYTE_MSG "LZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##@@@@@@"
#define WH_4096_BYTE_MSG "LZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##ASDFGHJKLZXCVBNMQWERTYUIOP##@@@@@@@@@@"

namespace wanhive {

NetworkTest::NetworkTest(uint64_t clientId, uint64_t serverId,
		const char *path) noexcept :
		Agent(clientId, path) {
	this->serverId = serverId;
	destinationId = clientId;
	iterations = 0;
	nSent = 0;
	nReceived = 0;
	msgLen = Message::MTU;
	WH_LOG_ALERT("Message length (bytes): %u", msgLen);
}

NetworkTest::~NetworkTest() {

}

void NetworkTest::test(const char *path) noexcept {
	std::cout << "Select an option\n" << "1. Flood test\n" << "2. Echo test\n"
			<< "::";
	int option = 0;
	std::cin >> option;

	if (CommandLine::inputError()) {
		return;
	}

	try {
		uint64_t serverId, clientId, destinationId;
		unsigned int iterations;

		switch (option) {
		case 1:
			std::cout << "Host ID (Bootstrap): " << std::endl;
			std::cin >> serverId;
			std::cout << "Source ID: " << std::endl;
			std::cin >> clientId;
			std::cout << "Destination ID (can be Source ID): " << std::endl;
			std::cin >> destinationId;
			std::cout << "Iterations (0 for infinite): " << std::endl;
			std::cin >> iterations;
			{
				NetworkTest nt(clientId, serverId, path);
				nt.flood(destinationId, iterations);
			}
			break;
		case 2:
			std::cout << "Host ID (Bootstrap): " << std::endl;
			std::cin >> serverId;
			std::cout << "Client ID: ";
			std::cin >> clientId;
			{
				NetworkTest nt(clientId, serverId, path);
				nt.echo();
			}
			break;
		default:
			std::cout << "Unknown option" << std::endl;
			break;
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
	} catch (...) {
		WH_LOG_EXCEPTION_U();
	}
}

void NetworkTest::flood(uint64_t destination, unsigned int iterations) {
	try {
		bootstrap(serverId, 5000);
		useKeyPair(NULL);
		this->iterations = iterations;
		this->destinationId = destination;
		std::cerr << "\n============START==============\n";
		Thread th(*this);
		Timer t;
		produce();
		th.join();
		std::cerr << "Elapsed time: " << t.elapsed() << " seconds" << std::endl;
		std::cerr << "Sent: " << nSent << ", Received: " << nReceived
				<< ", Loss: "
				<< ((double) (nSent - nReceived)) * 100 / ((double) nSent)
				<< "%" << std::endl;
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void NetworkTest::echo(unsigned int iterations) {
	try {
		bootstrap(serverId);
		//Disable timeout
		setSocketTimeout(0, 0);
		this->iterations = iterations;
		pong();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void NetworkTest::produce() noexcept {
	Packet out;
	memset(out.buffer(), 0, MTU);
	out.header().setAddress(getSource(), destinationId);
	out.header().setControl(msgLen, 0, 0);
	out.header().setContext(0, 0, 0);
	out.packHeader();
	unsigned int i = 0;
	try {
		auto ssl = getSecureSocket();
		if (ssl) {
			while (!iterations || i < iterations) {
				send(ssl, out);
				++i;
			}
		} else {
			auto sfd = getSocket();
			while (!iterations || i < iterations) {
				send(sfd, out);
				++i;
			}
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
	}
	std::cerr << "\n=============PACKETS SENT===========\n";
	nSent = i;
}

void NetworkTest::consume() noexcept {
	Packet in;
	unsigned int i = 0;
	try {
		auto ssl = getSecureSocket();
		if (ssl) {
			while (!iterations || i < iterations) {
				receive(ssl, in);
				i++;
			}
		} else {
			auto sfd = getSocket();
			while (!iterations || i < iterations) {
				receive(sfd, in);
				i++;
			}
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
	}
	nReceived = i;
}

void NetworkTest::pong() {
	unsigned int i = 0;
	while (!iterations || i < iterations) {
		Protocol::sendPong();
		i++;
	}
}

void NetworkTest::run(void *args) noexcept {
	consume();
}

} /* namespace wanhive */

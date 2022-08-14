/*
 * NetworkTest.h
 *
 * For stress testing of the Wanhive hubs
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_TEST_FLOOD_NETWORKTEST_H_
#define WH_TEST_FLOOD_NETWORKTEST_H_
#include "TestClient.h"
#include "../../base/common/Task.h"

namespace wanhive {
/**
 * Stress test of the overlay network
 */
class NetworkTest: private Task, private TestClient {
public:
	NetworkTest(uint64_t clientId, uint64_t serverId, const char *path) noexcept;
	~NetworkTest();
	//Run one of the available tests, <path> is the path to the configuration file
	static void test(const char *path) noexcept;
private:
	//Flood the destination and wait for it to echo back the packets
	void flood(uint64_t destination, unsigned int iterations = 0);
	//Echo the message back to the source
	void echo(unsigned int iterations = 0);
	void produce() noexcept;
	void consume() noexcept;
	void pong();
	//-----------------------------------------------------------------
	void run(void *arg) noexcept final;
	void setStatus(int status) noexcept final {

	}
	int getStatus() const noexcept final {
		return 0;
	}
private:
	uint64_t serverId;
	uint64_t destinationId;
	unsigned int iterations;
	unsigned int nSent;
	unsigned int nReceived;
	uint16_t msgLen;
	unsigned char iobuf[Message::MTU * 2];
};

} /* namespace wanhive */

#endif /* WH_TEST_FLOOD_NETWORKTEST_H_ */

/*
 * MulticastConsumer.h
 *
 * Multicast client example
 *
 *
 * Copyright (C) 2020 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_TEST_MULTICAST_MULTICASTCONSUMER_H_
#define WH_TEST_MULTICAST_MULTICASTCONSUMER_H_
#include "../../hub/ClientHub.h"

namespace wanhive {
/**
 * Multicast client, consumes multicast messages.
 * (For testing purpose only)
 * Thread safe at class level
 */
class MulticastConsumer: public ClientHub {
public:
	MulticastConsumer(unsigned long long uid, unsigned int topic,
			const char *path = nullptr) noexcept;
	virtual ~MulticastConsumer();
private:
	void stop(Watcher *w) noexcept override final;
	void cleanup() noexcept override final;
	void route(Message *message) noexcept override final;
	void maintain() noexcept override final;
	//-----------------------------------------------------------------
	//Process message published to a subscribed topic
	void processMulticastMessage(const Message *msg) noexcept;
	//Process everything other than a pub-sub message
	void process(Message *message) noexcept;
	//-----------------------------------------------------------------
	void processSubscribeResponse(const Message *msg) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Helper functions
	 */
	void handleInvalidMessage(const Message *msg) noexcept;
	//Sends subscription request to the server
	void subscribe(unsigned int topic) noexcept;
public:
	static constexpr unsigned int TOPICS = Topic::COUNT;
private:
	Timer timer;
	unsigned int topic;
	bool subscribed;
};

} /* namespace wanhive */

#endif /* WH_TEST_MULTICAST_MULTICASTCONSUMER_H_ */

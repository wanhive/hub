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
#include "../../edge/Receiver.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Multicast client, consumes multicast messages.
 */
class MulticastConsumer final: public Receiver {
public:
	MulticastConsumer(unsigned long long uid, unsigned int topic,
			const char *path = nullptr) noexcept;
	~MulticastConsumer();
private:
	void configure(void *arg) override;
	void cleanup() noexcept override;
	void route(Message *message) noexcept override;
	void maintain() noexcept override;
	//-----------------------------------------------------------------
	void process(Message *message) noexcept;
	void print(const Message *msg) noexcept;
	void subscribe(unsigned int delay) noexcept;
	void subscribe(const Message *msg) noexcept;
	void discard(const Message *msg) noexcept;
private:
	Timer timer;
	unsigned int topic;
};

} /* namespace wanhive */

#endif /* WH_TEST_MULTICAST_MULTICASTCONSUMER_H_ */

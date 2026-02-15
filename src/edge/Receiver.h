/**
 * @file Receiver.h
 *
 * Receiver Hub
 *
 *
 * Copyright (C) 2026 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_EDGE_RECEIVER_H_
#define WH_EDGE_RECEIVER_H_
#include "Monitor.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Receiver Hub
 */
class Receiver: public Monitor {
public:
	/**
	 * Constructor: creates a new receiver hub.
	 * @param uid unique identifier
	 * @param path configuration file's path
	 */
	Receiver(unsigned long long uid, const char *path = nullptr) noexcept;
	/**
	 * Destructor
	 */
	~Receiver();
protected:
	//-----------------------------------------------------------------
	void expel(Watcher *w) noexcept override;
	void configure(void *arg) override;
	void cleanup() noexcept override;
	void maintain() noexcept override;
	void route(Message *message) noexcept override;
	void onAlarm(unsigned long long uid, unsigned long long ticks) noexcept
			override;
	//-----------------------------------------------------------------
	/**
	 * Gets the topic identifier.
	 * @return topic id
	 */
	unsigned int channel() const noexcept;
	/**
	 * Gets the subscription status.
	 * @return true if subscribed, false otherwise
	 */
	bool subscribed() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Sends out a subscription request.
	 * @param delay delay (milliseconds) between subsequent calls
	 * @return true on success, false on error
	 */
	bool subscribe(unsigned int delay = 0) noexcept;
	/**
	 * Sends out a subscription cancellation request.
	 * @param delay delay (milliseconds) between subsequent calls
	 * @return true on success, false on error
	 */
	bool unsubscribe(unsigned int delay = 0) noexcept;
	/**
	 * Processes a subscription response.
	 * @param message subscription response
	 * @return true on success, false on error
	 */
	bool subscribe(const Message *message) noexcept;
	/**
	 * Processes a subscription cancellation response.
	 * @param message subscription cancellation response
	 * @return true on success, false on error
	 */
	bool unsubscribe(const Message *message) noexcept;
private:
	bool service(Message *message) noexcept;
	void subscribed(bool status) noexcept;
	void setup();
	void clear() noexcept;
private:
	Timer timer;
	struct {
		unsigned int interval;
		unsigned int channel;
		bool subscribed;
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_EDGE_RECEIVER_H_ */

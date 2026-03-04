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
 * @brief Receiver Hub
 * @details This hub accepts incoming messages for both end-to-end and
 * group communication.
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
	/**
	 * Gets the topic identifier.
	 * @return topic identifier
	 */
	unsigned int channel() const noexcept;
	/**
	 * Gets the subscription status.
	 * @return true if subscribed, false otherwise
	 */
	bool subscribed() const noexcept;
	/**
	 * Gets the multicast option flag.
	 * @return true if enabled, false if disabled
	 */
	bool multicast() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Initiates a subscription request.
	 * @param delay interval in milliseconds between consecutive calls
	 * @return true on success, false on error
	 */
	bool subscribe(unsigned int delay = 0) noexcept;
	/**
	 * Initiates a subscription cancellation request.
	 * @param delay interval in milliseconds between consecutive calls
	 * @return true on success, false on error
	 */
	bool unsubscribe(unsigned int delay = 0) noexcept;
	/**
	 * Processes a subscription response.
	 * @param message the response
	 * @return true on success, false on error
	 */
	bool subscribe(const Message *message) noexcept;
	/**
	 * Processes a subscription cancellation response.
	 * @param message the response
	 * @return true on success, false on error
	 */
	bool unsubscribe(const Message *message) noexcept;
	//-----------------------------------------------------------------
	void expel(Watcher *w) noexcept override;
	void configure(void *arg) override;
	void cleanup() noexcept override;
	void maintain() noexcept override;
	void route(Message *message) noexcept override;
	//-----------------------------------------------------------------
private:
	//-----------------------------------------------------------------
	void onAlarm(unsigned long long uid, unsigned long long ticks) noexcept
			override;
	/**
	 * Adapter: handles the session changes.
	 * @return true on success, false on error
	 */
	virtual bool onboard() noexcept;
	/**
	 * Adapter: processes the incoming messages.
	 * @param message incoming message
	 * @return true on success, false on error
	 */
	virtual bool service(Message *message) noexcept;
	//-----------------------------------------------------------------
	bool receive(Message *message) noexcept;
	void subscribed(bool status) noexcept;
	void setup();
	void clear() noexcept;
private:
	Timer timer;
	struct {
		unsigned int channel;
		bool subscribed;
		bool multicast;
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_EDGE_RECEIVER_H_ */

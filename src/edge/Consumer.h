/**
 * @file Consumer.h
 *
 * Consumer Hub
 *
 *
 * Copyright (C) 2026 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_EDGE_CONSUMER_H_
#define WH_EDGE_CONSUMER_H_
#include "Monitor.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Consumer Hub
 */
class Consumer: public Monitor {
public:
	/**
	 * Constructor: creates a new consumer hub.
	 * @param uid unique identifier
	 * @param path configuration file's path
	 */
	Consumer(unsigned long long uid, const char *path = nullptr) noexcept;
	/**
	 * Destructor
	 */
	~Consumer();
protected:
	//-----------------------------------------------------------------
	void expel(Watcher *w) noexcept override;
	void configure(void *arg) override;
	void cleanup() noexcept override;
	//-----------------------------------------------------------------
	/**
	 * Sends out a subscription request.
	 * @return true on success, false on error
	 */
	bool subscribe() noexcept;
	/**
	 * Sends out a subscription cancellation request.
	 * @return true on success, false on error
	 */
	bool unsubscribe() noexcept;
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
	/**
	 * Gets the topic identifier.
	 * @return topic id
	 */
	unsigned int topic() const noexcept;
	/**
	 * Gets the subscription status.
	 * @return true if subscribed, false otherwise
	 */
	bool subscribed() const noexcept;
private:
	void setup();
	void clear() noexcept;
private:
	struct {
		unsigned int topic;
		bool subscribed;
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_EDGE_CONSUMER_H_ */

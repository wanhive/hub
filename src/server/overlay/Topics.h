/*
 * Topics.h
 *
 * Topics management
 *
 *
 * Copyright (C) 2020 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_OVERLAY_TOPICS_H_
#define WH_SERVER_OVERLAY_TOPICS_H_
#include "../../base/ds/Array.h"
#include "../../base/ds/Khash.h"
#include "../../base/ds/Twiddler.h"
#include "../../hub/Topic.h"
#include "../../reactor/Watcher.h"

namespace wanhive {
/**
 * Subscription manager for overlay hubs
 * Trades off memory for time efficiency
 * Thread safe at class level
 */
class Topics {
public:
	Topics() noexcept;
	virtual ~Topics();
	//Associates the Watcher <w> with the <topic>
	bool put(unsigned int topic, const Watcher *w) noexcept;
	//Iterates over the list of Watchers associated with the <topic>
	Watcher* get(unsigned int topic, unsigned int index) const noexcept;
	//Unsubscribes the Watcher <w> from the <topic>
	void remove(unsigned int topic, const Watcher *w) noexcept;
	//Returns true if Watcher <w> is subscribed to the <topic>, false otherwise
	bool contains(unsigned int topic, const Watcher *w) const noexcept;
	//Returns the number of watchers subscribed to the <topic>
	unsigned int count(unsigned int topic) const noexcept;
	//Clears subscriptions without deallocating memory
	void clear() noexcept;
private:
	struct Key {
		const Watcher *w;
		unsigned int topic;
	};

	struct HFN {
		unsigned int operator()(const Key &key) const noexcept {
			return (Twiddler::mix((unsigned long long) key.w) + key.topic);
		}
	};

	struct EQFN {
		bool operator()(const Key &k1, const Key &k2) const noexcept {
			return ((k1.w == k2.w) && (k1.topic == k2.topic));
		}
	};

	//List of watchers subscribed to various topics
	Array<const Watcher*> topics[Topic::COUNT];
	//Index lookup table for fast insertion and deletion
	Khash<Key, unsigned int, true, HFN, EQFN> indexes;
};

} /* namespace wanhive */

#endif /* WH_SERVER_OVERLAY_TOPICS_H_ */

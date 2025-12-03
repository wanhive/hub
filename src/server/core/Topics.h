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

#ifndef WH_SERVER_CORE_TOPICS_H_
#define WH_SERVER_CORE_TOPICS_H_
#include "../../base/ds/Khash.h"
#include "../../base/ds/ReadyList.h"
#include "../../base/ds/Twiddler.h"
#include "../../hub/Topic.h"
#include "../../reactor/Watcher.h"

namespace wanhive {
/**
 * Topics manager
 */
class Topics {
public:
	/**
	 * Default constructor: initializes an empty object.
	 */
	Topics() noexcept;
	/**
	 * Destructor
	 */
	~Topics();
	/**
	 * Associates a watcher to the given topic.
	 * @param topic topic's identifier
	 * @param w watcher's pointer
	 * @return true on success, false on error (invalid topic or watcher)
	 */
	bool put(unsigned int topic, const Watcher *w) noexcept;
	/**
	 * Iterates over the list of watchers associated with the given topic.
	 * @param topic topic's identifier
	 * @param index list's index
	 * @return pointer to the watcher at the given index, nullptr on error
	 * (invalid topic/index).
	 */
	Watcher* get(unsigned int topic, unsigned int index) const noexcept;
	/**
	 * Dissociates a watcher from the given topic.
	 * @param topic topic's identifier
	 * @param w watcher's pointer
	 */
	void remove(unsigned int topic, const Watcher *w) noexcept;
	/**
	 * Checks whether a watcher is associated with a topic.
	 * @param topic topic's identifier
	 * @param w watcher's pointer
	 * @return true if an association exists, false otherwise
	 */
	bool contains(unsigned int topic, const Watcher *w) const noexcept;
	/**
	 * Returns the number of watchers associated with the given topic.
	 * @param topic topic's identifier
	 * @return subscriptions count for the given topic
	 */
	unsigned int count(unsigned int topic) const noexcept;
	/**
	 * Clears all associations (doesn't deallocate memory).
	 */
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

	//Lists of watchers organized by topics
	ReadyList<const Watcher*> topics[Topic::COUNT];
	//Index lookup table for fast insertion and deletion
	Kmap<Key, unsigned int, HFN, EQFN> indexes;
};

} /* namespace wanhive */

#endif /* WH_SERVER_CORE_TOPICS_H_ */

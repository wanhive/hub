/**
 * @file Topics.h
 *
 * Topics repository
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

/*! @namespace wanhive */
namespace wanhive {
/**
 * Topics repository
 */
class Topics {
public:
	/**
	 * Constructor: creates an empty repository.
	 */
	Topics() noexcept;
	/**
	 * Destructor
	 */
	~Topics();
	/**
	 * Associates a watcher to a topic.
	 * @param topic topic's identifier
	 * @param w watcher's pointer
	 * @return true on success, false on error
	 */
	bool put(unsigned int topic, const Watcher *w) noexcept;
	/**
	 * Goes through the list of watchers associated with a topic.
	 * @param topic topic's identifier
	 * @param index list's index
	 * @return watcher at the current index, nullptr on error
	 */
	Watcher* get(unsigned int topic, unsigned int index) const noexcept;
	/**
	 * Dissociates a watcher from a topic.
	 * @param topic topic's identifier
	 * @param w watcher's pointer
	 */
	void remove(unsigned int topic, const Watcher *w) noexcept;
	/**
	 * Checks a watcher's association with a topic.
	 * @param topic topic's identifier
	 * @param w watcher's pointer
	 * @return true if associated, false otherwise
	 */
	bool contains(unsigned int topic, const Watcher *w) const noexcept;
	/**
	 * Returns the number of watchers associated with a topic.
	 * @param topic topic's identifier
	 * @return association count
	 */
	unsigned int count(unsigned int topic) const noexcept;
	/**
	 * Clears all associations.
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

	ReadyList<const Watcher*> topics[Topic::COUNT];
	Kmap<Key, unsigned int, HFN, EQFN> indexes;
};

} /* namespace wanhive */

#endif /* WH_SERVER_CORE_TOPICS_H_ */

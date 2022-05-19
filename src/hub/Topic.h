/*
 * Topic.h
 *
 * Topic-based subscription management
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_TOPIC_H_
#define WH_HUB_TOPIC_H_

namespace wanhive {
/**
 * Topic-based subscription management
 * Thread safe at class level
 */
class Topic {
public:
	/**
	 * Default constructor: all the topics are un-subscribed
	 */
	Topic() noexcept;
	/**
	 * Destructor
	 */
	~Topic();
	//-----------------------------------------------------------------
	/**
	 * Adds subscription to the given topic.
	 * @param id the topic identifier
	 * @return true on success, false otherwise (invalid topic identifier)
	 */
	bool set(unsigned int id) noexcept;
	/**
	 * Removes subscription from the given topic.
	 * @param id the topic identifier
	 */
	void clear(unsigned int id) noexcept;
	/**
	 * Tests subscription to the given topic.
	 * @param id the topic identifier
	 * @return true if a subscription exists, false if a subscription doesn't
	 * exist, or the topic identifier is invalid.
	 */
	bool test(unsigned int id) const noexcept;
	/**
	 * Returns the number of subscribed topics.
	 * @return the total subscriptions count
	 */
	unsigned int count() const noexcept;
public:
	/** Maximum number of topics */
	static constexpr unsigned int COUNT = 256;
	/** Minimum topic identifier */
	static constexpr unsigned int MIN_ID = 0;
	/** Maximum topic identifier */
	static constexpr unsigned int MAX_ID = 255;
private:
	unsigned int n;
	unsigned char map[(COUNT + 7) / 8];
};

} /* namespace wanhive */

#endif /* WH_HUB_TOPIC_H_ */

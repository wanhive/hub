/**
 * @file Topic.h
 *
 * Topic subscriptions
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_TOPIC_H_
#define WH_HUB_TOPIC_H_

/*! @namespace wanhive */
namespace wanhive {
/**
 * Topic subscriptions
 */
class Topic {
public:
	/**
	 * Constructor: clears all subscriptions.
	 */
	Topic() noexcept;
	/**
	 * Destructor
	 */
	~Topic();
	//-----------------------------------------------------------------
	/**
	 * Adds subscription to a topic.
	 * @param id topic identifier
	 * @return true on success, false on error (invalid topic)
	 */
	bool set(unsigned int id) noexcept;
	/**
	 * Clears subscription to a topic.
	 * @param id topic identifier
	 */
	void clear(unsigned int id) noexcept;
	/**
	 * Tests subscription to a topic.
	 * @param id topic identifier
	 * @return true if subscription exists, false otherwise
	 */
	bool test(unsigned int id) const noexcept;
	/**
	 * Returns the number of subscribed topics.
	 * @return subscription count
	 */
	unsigned int count() const noexcept;
public:
	/*! Maximum number of topics */
	static constexpr unsigned int COUNT = 256;
	/*! Minimum topic identifier */
	static constexpr unsigned int MIN_ID = 0;
	/*! Maximum topic identifier */
	static constexpr unsigned int MAX_ID = 255;
private:
	unsigned int _count { };
	unsigned char bits[(COUNT + 7) / 8] { };
};

} /* namespace wanhive */

#endif /* WH_HUB_TOPIC_H_ */

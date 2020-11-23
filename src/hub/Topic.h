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
	Topic() noexcept;
	~Topic();

	//Subscribe to topic <id>
	bool set(unsigned int id) noexcept;
	//Unsubscribe from topic <id>
	void clear(unsigned int id) noexcept;
	//Test subscription to topic <id>
	bool test(unsigned int id) const noexcept;
	//Total subscriptions count
	unsigned int count() const noexcept;
public:
	//Max number of topics
	static constexpr unsigned int COUNT = 256;
	//Minimum topic identifier
	static constexpr unsigned int MIN_ID = 0;
	//Maximum topic identifier
	static constexpr unsigned int MAX_ID = 255;
private:
	unsigned int n;
	unsigned char map[(COUNT + 7) / 8];
};

} /* namespace wanhive */

#endif /* WH_HUB_TOPIC_H_ */

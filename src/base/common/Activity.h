/*
 * Activity.h
 *
 * Base class for activities
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_ACTIVITY_H_
#define WH_BASE_COMMON_ACTIVITY_H_

namespace wanhive {
/**
 * Base class for activities
 */
class Activity {
public:
	virtual ~Activity() = default;
	/**
	 * Checks the ability to carry on with an activity.
	 * @return true if possible, false otherwise
	 */
	virtual bool doable() const noexcept = 0;
	/**
	 * Conducts an activity.
	 * @param arg additional argument
	 */
	virtual void act(void *arg) noexcept = 0;
	/**
	 * Puts a stop or end to an activity.
	 */
	virtual void cease() noexcept = 0;
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_ACTIVITY_H_ */

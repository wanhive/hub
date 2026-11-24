/**
 * @file NonCopyable.h
 *
 * Non-copyable class
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_NONCOPYABLE_H_
#define WH_BASE_COMMON_NONCOPYABLE_H_

/*! @namespace wanhive */
namespace wanhive {
/**
 * @brief A class that cannot be copied or moved
 * @details This class, along with its subclasses, cannot be copied, moved, or
 * assigned to another instance.
 */
class NonCopyable {
protected:
	constexpr NonCopyable() noexcept = default;
	~NonCopyable() = default;
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable(NonCopyable&&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
	NonCopyable& operator=(NonCopyable&&) = delete;
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_NONCOPYABLE_H_ */

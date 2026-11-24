/**
 * @file Handle.h
 *
 * Restricted data access
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_HANDLE_H_
#define WH_BASE_DS_HANDLE_H_
#include "../common/reflect.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * @brief Data Handle
 * @details Restricts access to the wrapped value.
 * @tparam F friend class type
 * @tparam X data type
 * @tparam VALUE default value
 */
template<typename F, typename X, X VALUE = X { }> class Handle final {
public:
	/**
	 * Default constructor: value-initializes the underlying object.
	 */
	constexpr Handle() noexcept = default;
	/**
	 * Constructor: assigns a given value.
	 * @param value the value
	 */
	constexpr Handle(const X &value) noexcept :
			value { value } {

	}
	/**
	 * Copy constructor
	 * @param w object to copy
	 */
	constexpr Handle(const Handle &w) noexcept = default;
	/**
	 * Destructor
	 */
	~Handle() = default;
	/**
	 * Assignment operator
	 * @param w object to assign
	 * @return reference to self
	 */
	constexpr Handle& operator=(const Handle &w) noexcept = default;
private:
	constexpr operator X() const noexcept {
		return value;
	}
private:
	WH_POD_ASSERT(X);
	X value { VALUE };
	friend F;
};

/**
 * File descriptor handle
 */
template<typename F>
using FileHandle = Handle<F, int, -1>;
//-----------------------------------------------------------------
/**
 * @brief A key class constructible only by a friend
 * @details This class can be used to implement the passkey idiom, restricting
 * access to specific member functions and constructors.
 * @tparam F friend class type
 */
template<typename F> class Badge final {
private:
	constexpr Badge() noexcept {

	}
	constexpr Badge(const Badge&) = default;
	constexpr Badge(Badge&&) = default;
	Badge& operator=(const Badge&) = delete;
	Badge& operator=(Badge&&) = delete;
	friend F;
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_HANDLE_H_ */

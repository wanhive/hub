/*
 * SignalSet.h
 *
 * Signal set operations
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_SIGNALSET_H_
#define WH_BASE_UNIX_SIGNALSET_H_
#include <signal.h>

namespace wanhive {
/**
 * Signal set operations
 * @ref sigsetops(3)
 */
class SignalSet {
public:
	/**
	 * Default constructor: zeroes out the signal set.
	 */
	SignalSet() noexcept;
	/**
	 * Constructor: initializes the signal set.
	 * @param fill true for full set, false for empty set
	 */
	SignalSet(bool fill);
	/**
	 * Destructor
	 */
	~SignalSet();
	//-----------------------------------------------------------------
	/**
	 * Empties the set (includes all signals).
	 */
	void empty();
	/**
	 * Fills up the set (excludes all signals).
	 */
	void fill();
	/**
	 * Adds a signal to the set.
	 * @param signum signal number
	 */
	void add(int signum);
	/**
	 * Deletes a signal from the set.
	 * @param signum signal number
	 */
	void remove(int signum);
	/**
	 * Tests a signal's membership in the set.
	 * @param signum signal number
	 * @return true if member, false otherwise
	 */
	bool test(int signum) const;
	//-----------------------------------------------------------------
	/**
	 * Returns pointer to the internal structure.
	 * @return pointer to internal sigset_t structure
	 */
	sigset_t* mask() noexcept;
	/**
	 * Returns constant pointer to the internal structure.
	 * @return pointer to internal sigset_t structure
	 */
	const sigset_t* mask() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Wrapper for sigemptyset(3): initializes an empty set.
	 * @param set the signal set
	 */
	static void empty(sigset_t &set);
	/**
	 * Wrapper for sigfillset(3): initializes a full set.
	 * @param set the signal set
	 */
	static void fill(sigset_t &set);
	/**
	 * Wrapper for sigaddset(3): adds a signal to the given set.
	 * @param set the signal set
	 * @param signum signal number
	 */
	static void add(sigset_t &set, int signum);
	/**
	 * Wrapper for sigdelset(3): deletes a signal from the given set.
	 * @param set the signal set
	 * @param signum signal number
	 */
	static void remove(sigset_t &set, int signum);
	/**
	 * Wrapper for sigismember(3): tests a signal's membership in the given set.
	 * @param set the signal set
	 * @param signum signal number
	 * @return true if member, false otherwise
	 */
	static bool test(const sigset_t &set, int signum);
private:
	sigset_t _set;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_SIGNALSET_H_ */

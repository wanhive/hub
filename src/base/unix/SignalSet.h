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
 * REF: sigsetops(3)
 */
class SignalSet {
public:
	SignalSet() noexcept;
	SignalSet(bool fill);
	~SignalSet();

	//Empties the set
	void empty();
	//Fills up the set
	void fill();
	//Adds <signum> to the set
	void add(int signum);
	//Deletes <signum> from the set
	void remove(int signum);
	//Tests whether <signum> is a member of the set
	bool test(int signum) const;

	//Direct pointer to the internal structure
	sigset_t* mask() noexcept;
	//Const pointer to the internal structure
	const sigset_t* mask() const noexcept;

	//Wrapper for sigemptyset(3): initializes an empty set
	static void empty(sigset_t &set);
	//Wrapper for sigfillset(3): initializes a full set
	static void fill(sigset_t &set);
	//Wrapper for sigaddset(3): adds <signum> to the set
	static void add(sigset_t &set, int signum);
	//Wrapper for sigdelset: deletes <signum> from the set
	static void remove(sigset_t &set, int signum);
	//Wrapper for sigismember(3): tests <signum>'s membership in the set
	static bool test(const sigset_t &set, int signum);
private:
	sigset_t _set;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_SIGNALSET_H_ */

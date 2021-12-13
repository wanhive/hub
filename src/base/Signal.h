/*
 * Signal.h
 *
 * Signal handling on Linux
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SIGNAL_H_
#define WH_BASE_SIGNAL_H_
#include <signal.h>

namespace wanhive {
/**
 * Linux signal abstraction
 * man (7) signal
 * All the methods are thread safe
 */
class Signal {
public:
	//Block a signal, will be delivered when unblocked
	static void block(int signum);
	//Unblock a signal
	static void unblock(int signum);
	//Block all signals
	static void blockAll();
	//Unblock all signals, will be delivered when unblocked
	static void unblockAll();
	//Specified signal should be ignored (handler set to SIG_IGN)
	static void ignore(int signum);
	/*
	 * Set a handler for the signal <signum>, if <handler> not specified then
	 * a dummy handler is installed. If <restart> is true then the SA_RESTART
	 * flag is set.
	 */
	static void handle(int signum, void (*handler)(int)=nullptr, bool restart =
			true);
	//Send signal to the **calling process**
	static void raise(int signum);
	//Suspend the process/thread until a signal arrives (wrapper for pause)
	static void pause() noexcept;

	/*
	 * Wait until timeout or until a signal becomes pending. Returns true if
	 * the call was interrupted by a signal, false otherwise.
	 * Uses sigtimedwait(2)
	 */
	static bool timedWait(unsigned int milliseconds);
	//Same as the above, but waits for a particular signal
	static bool timedwait(unsigned int milliseconds, int signum);

	/*
	 * Waits for a signal, doesn't timeout
	 * uses sigwaitinfo(2)
	 */
	static void wait();
	//Same as the above, but waits for a particular signal
	static void wait(int signum);

	//Dummy signal handler which does nothing
	static void dummyHandler(int signum) noexcept;

	//Empties the set
	static void empty(sigset_t *set);
	//Fills up the set
	static void fill(sigset_t *set);
	//Adds a signal to the set
	static void add(sigset_t *set, int signum);
	//Removes a signal from the set
	static void remove(sigset_t *set, int signum);
	//Tests membership of a signal in the set
	static bool isMember(const sigset_t *set, int signum);
	//Sets calling thread's signal mask
	static void setMask(sigset_t *set);
	//Wrapper for pthread_sigmask(3)
	static void threadMask(int how, const sigset_t *set, sigset_t *oldset);
	//Wrapper for sigaction(2)
	static void setAction(int signum, const struct sigaction *act,
			struct sigaction *oldact);
};

} /* namespace wanhive */

#endif /* WH_BASE_SIGNAL_H_ */

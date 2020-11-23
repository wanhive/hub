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
	//Specified signal should be ignored (will never be delivered)
	static void ignore(int signum);
	/*
	 * Set a handler for signal signum, if handler not specified then a dummy
	 * handler is installed. For default behavior pass on SIG_DFL as the handler.
	 * If <restart> is true then SA_RESTART flag is set.
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

	//set <fd> to -1 to create a new descriptor, returns the file descriptor
	static int openSignalfd(int fd, const sigset_t &mask,
			bool blocking = false);
	//Closes a signal file descriptor
	static int closeSignalfd(int fd) noexcept;

	//Dummy signal handler
	static void dummyHandler(int signum) noexcept;
	//POSIX-compliant sigchild handler
	static void sigchildHandler(int signum) noexcept;

	static void empty(sigset_t *set);
	static void fill(sigset_t *set);
	static void add(sigset_t *set, int signum);
	static void remove(sigset_t *set, int signum);
	static bool isMember(const sigset_t *set, int signum);
	static void setMask(sigset_t *set);
	//All child threads inherit the mask (how =
	static void threadMask(int how, const sigset_t *set, sigset_t *oldset);
	static void setAction(int signum, const struct sigaction *act,
			struct sigaction *oldact);
};

} /* namespace wanhive */

#endif /* WH_BASE_SIGNAL_H_ */

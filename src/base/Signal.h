/*
 * Signal.h
 *
 * Common signal handling routines
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
 * Common signal handling routines
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
	//Specified signal should have default behavior (handler set to SIG_DFL)
	static void reset(int signum);
	/*
	 * Install a handler for the signal <signum>, if <handler> is nullptr then
	 * a dummy handler is installed. If <restart> is true then the SA_RESTART
	 * flag is set.
	 */
	static void handle(int signum, void (*handler)(int)=nullptr, bool restart =
			true);
};

} /* namespace wanhive */

#endif /* WH_BASE_SIGNAL_H_ */

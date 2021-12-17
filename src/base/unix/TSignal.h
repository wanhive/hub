/*
 * TSignal.h
 *
 * Signal handling for posix threads
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_TSIGNAL_H_
#define WH_BASE_UNIX_TSIGNAL_H_
#include "SignalSet.h"
#include <pthread.h>

namespace wanhive {
/**
 * Signal handling for posix threads
 */
class TSignal {
public:
	//Sets signal mask
	static void setMask(int how, const SignalSet &ss);
	//Sets signal mask and returns the old one
	static void setMask(int how, const SignalSet &ss, SignalSet &old);
	//Returns the current signal mask
	static void getMask(SignalSet &ss);
	//Wrapper for pthread_sigmask(3): changes/fetches signal mask
	static void mask(int how, const sigset_t *set, sigset_t *old);

	//Sends signal to the calling thread
	static void raise(int signum);
	//Wrapper for pthread_kill(3): sends signal to a thread
	static void raise(pthread_t threadId, int signum);

	//Waits for a signal
	static int wait(const SignalSet &ss);
	//Wrapper for sigwait(3): waits for a signal
	static int wait(const sigset_t *set);

	//Wrapper for pthread_sigqueue(3): queues up a signal to a thread
	static void queue(pthread_t threadId, int signum, const sigval value);
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_TSIGNAL_H_ */

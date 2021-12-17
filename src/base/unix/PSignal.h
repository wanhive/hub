/*
 * PSignal.h
 *
 * Signal handling for processes
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_PSIGNAL_H_
#define WH_BASE_UNIX_PSIGNAL_H_
#include "SignalSet.h"
#include <sys/types.h>

namespace wanhive {
/**
 * Signal handling for processes
 */
class PSignal {
public:
	//Sends signal to the calling process
	static void raise(int signum);
	//Wrapper for kill(2): sends signal to a process
	static void raise(pid_t pid, int signum);
	//Wrapper for alarm(2): sets an alarm clock
	static unsigned int alarm(unsigned int seconds) noexcept;
	//Wrapper for pause(2): waits for signal
	static void pause() noexcept;

	//Sets signal mask
	static void setMask(int how, const SignalSet &ss);
	//Sets signal mask and returns the old mask
	static void setMask(int how, const SignalSet &ss, SignalSet &old);
	//Returns the current signal mask
	static void getMask(SignalSet &ss);
	//Wrapper for sigprocmask(2): changes/fetches the signal mask
	static void mask(int how, const sigset_t *set, sigset_t *old);

	//Examines pending signals
	static void pending(SignalSet &ss);
	//Wrapper for sigpending(2): examins pending signals
	static void pending(sigset_t *set);

	//Wrapper for sigsuspend(2): waits for a signal
	static void wait(const SignalSet &ss);
	//Wrapper for sigsuspend(2): waits for a signal
	static void wait(const sigset_t *set);

	//Wrapper for sigqueue(2): queues up a signal to a process
	static void queue(pid_t pid, int signum, const sigval value);
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_PSIGNAL_H_ */

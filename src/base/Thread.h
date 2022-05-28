/*
 * Thread.h
 *
 * Lightweight thread implementation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_THREAD_H_
#define WH_BASE_THREAD_H_
#include "unix/PThread.h"

namespace wanhive {
/**
 * Lightweight thread implementation
 * Thread safe at class level
 */
class Thread: private PThread {
public:
	/**
	 * Constructor: creates and executes a posix thread.
	 * @param task the task to execute
	 * @param arg additional argument for the task (see Task::run())
	 */
	Thread(Task &task, void *arg = nullptr);
	/**
	 * Destructor
	 */
	~Thread();
	using PThread::join;
	using PThread::getStatus;
	using PThread::setStatus;
};

} /* namespace wanhive */

#endif /* WH_BASE_THREAD_H_ */

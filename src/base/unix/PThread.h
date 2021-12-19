/*
 * PThread.h
 *
 * Posix thread abstraction
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_PTHREAD_H_
#define WH_BASE_UNIX_PTHREAD_H_
#include "../common/Task.h"
#include <pthread.h>

namespace wanhive {
/**
 * Posix thread abstraction
 * REF: pthreads(7)
 */
class PThread {
public:
	//Creates a new thread
	PThread(Task &task, void *arg, bool detached);
	~PThread();

	//Wrapper for pthread_join(3): waits for the thread to terminate
	void* join();
	//Wrapper for pthread_exit(3): terminates the calling thread
	static void exit(void *arg);
	//Returns thread ID associated with this object
	pthread_t getId() const noexcept;
	//Returns task's status
	int getStatus() const noexcept;
	//Sets task's status
	void setStatus(int status) noexcept;
	//Wrapper for pthread_self(3): returns calling thread's ID
	static pthread_t self() noexcept;
	//Wrapper for pthread_equal(3): compares thread IDs
	static bool equal(pthread_t t1, pthread_t t2) noexcept;
private:
	void start();
	void run(void *arg) noexcept;
	static void* routine(void *arg);
private:
	pthread_t tid; //Thread ID
	Task &task;
	void *arg;
	const bool detached;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_PTHREAD_H_ */

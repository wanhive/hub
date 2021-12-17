/*
 * Thread.cpp
 *
 * Concurrent task execution, uses Linux's pthread
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Thread.h"
#include "common/Exception.h"
#include "common/defines.h"
#include "unix/SystemException.h"
#include <cstdio>
#include <unistd.h>

namespace wanhive {
Thread::Thread() noexcept :
		task(nullptr), arg(nullptr) {
	clear();
}

Thread::Thread(Task *target) noexcept :
		task(target), arg(nullptr) {
	clear();
}

Thread::~Thread() {

}

void Thread::run(void *arg) noexcept {
	if (task) {
		task->run(arg);
	}
}

int Thread::getStatus() const noexcept {
	if (task) {
		return task->getStatus();
	} else {
		return 0;
	}
}

void Thread::setStatus(int status) noexcept {
	if (task) {
		task->setStatus(status);
	}
}

void Thread::start(void *arg) {
	if (!alive) {
		this->arg = arg;
		auto s = pthread_create(&threadId, nullptr, Thread::entryPoint, this);
		if (!s) {
			alive = true;
		} else {
			throw SystemException(s);
		}
	} else {
		throw Exception(EX_INVALIDSTATE);
	}
}

void Thread::detach() {
	auto s = pthread_detach(threadId);
	if (!s) {
		clear();
	} else {
		throw SystemException(s);
	}
}

void Thread::join() {
	auto s = pthread_join(threadId, nullptr);
	if (!s) {
		clear();
	} else {
		throw SystemException(s);
	}
}

bool Thread::isAlive() const noexcept {
	return alive;
}

void Thread::signal(int signum) {
	auto error = pthread_kill(threadId, signum);
	if (error) {
		throw SystemException(error);
	}
}

void Thread::interrupt(int signum) noexcept {
	pthread_kill(threadId, signum);
}

void Thread::setAffinity(int cpuNumber) {
	cpu_set_t cpuSet;
	CPU_ZERO(&cpuSet);
	CPU_SET(cpuNumber, &cpuSet);
	auto s = pthread_setaffinity_np(threadId, sizeof(cpu_set_t), &cpuSet);
	if (s) {
		throw SystemException(s);
	}
}

void* Thread::getArgument() const noexcept {
	return arg;
}

Task* Thread::getTask() const noexcept {
	return task;
}

long Thread::getNumberOfCPUs() {
	// Getting number of CPUs
	auto cpus = sysconf(_SC_NPROCESSORS_ONLN);
	if (cpus < 0) {
		throw SystemException();
	}
	return cpus;
}

void* Thread::entryPoint(void *ptr) noexcept {
	Thread *th = (Thread*) ptr;
	th->run(th->getArgument());
	return nullptr;
}

void Thread::clear() noexcept {
	threadId = pthread_self();
	alive = false;
}

} /* namespace wanhive */

/*
 * PThread.cpp
 *
 * Posix thread
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "PThread.h"
#include "SystemException.h"
#include "../common/Exception.h"

namespace wanhive {

PThread::PThread(Task &task, void *arg, bool detached) :
		tid(self()), task(task), arg(arg), detached(detached) {
	start();
}

PThread::~PThread() {

}

void* PThread::join() {
	if (detached) {
		throw Exception(EX_OPERATION);
	} else {
		void *ret;
		auto status = ::pthread_join(tid, &ret);
		if (status == 0) {
			return ret;
		} else {
			throw SystemException(status);
		}
	}
}

void PThread::exit(void *arg) {
	::pthread_exit(arg);
}

pthread_t PThread::getId() const noexcept {
	return tid;
}

int PThread::getStatus() const noexcept {
	return task.getStatus();
}

void PThread::setStatus(int status) noexcept {
	task.setStatus(status);
}

pthread_t PThread::self() noexcept {
	return ::pthread_self();
}

bool PThread::equal(pthread_t t1, pthread_t t2) noexcept {
	return (::pthread_equal(t1, t2) != 0);
}

void PThread::start() {
	pthread_attr_t attr;
	auto rc = ::pthread_attr_init(&attr);
	if (rc != 0) {
		throw SystemException(rc);
	}

	if (detached) {
		rc = ::pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		if (rc != 0) {
			::pthread_attr_destroy(&attr);
			throw SystemException(rc);
		}
	}

	rc = ::pthread_create(&tid, &attr, PThread::routine, this);
	::pthread_attr_destroy(&attr);
	if (rc != 0) {
		throw SystemException(rc);
	}
}

void PThread::run(void *arg) noexcept {
	::pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr);
	task.run(arg);
}

void* PThread::routine(void *arg) {
	PThread *th = static_cast<PThread*>(arg);
	th->run(th->arg);
	return nullptr;
}

} /* namespace wanhive */

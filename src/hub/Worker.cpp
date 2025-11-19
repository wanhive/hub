/*
 * Worker.cpp
 *
 * Background task
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Worker.h"
#include "../base/common/Exception.h"
#include <cstdlib>

namespace wanhive {

Worker::Job::Job(Activity &action) noexcept :
		action { action } {
}

Worker::Job::~Job() {
}

void Worker::Job::run(void *arg) noexcept {
	action.act(arg);
}

int Worker::Job::getStatus() const noexcept {
	return 0;
}

void Worker::Job::setStatus(int status) noexcept {

}

Worker::Worker() noexcept :
		job { *this } {

}

Worker::~Worker() {
	if (thread) {
		abort();
	}
}

bool Worker::start(void *arg) {
	try {
		if (!doable()) {
			return false;
		} else if (!thread) {
			thread = new Thread(job, arg);
			return true;
		} else {
			throw Exception(EX_STATE);
		}
	} catch (const BaseException &e) {
		throw;
	} catch (...) {
		throw Exception(EX_MEMORY);
	}
}

void Worker::stop() {
	if (thread) {
		thread->join();
		delete thread;
		thread = nullptr;
		cease();
	}
}

bool Worker::doable() const noexcept {
	return false;
}

void Worker::act(void *arg) noexcept {
}

void Worker::cease() noexcept {
}

} /* namespace wanhive */


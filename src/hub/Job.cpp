/*
 * Job.cpp
 *
 * Asynchronous task
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Job.h"
#include "../base/common/Exception.h"
#include <cstdlib>

namespace wanhive {

Job::Runner::Runner(Activity &action) noexcept :
		action { action } {

}

Job::Runner::~Runner() {

}

void Job::Runner::run(void *arg) noexcept {
	action.act(arg);
}

int Job::Runner::getStatus() const noexcept {
	return 0;
}

void Job::Runner::setStatus(int status) noexcept {
}

}  // namespace wanhive

namespace wanhive {

Job::Job() noexcept :
		runner { *this } {

}

Job::~Job() {
	if (thread) {
		abort();
	}
}

bool Job::start(void *arg) {
	try {
		if (!doable()) {
			return false;
		} else if (!thread) {
			thread = new Thread(runner, arg);
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

void Job::stop() {
	if (thread) {
		thread->join();
		delete thread;
		thread = nullptr;
	}

	cease();
}

bool Job::doable() const noexcept {
	return false;
}

void Job::act(void *arg) noexcept {

}

void Job::cease() noexcept {

}

} /* namespace wanhive */

/*
 * Thread.cpp
 *
 * Lightweight thread implementation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Thread.h"

namespace wanhive {

Thread::Thread(Task &task, void *arg) :
		PThread(task, arg, false) {

}

Thread::~Thread() {

}

} /* namespace wanhive */

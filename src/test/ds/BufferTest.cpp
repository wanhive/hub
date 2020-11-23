/*
 * BufferTest.cpp
 *
 * CircularBuffer test routines
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "BufferTest.h"
#include <iostream>

namespace wanhive {

BufferTest::BufferTest() noexcept {

}

BufferTest::~BufferTest() {

}

void BufferTest::test() noexcept {
	try {
		buf.initialize(65536);
		Timer timer;
		Thread t(this);
		t.start();
		unsigned long i = 0;
		while (i <= LOOPS) {
			if (buf.put(i)) {
				i += 1;
			}
		}
		t.join();
		std::cout << "Thread test: " << LOOPS << " loops took "
				<< timer.elapsed() << " seconds" << std::endl;

		timer.now();
		vectorTest();
		std::cout << "Vector test: " << LOOPS << " loops took "
				<< timer.elapsed() << " seconds" << std::endl;
	} catch (...) {
		std::cout << "Tests failed due to exception" << std::endl;
	}
}

void BufferTest::vectorTest() noexcept {
	CircularBufferVector<char> vector;
	char a[16];
	unsigned long i = 0;

	for (; i < LOOPS; ++i) {
		a[0] = '\0';
		bufv.write((char*) "Wanhive", 7);
		bufv.read(a, 7);
		if (a[0] != 'W') {
			break;
		}
		bufv.write((char*) "Domain", 6);
		bufv.getReadable(vector);
		bufv.skipRead(6);
	}

	if (i != LOOPS) {
		std::cout << "Vector test FAILED" << std::endl;
	}
}

void BufferTest::run(void *arg) noexcept {
	unsigned long i = 0;
	unsigned long j = 0;
	while (i < LOOPS) {
		if (!buf.get(j)) {
			continue;
		} else if (j == i) {
			i++;
		} else {
			break;
		}
	}

	if (i != LOOPS) {
		std::cout << "Thread test FAILED" << std::endl;
	}
}

} /* namespace wanhive */

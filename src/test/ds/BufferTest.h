/*
 * BufferTest.h
 *
 * CircularBuffer test routines
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_TEST_DS_BUFFERTEST_H_
#define WH_TEST_DS_BUFFERTEST_H_
#include "../../base/Thread.h"
#include "../../base/Timer.h"
#include "../../base/ds/CircularBuffer.h"
#include "../../base/ds/StaticCircularBuffer.h"

namespace wanhive {

class BufferTest: private Task {
public:
	BufferTest() noexcept;
	~BufferTest();
	void test() noexcept;
private:
	void vectorTest() noexcept;
	void run(void *arg) noexcept final;
	void setStatus(int status) noexcept final {

	}
	int getStatus() const noexcept final {
		return 0;
	}
private:
	CircularBuffer<unsigned long, true> buf;
	StaticCircularBuffer<char, 16, false> bufv;
	static constexpr unsigned long LOOPS = 1024 * 1024 * 24;
};

} /* namespace wanhive */

#endif /* WH_TEST_DS_BUFFERTEST_H_ */

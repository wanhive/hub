/*
 * HashTableTest.h
 *
 * Hash table test routines
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_TEST_DS_HASHTABLETEST_H_
#define WH_TEST_DS_HASHTABLETEST_H_

namespace wanhive {

class HashTableTest {
public:
	HashTableTest(unsigned int count = 5000000) noexcept;
	~HashTableTest();
	void execute() noexcept;
private:
	void initData() noexcept;
	void destroyData() noexcept;
	void intTest() noexcept;
	void strTest() noexcept;
	void packedTest() noexcept;
private:
	unsigned int dataSize;
	unsigned *intData;
	char **strData;
};

} /* namespace wanhive */

#endif /* WH_TEST_DS_HASHTABLETEST_H_ */

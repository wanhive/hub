/*
 * HashTableTest.cpp
 *
 * Hash table test routines
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "HashTableTest.h"
#include "../../base/Timer.h"
#include "../../base/ds/Khash.h"
#include <climits>
#include <cstdio>
#include <cstdlib>

namespace wanhive {
struct packed_t {
	unsigned key;
	unsigned char val;
};

struct customhash {
	unsigned int operator()(const packed_t &key) const noexcept {
		return (unsigned int) key.key;
	}
};

struct customcmp {
	bool operator()(const packed_t &a, const packed_t &b) const noexcept {
		return a.key == b.key;
	}
};

HashTableTest::HashTableTest(unsigned int count) noexcept :
		dataSize(count) {
	initData();
}

HashTableTest::~HashTableTest() {
	destroyData();
}

void HashTableTest::execute() noexcept {
	Timer t;
	intTest();
	printf("%.3lf sec\n", t.elapsed());
	t.now();
	strTest();
	printf("%.3lf sec\n", t.elapsed());
	t.now();
	packedTest();
	printf("%.3lf sec\n", t.elapsed());
}

void HashTableTest::initData() noexcept {
	char buf[256];
	uint32_t x = 11;
	printf("Generating data [total %u]...\n", dataSize);
	intData = (unsigned*) calloc(dataSize, sizeof(unsigned));
	strData = (char**) calloc(dataSize, sizeof(char*));
	for (unsigned int i = 0; i < dataSize; ++i) {
		intData[i] = (unsigned) (dataSize * ((double) x / UINT_MAX) / 4)
				* 271828183u;
		sprintf(buf, "%x", intData[i]);
		strData[i] = strdup(buf);
		x = 1664525L * x + 1013904223L;
	}
	printf("Done!\n");
}

void HashTableTest::destroyData() noexcept {
	for (unsigned int i = 0; i < dataSize; ++i) {
		free(strData[i]);
	}
	free(strData);
	free(intData);
}

void HashTableTest::intTest() noexcept {
	Kmap<int, unsigned char> h;
	unsigned *data = intData;
	unsigned int collisions = 0;
	unsigned int insertions = 0;

	for (unsigned int i = 0; i < dataSize; ++i) {
		bool success = h.hmPut(data[i], i & 0xff);
		if (!success) {
			collisions++;
			h.removeKey(data[i]);
		} else {
			insertions++;
		}
	}
	printf(
			"[int, uchar] size: %u, collisions: %u, insertions: %u, capacity: %u\n",
			h.size(), collisions, insertions, h.capacity());
}

void HashTableTest::strTest() noexcept {
	Kset<const char*> h;
	char **data = strData;
	unsigned int collisions = 0;
	unsigned int insertions = 0;

	for (unsigned int i = 0; i < dataSize; ++i) {
		bool success = h.hsPut(data[i]);
		if (!success) {
			collisions++;
			h.removeKey(data[i]);
		} else {
			insertions++;
		}
	}
	printf("[string] size: %u, collisions: %u, insertions: %u, capacity: %u\n",
			h.size(), collisions, insertions, h.capacity());
}

void HashTableTest::packedTest() noexcept {
	Kset<packed_t, customhash, customcmp> h;
	unsigned *data = intData;
	unsigned int collisions = 0;
	unsigned int insertions = 0;

	for (unsigned int i = 0; i < dataSize; ++i) {
		packed_t x;
		x.key = data[i];
		x.val = i & 0xff;
		bool success = h.hsPut(x);
		if (!success) {
			collisions++;
			h.removeKey(x);
		} else {
			insertions++;
		}
	}
	printf(
			"[{int, uchar}] size: %u (sizeof=%zu), collisions: %u, insertions: %u, capacity: %u\n",
			h.size(), sizeof(packed_t), collisions, insertions, h.capacity());
}

} /* namespace wanhive */

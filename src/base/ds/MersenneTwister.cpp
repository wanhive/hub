/*
 * MersenneTwister.cpp
 *
 * MT19937 based 32-bit random number generator
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "MersenneTwister.h"

namespace wanhive {

MersenneTwister::MersenneTwister(unsigned long s) noexcept :
		index(N) {
	this->seed(s);
}

MersenneTwister::~MersenneTwister() {

}

void MersenneTwister::seed(unsigned long s) noexcept {
	mt[0] = s & MASK;

	for (unsigned int i = 1; i < N; ++i) {
		mt[i] = (F * (mt[i - 1] ^ (mt[i - 1] >> (W - 2))) + i);
		mt[i] &= MASK;
	}

	index = 0;
}

unsigned long MersenneTwister::next() noexcept {
	if (index == N) {
		twist();
	}

	auto y = mt[index++];
	y ^= (y >> U) & TMD;
	y ^= (y << S) & TMB;
	y ^= (y << T) & TMC;
	y ^= (y >> L);

	return y & MASK;
}

void MersenneTwister::twist() noexcept {
	for (unsigned int i = 0; i < N; ++i) {
		auto x = ((mt[i] & UBM) | (mt[(i + 1) % N] & LBM));
		auto y = (x & 0x1UL) ? ((x >> 1) ^ A) : (x >> 1);
		mt[i] = mt[(i + M) % N] ^ y;
	}

	index = 0;
}

} /* namespace wanhive */

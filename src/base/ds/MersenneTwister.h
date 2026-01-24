/*
 * MersenneTwister.h
 *
 * MT19937 based 32-bit random number generator
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_MERSENNETWISTER_H_
#define WH_BASE_DS_MERSENNETWISTER_H_

namespace wanhive {
/**
 * MT19937 (32-bit generator)
 * Not suitable for cryptographic application
 * @ref http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.c
 */
class MersenneTwister {
public:
	/**
	 * Constructor: creates a new random number generator.
	 * @param s value for seeding the generator
	 */
	MersenneTwister(unsigned long s = 5489) noexcept;
	/**
	 * Destructor
	 */
	~MersenneTwister();
	/**
	 * Applies a new seed to the generator.
	 * @param s the new seed's value
	 */
	void seed(unsigned long s) noexcept;
	/**
	 * Generates and returns a 32-bit random number.
	 * @return the random number
	 */
	unsigned long next() noexcept;
private:
	void twist() noexcept;
private:
	static constexpr unsigned long MASK = 0xffffffffUL;
	static constexpr unsigned int W = 32;
	static constexpr unsigned int N = 624;
	static constexpr unsigned int M = 397;
	static constexpr unsigned int R = 31;
	static constexpr unsigned long A = 0x9908b0dfUL;
	static constexpr unsigned long LBM = (1UL << R) - 1;
	static constexpr unsigned long UBM = MASK & (~LBM);
	static constexpr unsigned int U = 11;
	static constexpr unsigned int S = 7;
	static constexpr unsigned int T = 15;
	static constexpr unsigned int L = 18;
	static constexpr unsigned long TMD = 0xffffffffUL;
	static constexpr unsigned long TMB = 0x9d2c5680UL;
	static constexpr unsigned long TMC = 0xefc60000UL;
	static constexpr unsigned long F = 1812433253UL;

	unsigned long mt[N];
	unsigned int index;
};

} /* namespace wanhive */
#endif /* WH_BASE_DS_MERSENNETWISTER_H_ */

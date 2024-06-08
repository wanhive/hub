/*
 * TokenBucket.cpp
 *
 * Token Bucket
 *
 *
 * Copyright (C) 2024 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "TokenBucket.h"
#include <cstdint>

namespace {

constexpr unsigned long long TB_LIMIT = UINT64_MAX;

}  // namespace

namespace wanhive {

TokenBucket::TokenBucket() noexcept :
		Counter { TB_LIMIT }, active { false } {

}

TokenBucket::TokenBucket(unsigned long long tokens) noexcept :
		Counter { TB_LIMIT }, active { true } {
	Counter::setCount(tokens);
}

TokenBucket::~TokenBucket() noexcept {

}

void TokenBucket::fill(unsigned long long tokens) noexcept {
	Counter::setCount(tokens);
	active = true;
}

void TokenBucket::add(unsigned int count) noexcept {
	Counter::up(count);
	active = true;
}

bool TokenBucket::take(unsigned int count) noexcept {
	return !active || Counter::down(count);
}

} /* namespace wanhive */

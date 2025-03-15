/*
 * Tokens.cpp
 *
 * Token Bucket
 *
 *
 * Copyright (C) 2024 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Tokens.h"
#include <cstdint>

namespace {

constexpr unsigned long long TB_LIMIT = UINT64_MAX;

}  // namespace

namespace wanhive {

Tokens::Tokens() noexcept :
		Counter { TB_LIMIT }, active { false } {

}

Tokens::Tokens(unsigned int count) noexcept :
		Counter { TB_LIMIT }, active { true } {
	Counter::setCount(count);
}

Tokens::~Tokens() {

}

void Tokens::fill(unsigned int count) noexcept {
	Counter::setCount(count);
	active = true;
}

unsigned int Tokens::drain() noexcept {
	auto result = Counter::getCount();
	Counter::setCount(0);
	return result;
}

bool Tokens::take(unsigned int count) noexcept {
	return !active || Counter::down(count);
}

bool Tokens::add(unsigned int count) noexcept {
	if (Counter::up(count)) {
		active = true;
		return true;
	} else {
		return false;
	}
}

unsigned int Tokens::available() const noexcept {
	return Counter::getCount();
}

} /* namespace wanhive */

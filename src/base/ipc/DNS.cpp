/*
 * DNS.cpp
 *
 * Domain name resolution and reverse lookup
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "DNS.h"
#include "NetworkAddressException.h"
#include <cstring>

namespace wanhive {

DNS::DNS() noexcept {

}

DNS::DNS(const char *node, const char *service, const SocketTraits *traits) {
	lookup(node, service, traits);
}

DNS::~DNS() {
	clear();
}

void DNS::lookup(const char *node, const char *service,
		const SocketTraits *traits) {
	clear();
	addrinfo info, *hints;
	if (traits) {
		::memset(&info, 0, sizeof(info));
		info.ai_flags = traits->flags;
		info.ai_family = traits->domain;
		info.ai_socktype = traits->type;
		info.ai_protocol = traits->protocol;
		hints = &info;
	} else {
		hints = nullptr;
	}

	auto s = ::getaddrinfo(node, service, hints, &result);
	if (s == 0) {
		rewind();
	} else {
		throw NetworkAddressException(s);
	}
}

const addrinfo* DNS::next() noexcept {
	if (index) {
		auto ret = index;
		index = index->ai_next;
		return ret;
	} else {
		return nullptr;
	}
}

void DNS::rewind() noexcept {
	index = result;
}

void DNS::clear() noexcept {
	::freeaddrinfo(result);
	result = nullptr;
	index = nullptr;
}

void DNS::getTraits(const addrinfo *info, SocketTraits &traits) noexcept {
	if (info) {
		traits.domain = info->ai_family;
		traits.type = info->ai_socktype;
		traits.protocol = info->ai_protocol;
		traits.flags = 0;
	} else {
		::memset(&traits, 0, sizeof(traits));
	}
}

void DNS::getAddress(const addrinfo *info, SocketAddress &sa) noexcept {
	if (info) {
		::memcpy(&sa.address, info->ai_addr, info->ai_addrlen);
		sa.length = info->ai_addrlen;
	} else {
		::memset(&sa, 0, sizeof(sa));
	}
}

void DNS::reverse(const SocketAddress &sa, NameInfo &ni, int flags) {
	auto s = ::getnameinfo((const sockaddr*) &sa.address, sa.length, ni.host,
			sizeof(ni.host), ni.service, sizeof(ni.service), flags);
	if (s) {
		throw NetworkAddressException(s);
	}
}

} /* namespace wanhive */

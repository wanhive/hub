/*
 * OverlayHubInfo.cpp
 *
 * Overlay hub's runtime statistics
 *
 *
 * Copyright (C) 2022 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "OverlayHubInfo.h"
#include "../../base/ds/Serializer.h"
#include <cstdio>

namespace wanhive {

OverlayHubInfo::OverlayHubInfo() noexcept {

}

OverlayHubInfo::~OverlayHubInfo() {

}

void OverlayHubInfo::clear() noexcept {
	HubInfo::clear();
	predecessor = 0;
	successor = 0;
	routes = 0;
	stable = false;
}

unsigned long long OverlayHubInfo::getPredecessor() const noexcept {
	return predecessor;
}

void OverlayHubInfo::setPredecessor(unsigned long long predecessor) noexcept {
	this->predecessor = predecessor;
}

unsigned long long OverlayHubInfo::getSuccessor() const noexcept {
	return successor;
}

void OverlayHubInfo::setSuccessor(unsigned long long successor) noexcept {
	this->successor = successor;
}

unsigned int OverlayHubInfo::getRoutes() const noexcept {
	return routes;
}

void OverlayHubInfo::setRoutes(unsigned int routes) noexcept {
	this->routes = (routes <= DHT::KEY_LENGTH) ? routes : DHT::KEY_LENGTH;
}

bool OverlayHubInfo::isStable() const noexcept {
	return stable;
}

void OverlayHubInfo::setStable(bool stable) noexcept {
	this->stable = stable;
}

const RouteInfo* OverlayHubInfo::getRoute(unsigned int index) const noexcept {
	if (index < routes) {
		return &route[index];
	} else {
		return nullptr;
	}
}

void OverlayHubInfo::setRoute(const RouteInfo &table,
		unsigned int index) noexcept {
	if (index < routes) {
		route[index] = table;
	}
}

unsigned int OverlayHubInfo::pack(unsigned char *buffer,
		unsigned int size) const noexcept {
	if (size < MAX_BYTES) {
		return 0;
	}

	auto index = HubInfo::pack(buffer, size);
	Serializer::packi64(buffer + index, getPredecessor());
	index += sizeof(uint64_t);
	Serializer::packi64(buffer + index, getSuccessor());
	index += sizeof(uint64_t);
	Serializer::packi32(buffer + index, getRoutes());
	index += sizeof(uint32_t);
	Serializer::packi8(buffer + index, isStable() ? 1 : 0);
	index += sizeof(uint8_t);

	for (unsigned int i = 0; i < getRoutes(); ++i) {
		Serializer::packi64(buffer + index, getRoute(i)->start);
		index += sizeof(uint64_t);
		Serializer::packi64(buffer + index, getRoute(i)->current);
		index += sizeof(uint64_t);
		Serializer::packi64(buffer + index, getRoute(i)->old);
		index += sizeof(uint64_t);
		Serializer::packi8(buffer + index, getRoute(i)->connected ? 1 : 0);
		index += sizeof(uint8_t);
	}

	return index;
}

unsigned int OverlayHubInfo::unpack(const unsigned char *buffer,
		unsigned int size) noexcept {
	if (!buffer || size < MIN_BYTES) {
		return 0;
	}

	auto index = HubInfo::unpack(buffer, size);
	setPredecessor(Serializer::unpacku64(buffer + index));
	index += sizeof(uint64_t);
	setSuccessor(Serializer::unpacku64(buffer + index));
	index += sizeof(uint64_t);
	setRoutes(Serializer::unpacku32(buffer + index));
	index += sizeof(uint32_t);
	setStable(Serializer::unpacku8(buffer + index));
	index += sizeof(uint8_t);

	if (size < MIN_BYTES + (25 * getRoutes())) {
		return 0;
	}

	for (unsigned int i = 0; i < getRoutes(); ++i) {
		RouteInfo ri;
		ri.start = Serializer::unpacku64(buffer + index);
		index += sizeof(uint64_t);
		ri.current = Serializer::unpacku64(buffer + index);
		index += sizeof(uint64_t);
		ri.old = Serializer::unpacku64(buffer + index);
		index += sizeof(uint64_t);
		ri.connected = Serializer::unpacku8(buffer + index);
		index += sizeof(uint8_t);
		setRoute(ri, i);
	}

	return index;
}

void OverlayHubInfo::print() const noexcept {
	printf("\n------------------------------------------\n");
	HubInfo::print();
	printf("\n------------------------------------------\n");
	printf("PREDECESSOR: %llu, SUCCESSOR: %llu\n\n"
			"ROUTING TABLE [STABLE: %s]\n", getPredecessor(), getSuccessor(),
			WH_BOOLF(isStable()));
	printf(" SN    START  CURRENT  HISTORY   CONNECTED\n");

	for (unsigned int i = 0; i < getRoutes(); i++) {
		auto r = getRoute(i);
		printf("%3u%9llu%9llu%9llu%12s\n", (i + 1), r->start, r->current,
				r->old, WH_BOOLF(r->connected));
	}
	printf("\n------------------------------------------\n");
}

} /* namespace wanhive */

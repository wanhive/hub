/*
 * HubInfo.cpp
 *
 * Hub's runtime information
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "HubInfo.h"
#include "../base/ds/Serializer.h"
#include <cstdio>

namespace {

static constexpr unsigned int infoBytes() noexcept {
	return (6 * sizeof(uint64_t)) + (5 * sizeof(uint32_t));
}

}  // namespace

namespace wanhive {

HubInfo::HubInfo() noexcept {

}

HubInfo::~HubInfo() {

}

void HubInfo::clear() noexcept {
	uid = 0;
	uptime = 0;
	received = { 0, 0 };
	dropped = { 0, 0 };
	connections = { 0, 0 };
	messages = { 0, 0 };
	mtu = 0;
}

unsigned long long HubInfo::getUid() const noexcept {
	return uid;
}

void HubInfo::setUid(unsigned long long uid) noexcept {
	this->uid = uid;
}

double HubInfo::getUptime() const noexcept {
	return uptime;
}

void HubInfo::setUptime(double uptime) noexcept {
	this->uptime = uptime;
}

const TrafficInfo& HubInfo::getReceived() const noexcept {
	return received;
}

void HubInfo::setReceived(const TrafficInfo &received) noexcept {
	this->received = received;
}

const TrafficInfo& HubInfo::getDropped() const noexcept {
	return dropped;
}

void HubInfo::setDropped(const TrafficInfo &dropped) noexcept {
	this->dropped = dropped;
}

const ResourceInfo& HubInfo::getConnections() const noexcept {
	return connections;
}

void HubInfo::setConnections(const ResourceInfo &connections) noexcept {
	this->connections = connections;
}

const ResourceInfo& HubInfo::getMessages() const noexcept {
	return messages;
}

void HubInfo::setMessages(const ResourceInfo &messages) noexcept {
	this->messages = messages;
}

unsigned int HubInfo::getMTU() const noexcept {
	return mtu;
}

void HubInfo::setMTU(unsigned int mtu) noexcept {
	this->mtu = mtu;
}

unsigned int HubInfo::pack(unsigned char *buffer,
		unsigned int size) const noexcept {
	return Serializer::pack(buffer, size, "QgQQQQLLLLL", uid, uptime,
			received.units, received.bytes, dropped.units, dropped.bytes,
			connections.max, connections.used, messages.max, messages.used, mtu);
}

unsigned int HubInfo::unpack(const unsigned char *buffer,
		unsigned int size) noexcept {
	if (!buffer || size < infoBytes()) {
		return 0;
	}

	unsigned int index = 0;
	uid = Serializer::unpacku64(buffer + index);
	index += sizeof(uint64_t);
	uptime = Serializer::unpackf64(buffer + index);
	index += sizeof(uint64_t);

	received.units = Serializer::unpacku64(buffer + index);
	index += sizeof(uint64_t);
	received.bytes = Serializer::unpacku64(buffer + index);
	index += sizeof(uint64_t);
	dropped.units = Serializer::unpacku64(buffer + index);
	index += sizeof(uint64_t);
	dropped.bytes = Serializer::unpacku64(buffer + index);
	index += sizeof(uint64_t);

	connections.max = Serializer::unpacku32(buffer + index);
	index += sizeof(uint32_t);
	connections.used = Serializer::unpacku32(buffer + index);
	index += sizeof(uint32_t);
	messages.max = Serializer::unpacku32(buffer + index);
	index += sizeof(uint32_t);
	messages.used = Serializer::unpacku32(buffer + index);
	index += sizeof(uint32_t);

	mtu = Serializer::unpacku32(buffer + index);
	index += sizeof(uint32_t);
	return index;
}

void HubInfo::print() const noexcept {
	printf("KEY: %llu\n", uid);
	printf("Maximum Message Size (MTU): %u bytes\n\n", mtu);

	printf("RESOURCE USAGE\n");
	printf("--------------\n");
	printf("Connections (%%): [%u / %u]\n", connections.used, connections.max);
	printf("Messages (%%):    [%u / %u]\n", messages.used, messages.max);

	printf("\nPERFORMANCE DATA\n");
	printf("----------------\n");
	printf("Uptime: ");
	if ((uint64_t) uptime < 60) {
		printf("%.2fs\n", uptime);
	} else if ((uint64_t) uptime < 3600) {
		printf("%.2fmin\n", (uptime / 60));
	} else {
		printf("%.2fhr\n", (uptime / 3600));
	}
	printf("Incoming (packets): %20llu\n", received.units);
	printf("Incoming (KB):      %20llu\n", (received.bytes / 1024));
	printf("Outgoing (packets): %20llu\n", (received.units - dropped.units));
	printf("Outgoing (KB):      %20llu\n",
			((received.bytes - dropped.bytes) / 1024));
}

} /* namespace wanhive */

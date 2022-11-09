/*
 * MessageAddress.cpp
 *
 * Message's address
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "MessageAddress.h"
#include "../base/ds/Serializer.h"

namespace wanhive {

MessageAddress::MessageAddress() noexcept :
		label { }, source { }, destination { } {

}

MessageAddress::MessageAddress(uint64_t source, uint64_t destination,
		uint64_t label) noexcept :
		label { label }, source { source }, destination { destination } {

}

MessageAddress::MessageAddress(const unsigned char *data) noexcept {
	readAddress(data);
}

MessageAddress::~MessageAddress() {

}

uint64_t MessageAddress::getLabel() const noexcept {
	return label;
}

void MessageAddress::setLabel(uint64_t label) noexcept {
	this->label = label;
}

uint64_t MessageAddress::getSource() const noexcept {
	return source;
}

void MessageAddress::setSource(uint64_t source) noexcept {
	this->source = source;
}

uint64_t MessageAddress::getDestination() const noexcept {
	return destination;
}

void MessageAddress::setDestination(uint64_t destination) noexcept {
	this->destination = destination;
}

void MessageAddress::getAddress(uint64_t &source,
		uint64_t &destination) const noexcept {
	source = getSource();
	destination = getDestination();
}

void MessageAddress::setAddress(uint64_t source, uint64_t destination) noexcept {
	setSource(source);
	setDestination(destination);
}

void MessageAddress::readAddress(const unsigned char *data) noexcept {
	setLabel(readLabel(data));
	setSource(readSource(data));
	setDestination(readDestination(data));
}

void MessageAddress::writeAddress(unsigned char *data) const noexcept {
	writeLabel(data, getLabel());
	writeSource(data, getSource());
	writeDestination(data, getDestination());
}

uint64_t MessageAddress::readLabel(const unsigned char *data) noexcept {
	return Serializer::unpacku64(data);
}

void MessageAddress::writeLabel(unsigned char *data, uint64_t label) noexcept {
	Serializer::packi64(data, label);
}

uint64_t MessageAddress::readSource(const unsigned char *data) noexcept {
	return Serializer::unpacku64(data + 8);
}

void MessageAddress::writeSource(unsigned char *data, uint64_t source) noexcept {
	Serializer::packi64(data + 8, source);
}

uint64_t MessageAddress::readDestination(const unsigned char *data) noexcept {
	return Serializer::unpacku64(data + 16);
}

void MessageAddress::writeDestination(unsigned char *data,
		uint64_t destination) noexcept {
	Serializer::packi64(data + 16, destination);
}

} /* namespace wanhive */

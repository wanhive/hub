/*
 * Frame.cpp
 *
 * Message frame
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Frame.h"

namespace wanhive {

Frame::Frame() noexcept :
		origin { } {

}

Frame::Frame(uint64_t origin) noexcept :
		origin { origin } {

}

Frame::~Frame() {

}

uint64_t Frame::getOrigin() const noexcept {
	return origin;
}

MessageHeader& Frame::header() noexcept {
	return _header;
}

const MessageHeader& Frame::header() const noexcept {
	return _header;
}

unsigned char* Frame::buffer(unsigned int offset) noexcept {
	if (offset < MTU) {
		return (_frame.array() + offset);
	} else {
		return nullptr;
	}
}

const unsigned char* Frame::buffer(unsigned int offset) const noexcept {
	if (offset < MTU) {
		return (_frame.array() + offset);
	} else {
		return nullptr;
	}
}

unsigned char* Frame::payload(unsigned int offset) noexcept {
	if (offset < PAYLOAD_SIZE) {
		return (_frame.array() + HEADER_SIZE + offset);
	} else {
		return nullptr;
	}
}

const unsigned char* Frame::payload(unsigned int offset) const noexcept {
	if (offset < PAYLOAD_SIZE) {
		return (_frame.array() + HEADER_SIZE + offset);
	} else {
		return nullptr;
	}
}

void Frame::clear() noexcept {
	_header.clear();
	_frame.clear();
}

unsigned int Frame::getHops() const noexcept {
	return hops;
}

void Frame::setHops(unsigned int hops) noexcept {
	this->hops = hops;
}

unsigned int Frame::getLinks() const noexcept {
	return links;
}

void Frame::setLinks(unsigned int links) noexcept {
	this->links = links;
}

} /* namespace wanhive */

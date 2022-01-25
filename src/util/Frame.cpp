/*
 * Frame.cpp
 *
 * The message frame
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Frame.h"

namespace wanhive {

Frame::Frame(uint64_t origin) noexcept :
		hopCount { 0 }, referenceCount { 0 }, origin { origin } {
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

bool Frame::bind() noexcept {
	auto length = _header.getLength();
	if (testLength(length) && (_frame.getIndex() == 0)
			&& _frame.setLimit(length)) {
		MessageHeader::setLength(_frame.array(), length);
		return true;
	} else {
		return false;
	}
}

bool Frame::validate() const noexcept {
	return (_frame.getIndex() == 0)
			&& (_frame.getLimit() == _header.getLength())
			&& (_header.getLength() >= HEADER_SIZE);
}

bool Frame::testLength(unsigned int length) noexcept {
	return (length >= HEADER_SIZE && length <= MTU);
}

unsigned int Frame::packets(unsigned int bytes) noexcept {
	return ((unsigned long long) bytes + PAYLOAD_SIZE - 1) / PAYLOAD_SIZE;
}

unsigned int Frame::getHopCount() const noexcept {
	return hopCount;
}

void Frame::setHopCount(unsigned int hopCount) noexcept {
	this->hopCount = hopCount;
}

unsigned int Frame::getReferenceCount() const noexcept {
	return referenceCount;
}

void Frame::setReferenceCount(unsigned int referenceCount) noexcept {
	this->referenceCount = referenceCount;
}

} /* namespace wanhive */
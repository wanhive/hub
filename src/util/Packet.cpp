/*
 * Packet.cpp
 *
 * Message buffer for blocking IO
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Packet.h"
#include "../base/ds/Serializer.h"

namespace wanhive {

Packet::Packet() noexcept {

}

Packet::~Packet() {

}

MessageHeader& Packet::header() noexcept {
	return _header;
}

const MessageHeader& Packet::header() const noexcept {
	return _header;
}

unsigned char* Packet::buffer(unsigned int offset) noexcept {
	if (offset < Message::MTU) {
		return (_buffer + offset);
	} else {
		return nullptr;
	}
}

const unsigned char* Packet::buffer(unsigned int offset) const noexcept {
	if (offset < Message::MTU) {
		return (_buffer + offset);
	} else {
		return nullptr;
	}
}

unsigned char* Packet::payload(unsigned int offset) noexcept {
	if (offset < Message::PAYLOAD_SIZE) {
		return (_buffer + Message::HEADER_SIZE + offset);
	} else {
		return nullptr;
	}
}

const unsigned char* Packet::payload(unsigned int offset) const noexcept {
	if (offset < Message::PAYLOAD_SIZE) {
		return (_buffer + Message::HEADER_SIZE + offset);
	} else {
		return nullptr;
	}
}

} /* namespace wanhive */

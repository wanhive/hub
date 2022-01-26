/*
 * Packet.cpp
 *
 * The message packet
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Packet.h"

namespace wanhive {

Packet::Packet(uint64_t origin) noexcept :
		Frame { origin } {

}

Packet::~Packet() {

}

bool Packet::packHeader(const MessageHeader &header) noexcept {
	auto length = header.getLength();
	if (testLength(length) && (frame().getIndex() == 0)
			&& frame().setLimit(length)) {
		return header.serialize(frame().array());
	} else {
		return false;
	}
}

bool Packet::packHeader() noexcept {
	return packHeader(header());
}

bool Packet::unpackHeader(MessageHeader &header) const noexcept {
	return header.deserialize(frame().array());
}

bool Packet::unpackHeader() noexcept {
	return unpackHeader(header());
}

bool Packet::bind() noexcept {
	auto length = header().getLength();
	if (testLength(length) && (frame().getIndex() == 0)
			&& frame().setLimit(length)) {
		MessageHeader::setLength(frame().array(), length);
		return true;
	} else {
		return false;
	}
}

bool Packet::validate() const noexcept {
	return (frame().getIndex() == 0)
			&& (frame().getLimit() == header().getLength())
			&& (header().getLength() >= HEADER_SIZE);
}

bool Packet::testLength() const noexcept {
	return testLength(header().getLength());
}

bool Packet::testLength(unsigned int length) noexcept {
	return (length >= HEADER_SIZE && length <= MTU);
}

unsigned int Packet::packets(unsigned int bytes) noexcept {
	return ((unsigned long long) bytes + PAYLOAD_SIZE - 1) / PAYLOAD_SIZE;
}

void Packet::printHeader(bool deep) const noexcept {
	if (deep) {
		MessageHeader header;
		unpackHeader(header);
		header.print();
	} else {
		this->header().print();
	}
}

} /* namespace wanhive */

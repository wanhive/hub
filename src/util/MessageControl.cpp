/*
 * MessageControl.cpp
 *
 * Message flow control implementation
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "MessageControl.h"
#include "../base/ds/Serializer.h"

namespace wanhive {

MessageControl::MessageControl() noexcept :
		length { }, sequenceNumber { }, session { } {

}

MessageControl::MessageControl(uint16_t length, uint16_t sequenceNumber,
		uint8_t session) noexcept :
		length { length }, sequenceNumber { sequenceNumber }, session { session } {

}

MessageControl::MessageControl(const unsigned char *buffer) noexcept {
	readControl(buffer);
}

MessageControl::~MessageControl() {

}

uint16_t MessageControl::getLength() const noexcept {
	return length;
}

void MessageControl::setLength(uint16_t length) noexcept {
	this->length = length;
}

uint16_t MessageControl::getSequenceNumber() const noexcept {
	return sequenceNumber;
}

void MessageControl::setSequenceNumber(uint16_t sequenceNumber) noexcept {
	this->sequenceNumber = sequenceNumber;
}

uint8_t MessageControl::getSession() const noexcept {
	return session;
}

void MessageControl::setSession(uint8_t session) noexcept {
	this->session = session;
}

void MessageControl::getControl(uint16_t &length, uint16_t &sequenceNumber,
		uint8_t &session) const noexcept {
	length = getLength();
	sequenceNumber = getSequenceNumber();
	session = getSession();
}

void MessageControl::setControl(uint16_t length, uint16_t sequenceNumber,
		uint8_t session) noexcept {
	setLength(length);
	setSequenceNumber(sequenceNumber);
	setSession(session);
}

void MessageControl::readControl(const unsigned char *buffer) noexcept {
	setLength(readLength(buffer));
	setSequenceNumber(readSequenceNumber(buffer));
	setSession(readSession(buffer));
}

void MessageControl::writeControl(unsigned char *buffer) const noexcept {
	writeLength(buffer, getLength());
	writeSequenceNumber(buffer, getSequenceNumber());
	writeSession(buffer, getSession());
}

uint16_t MessageControl::readLength(const unsigned char *buffer) noexcept {
	return Serializer::unpacku16(buffer);
}

void MessageControl::writeLength(unsigned char *buffer,
		uint16_t length) noexcept {
	Serializer::packi16(buffer, length);
}

uint16_t MessageControl::readSequenceNumber(
		const unsigned char *buffer) noexcept {
	return Serializer::unpacku16(buffer + 2);
}

void MessageControl::writeSequenceNumber(unsigned char *buffer,
		uint16_t sequenceNumber) noexcept {
	Serializer::packi16(buffer + 2, sequenceNumber);
}

uint8_t MessageControl::readSession(const unsigned char *buffer) noexcept {
	return Serializer::unpacku8(buffer + 4);
}

void MessageControl::writeSession(unsigned char *buffer,
		uint8_t session) noexcept {
	Serializer::packi8(buffer + 4, session);
}

} /* namespace wanhive */

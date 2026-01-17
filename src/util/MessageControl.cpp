/*
 * MessageControl.cpp
 *
 * Message's flow control
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

MessageControl::MessageControl(const unsigned char *data) noexcept {
	read(data);
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

void MessageControl::get(uint16_t &length, uint16_t &sequenceNumber,
		uint8_t &session) const noexcept {
	length = getLength();
	sequenceNumber = getSequenceNumber();
	session = getSession();
}

void MessageControl::set(uint16_t length, uint16_t sequenceNumber,
		uint8_t session) noexcept {
	setLength(length);
	setSequenceNumber(sequenceNumber);
	setSession(session);
}

void MessageControl::read(const unsigned char *data) noexcept {
	setLength(readLength(data));
	setSequenceNumber(readSequenceNumber(data));
	setSession(readSession(data));
}

void MessageControl::write(unsigned char *data) const noexcept {
	writeLength(data, getLength());
	writeSequenceNumber(data, getSequenceNumber());
	writeSession(data, getSession());
}

uint16_t MessageControl::readLength(const unsigned char *data) noexcept {
	return Serializer::unpacku16(data);
}

void MessageControl::writeLength(unsigned char *data, uint16_t length) noexcept {
	Serializer::packi16(data, length);
}

uint16_t MessageControl::readSequenceNumber(const unsigned char *data) noexcept {
	return Serializer::unpacku16(data + 2);
}

void MessageControl::writeSequenceNumber(unsigned char *data,
		uint16_t sequenceNumber) noexcept {
	Serializer::packi16(data + 2, sequenceNumber);
}

uint8_t MessageControl::readSession(const unsigned char *data) noexcept {
	return Serializer::unpacku8(data + 4);
}

void MessageControl::writeSession(unsigned char *data, uint8_t session) noexcept {
	Serializer::packi8(data + 4, session);
}

} /* namespace wanhive */

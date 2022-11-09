/*
 * MessageHeader.cpp
 *
 * Message header
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "MessageHeader.h"
#include <cinttypes>
#include <cstdio>

namespace {

//Offset to serialized address data
constexpr unsigned int OFF_ADDR = 0;
//Offset to serialized flow control data
constexpr unsigned int OFF_CTRL = wanhive::MessageAddress::SIZE;
//Offset to serialized context data
constexpr unsigned int OFF_CTX = OFF_CTRL + wanhive::MessageControl::SIZE;

}  // namespace

namespace wanhive {

MessageHeader::MessageHeader() noexcept {

}

MessageHeader::MessageHeader(const unsigned char *data) noexcept :
		MessageAddress { data }, MessageControl { data + OFF_CTRL }, MessageContext {
				data + OFF_CTX } {
}

MessageHeader::~MessageHeader() {

}

void MessageHeader::clear() noexcept {
	setLabel(0);
	setAddress(0, 0);
	setControl(0, 0, 0);
	setContext(0, 0, 0);
}

unsigned int MessageHeader::read(const unsigned char *data) noexcept {
	readAddress(data);
	readControl(data);
	readContext(data);
	return SIZE;
}

unsigned int MessageHeader::write(unsigned char *data) const noexcept {
	writeAddress(data);
	writeControl(data);
	writeContext(data);
	return SIZE;
}

void MessageHeader::readAddress(const unsigned char *data) noexcept {
	MessageAddress::readAddress(data);
}

void MessageHeader::writeAddress(unsigned char *data) const noexcept {
	MessageAddress::writeAddress(data);
}

void MessageHeader::readControl(const unsigned char *data) noexcept {
	MessageControl::readControl(data + OFF_CTRL);
}

void MessageHeader::writeControl(unsigned char *data) const noexcept {
	MessageControl::writeControl(data + OFF_CTRL);
}

void MessageHeader::readContext(const unsigned char *data) noexcept {
	MessageContext::readContext(data + OFF_CTX);
}

void MessageHeader::writeContext(unsigned char *data) const noexcept {
	MessageContext::writeContext(data + OFF_CTX);
}

void MessageHeader::print() const noexcept {
	fprintf(stderr,
			"LABEL:%#" PRIx64 " SRC:%" PRIu64 " DEST:%" PRIu64 " LEN:%u SEQN:%u SESSION:%u CMD:%u QLF:%u STATUS:%u\n",
			getLabel(), getSource(), getDestination(), getLength(),
			getSequenceNumber(), getSession(), getCommand(), getQualifier(),
			getStatus());
}

uint64_t MessageHeader::readLabel(const unsigned char *data) noexcept {
	return MessageAddress::readLabel(data);
}

void MessageHeader::writeLabel(unsigned char *data, uint64_t label) noexcept {
	MessageAddress::writeLabel(data, label);
}

uint64_t MessageHeader::readSource(const unsigned char *data) noexcept {
	return MessageAddress::readSource(data);
}

void MessageHeader::writeSource(unsigned char *data, uint64_t source) noexcept {
	MessageAddress::writeSource(data, source);
}

uint64_t MessageHeader::readDestination(const unsigned char *data) noexcept {
	return MessageAddress::readDestination(data);
}

void MessageHeader::writeDestination(unsigned char *data,
		uint64_t destination) noexcept {
	MessageAddress::writeDestination(data, destination);
}

uint16_t MessageHeader::readLength(const unsigned char *data) noexcept {
	return MessageControl::readLength(data + OFF_CTRL);
}

void MessageHeader::writeLength(unsigned char *data, uint16_t length) noexcept {
	MessageControl::writeLength(data + OFF_CTRL, length);
}

uint16_t MessageHeader::readSequenceNumber(const unsigned char *data) noexcept {
	return MessageControl::readSequenceNumber(data + OFF_CTRL);
}

void MessageHeader::writeSequenceNumber(unsigned char *data,
		uint16_t sequenceNumber) noexcept {
	MessageControl::writeSequenceNumber(data + OFF_CTRL, sequenceNumber);
}

uint8_t MessageHeader::readSession(const unsigned char *data) noexcept {
	return MessageControl::readSession(data + OFF_CTRL);
}

void MessageHeader::writeSession(unsigned char *data, uint8_t session) noexcept {
	MessageControl::writeSession(data + OFF_CTRL, session);
}

uint8_t MessageHeader::readCommand(const unsigned char *data) noexcept {
	return MessageContext::readCommand(data + OFF_CTX);
}

void MessageHeader::writeCommand(unsigned char *data, uint8_t command) noexcept {
	MessageContext::writeCommand(data + OFF_CTX, command);
}

uint8_t MessageHeader::readQualifier(const unsigned char *data) noexcept {
	return MessageContext::readQualifier(data + OFF_CTX);
}

void MessageHeader::writeQualifier(unsigned char *data,
		uint8_t qualifier) noexcept {
	MessageContext::writeQualifier(data + OFF_CTX, qualifier);
}

uint8_t MessageHeader::readStatus(const unsigned char *data) noexcept {
	return MessageContext::readStatus(data + OFF_CTX);
}

void MessageHeader::writeStatus(unsigned char *data, uint8_t status) noexcept {
	MessageContext::writeStatus(data + OFF_CTX, status);
}

} /* namespace wanhive */

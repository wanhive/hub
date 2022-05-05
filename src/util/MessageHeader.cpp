/*
 * MessageHeader.cpp
 *
 * Message header implementation
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

namespace wanhive {

MessageHeader::MessageHeader() noexcept {

}

MessageHeader::MessageHeader(const unsigned char *buffer) noexcept :
		MessageAddress { buffer }, MessageControl { buffer + OFF_CTRL }, MessageContext {
				buffer + OFF_CTX } {
}

MessageHeader::~MessageHeader() {

}

void MessageHeader::clear() noexcept {
	setLabel(0);
	setAddress(0, 0);
	setControl(0, 0, 0);
	setContext(0, 0, 0);
}

unsigned int MessageHeader::read(const unsigned char *buffer) noexcept {
	readAddress(buffer);
	readControl(buffer);
	readContext(buffer);
	return SIZE;
}

unsigned int MessageHeader::write(unsigned char *buffer) const noexcept {
	writeAddress(buffer);
	writeControl(buffer);
	writeContext(buffer);
	return SIZE;
}

void MessageHeader::readAddress(const unsigned char *buffer) noexcept {
	MessageAddress::readAddress(buffer);
}

void MessageHeader::writeAddress(unsigned char *buffer) const noexcept {
	MessageAddress::writeAddress(buffer);
}

void MessageHeader::readControl(const unsigned char *buffer) noexcept {
	MessageControl::readControl(buffer + OFF_CTRL);
}

void MessageHeader::writeControl(unsigned char *buffer) const noexcept {
	MessageControl::writeControl(buffer + OFF_CTRL);
}

void MessageHeader::readContext(const unsigned char *buffer) noexcept {
	MessageContext::readContext(buffer + OFF_CTX);
}

void MessageHeader::writeContext(unsigned char *buffer) const noexcept {
	MessageContext::writeContext(buffer + OFF_CTX);
}

void MessageHeader::print() const noexcept {
	fprintf(stderr,
			"LABEL:%#" PRIx64 " SRC:%" PRIu64 " DEST:%" PRIu64 " LENGTH:%u SEQN:%u SESSION:%u CMD:%u QLF:%u STATUS:%u\n",
			getLabel(), getSource(), getDestination(), getLength(),
			getSequenceNumber(), getSession(), getCommand(), getQualifier(),
			getStatus());
}

uint64_t MessageHeader::readLabel(const unsigned char *buffer) noexcept {
	return MessageAddress::readLabel(buffer);
}

void MessageHeader::writeLabel(unsigned char *buffer, uint64_t label) noexcept {
	MessageAddress::writeLabel(buffer, label);
}

uint64_t MessageHeader::readSource(const unsigned char *buffer) noexcept {
	return MessageAddress::readSource(buffer);
}

void MessageHeader::writeSource(unsigned char *buffer, uint64_t source) noexcept {
	MessageAddress::writeSource(buffer, source);
}

uint64_t MessageHeader::readDestination(const unsigned char *buffer) noexcept {
	return MessageAddress::readDestination(buffer);
}

void MessageHeader::writeDestination(unsigned char *buffer,
		uint64_t destination) noexcept {
	MessageAddress::writeDestination(buffer, destination);
}

uint16_t MessageHeader::readLength(const unsigned char *buffer) noexcept {
	return MessageControl::readLength(buffer + OFF_CTRL);
}

void MessageHeader::writeLength(unsigned char *buffer, uint16_t length) noexcept {
	MessageControl::writeLength(buffer + OFF_CTRL, length);
}

uint16_t MessageHeader::readSequenceNumber(const unsigned char *buffer) noexcept {
	return MessageControl::readSequenceNumber(buffer + OFF_CTRL);
}

void MessageHeader::writeSequenceNumber(unsigned char *buffer,
		uint16_t sequenceNumber) noexcept {
	MessageControl::writeSequenceNumber(buffer + OFF_CTRL, sequenceNumber);
}

uint8_t MessageHeader::readSession(const unsigned char *buffer) noexcept {
	return MessageControl::readSession(buffer + OFF_CTRL);
}

void MessageHeader::writeSession(unsigned char *buffer,
		uint8_t session) noexcept {
	MessageControl::writeSession(buffer + OFF_CTRL, session);
}

uint8_t MessageHeader::readCommand(const unsigned char *buffer) noexcept {
	return MessageContext::readCommand(buffer + OFF_CTX);
}

void MessageHeader::writeCommand(unsigned char *buffer,
		uint8_t command) noexcept {
	MessageContext::writeCommand(buffer + OFF_CTX, command);
}

uint8_t MessageHeader::readQualifier(const unsigned char *buffer) noexcept {
	return MessageContext::readQualifier(buffer + OFF_CTX);
}

void MessageHeader::writeQualifier(unsigned char *buffer,
		uint8_t qualifier) noexcept {
	MessageContext::writeQualifier(buffer + OFF_CTX, qualifier);
}

uint8_t MessageHeader::readStatus(const unsigned char *buffer) noexcept {
	return MessageContext::readStatus(buffer + OFF_CTX);
}

void MessageHeader::writeStatus(unsigned char *buffer, uint8_t status) noexcept {
	MessageContext::writeStatus(buffer + OFF_CTX, status);
}

} /* namespace wanhive */

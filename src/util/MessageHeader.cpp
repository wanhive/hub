/*
 * MessageHeader.cpp
 *
 * Wanhive's fixed size message header implementation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "MessageHeader.h"
#include "../base/ds/Serializer.h"
#include <cstdio>
#include <cstring>

namespace wanhive {

MessageHeader::MessageHeader() noexcept {
	clear();
}

MessageHeader::MessageHeader(uint64_t source, uint64_t destination,
		uint16_t length, uint16_t sequenceNumber, uint8_t session,
		uint8_t command, uint8_t qualifier, uint8_t status,
		uint64_t label) noexcept {
	load(source, destination, length, sequenceNumber, session, command,
			qualifier, status, label);
}

MessageHeader::~MessageHeader() {

}

void MessageHeader::clear() noexcept {
	memset(&header, 0, sizeof(header));
}

uint64_t MessageHeader::getLabel() const noexcept {
	return header.label;
}

void MessageHeader::setLabel(uint64_t label) noexcept {
	header.label = label;
}

uint64_t MessageHeader::getSource() const noexcept {
	return header.source;
}
void MessageHeader::setSource(uint64_t source) noexcept {
	header.source = source;
}

uint64_t MessageHeader::getDestination() const noexcept {
	return header.destination;
}
void MessageHeader::setDestination(uint64_t destination) noexcept {
	header.destination = destination;
}

uint16_t MessageHeader::getLength() const noexcept {
	return header.length;
}
void MessageHeader::setLength(uint16_t length) noexcept {
	header.length = length;
}

uint16_t MessageHeader::getSequenceNumber() const noexcept {
	return header.sequenceNumber;
}
void MessageHeader::setSequenceNumber(uint16_t sequenceNumber) noexcept {
	header.sequenceNumber = sequenceNumber;
}

uint8_t MessageHeader::getSession() const noexcept {
	return header.session;
}
void MessageHeader::setSession(uint8_t session) noexcept {
	header.session = session;
}

uint8_t MessageHeader::getCommand() const noexcept {
	return header.ctx.command;
}
void MessageHeader::setCommand(uint8_t command) noexcept {
	header.ctx.command = command;
}

uint8_t MessageHeader::getQualifier() const noexcept {
	return header.ctx.qualifier;
}
void MessageHeader::setQualifier(uint8_t qualifier) noexcept {
	header.ctx.qualifier = qualifier;
}

uint8_t MessageHeader::getStatus() const noexcept {
	return header.ctx.status;
}
void MessageHeader::setStatus(uint8_t status) noexcept {
	header.ctx.status = status;
}

void MessageHeader::load(uint64_t source, uint64_t destination, uint16_t length,
		uint16_t sequenceNumber, uint8_t session, uint8_t command,
		uint8_t qualifier, uint8_t status, uint64_t label) noexcept {
	header.label = label;
	header.source = source;
	header.destination = destination;
	header.length = length;
	header.sequenceNumber = sequenceNumber;
	header.session = session;
	header.ctx.command = command;
	header.ctx.qualifier = qualifier;
	header.ctx.status = status;
}

unsigned int MessageHeader::deserialize(const unsigned char *buffer) noexcept {
	header.label = getLabel(buffer);
	header.source = getSource(buffer);
	header.destination = getDestination(buffer);

	header.length = getLength(buffer);
	header.sequenceNumber = getSequenceNumber(buffer);
	header.session = getSession(buffer);

	header.ctx.command = getCommand(buffer);
	header.ctx.qualifier = getQualifier(buffer);
	header.ctx.status = getStatus(buffer);
	return SIZE;
}

unsigned int MessageHeader::serialize(unsigned char *buffer) const noexcept {
	setLabel(buffer, header.label);
	setSource(buffer, header.source);
	setDestination(buffer, header.destination);

	setLength(buffer, header.length);
	setSequenceNumber(buffer, header.sequenceNumber);
	setSession(buffer, header.session);

	setCommand(buffer, header.ctx.command);
	setQualifier(buffer, header.ctx.qualifier);
	setStatus(buffer, header.ctx.status);
	return SIZE;
}

void MessageHeader::print() const noexcept {
	fprintf(stderr,
			"LABEL:%#" PRIx64 " SRC:%" PRIu64 " DEST:%" PRIu64 " LENGTH:%u SEQN:%u SESSION:%u CMD:%u QLF:%u STATUS:%u\n",
			header.label, header.source, header.destination, header.length,
			header.sequenceNumber, header.session, header.ctx.command,
			header.ctx.qualifier, header.ctx.status);
}

uint64_t MessageHeader::getLabel(const unsigned char *buffer) noexcept {
	return Serializer::unpacku64(buffer);
}
void MessageHeader::setLabel(unsigned char *buffer, uint64_t label) noexcept {
	Serializer::packi64(buffer, label);
}

uint64_t MessageHeader::getSource(const unsigned char *buffer) noexcept {
	return Serializer::unpacku64(buffer + 8);
}
void MessageHeader::setSource(unsigned char *buffer, uint64_t source) noexcept {
	Serializer::packi64(buffer + 8, source);
}

uint64_t MessageHeader::getDestination(const unsigned char *buffer) noexcept {
	return Serializer::unpacku64(buffer + 16);
}
void MessageHeader::setDestination(unsigned char *buffer,
		uint64_t destination) noexcept {
	Serializer::packi64(buffer + 16, destination);
}

uint16_t MessageHeader::getLength(const unsigned char *buffer) noexcept {
	return Serializer::unpacku16(buffer + 24);
}
void MessageHeader::setLength(unsigned char *buffer, uint16_t length) noexcept {
	Serializer::packi16(buffer + 24, length);
}

uint16_t MessageHeader::getSequenceNumber(const unsigned char *buffer) noexcept {
	return Serializer::unpacku16(buffer + 26);
}
void MessageHeader::setSequenceNumber(unsigned char *buffer,
		uint16_t sequenceNumber) noexcept {
	Serializer::packi16(buffer + 26, sequenceNumber);
}

uint8_t MessageHeader::getSession(const unsigned char *buffer) noexcept {
	return Serializer::unpacku8(buffer + 28);
}
void MessageHeader::setSession(unsigned char *buffer, uint8_t session) noexcept {
	Serializer::packi8(buffer + 28, session);
}

uint8_t MessageHeader::getCommand(const unsigned char *buffer) noexcept {
	return Serializer::unpacku8(buffer + 29);
}

void MessageHeader::setCommand(unsigned char *buffer, uint8_t command) noexcept {
	Serializer::packi8(buffer + 29, command);
}

uint8_t MessageHeader::getQualifier(const unsigned char *buffer) noexcept {
	return Serializer::unpacku8(buffer + 30);
}
void MessageHeader::setQualifier(unsigned char *buffer,
		uint8_t qualifier) noexcept {
	Serializer::packi8(buffer + 30, qualifier);
}

uint8_t MessageHeader::getStatus(const unsigned char *buffer) noexcept {
	return Serializer::unpacku8(buffer + 31);
}
void MessageHeader::setStatus(unsigned char *buffer, uint8_t status) noexcept {
	Serializer::packi8(buffer + 31, status);
}

unsigned int MessageHeader::serialize(unsigned char *buffer, uint64_t source,
		uint64_t destination, uint16_t length, uint16_t sequenceNumber,
		uint8_t session, uint8_t command, uint8_t qualifier, uint8_t status,
		uint64_t label) noexcept {
	setLabel(buffer, label);
	setSource(buffer, source);
	setDestination(buffer, destination);

	setLength(buffer, length);
	setSequenceNumber(buffer, sequenceNumber);
	setSession(buffer, session);

	setCommand(buffer, command);
	setQualifier(buffer, qualifier);
	setStatus(buffer, status);
	return SIZE;
}

} /* namespace wanhive */

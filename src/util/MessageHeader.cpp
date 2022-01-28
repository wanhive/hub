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
#include "../base/ds/Serializer.h"
#include <cstdio>
#include <cstring>

namespace wanhive {

MessageHeader::MessageHeader() noexcept {
	clear();
}

MessageHeader::MessageHeader(const MessageAddress &address,
		const MessageControl &ctrl, const MessageContext &ctx,
		uint64_t label) noexcept {
	load(address, ctrl, ctx, label);
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
	return header.addr.source;
}
void MessageHeader::setSource(uint64_t source) noexcept {
	header.addr.source = source;
}

uint64_t MessageHeader::getDestination() const noexcept {
	return header.addr.destination;
}
void MessageHeader::setDestination(uint64_t destination) noexcept {
	header.addr.destination = destination;
}

uint16_t MessageHeader::getLength() const noexcept {
	return header.ctrl.length;
}
void MessageHeader::setLength(uint16_t length) noexcept {
	header.ctrl.length = length;
}

uint16_t MessageHeader::getSequenceNumber() const noexcept {
	return header.ctrl.sequenceNumber;
}
void MessageHeader::setSequenceNumber(uint16_t sequenceNumber) noexcept {
	header.ctrl.sequenceNumber = sequenceNumber;
}

uint8_t MessageHeader::getSession() const noexcept {
	return header.ctrl.session;
}
void MessageHeader::setSession(uint8_t session) noexcept {
	header.ctrl.session = session;
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

void MessageHeader::getAddress(MessageAddress &address) const noexcept {
	address = header.addr;
}

void MessageHeader::setAddress(const MessageAddress &address) noexcept {
	header.addr = address;
}

void MessageHeader::getControl(MessageControl &ctrl) const noexcept {
	ctrl = header.ctrl;
}

void MessageHeader::setControl(const MessageControl &ctrl) noexcept {
	header.ctrl = ctrl;
}

void MessageHeader::getContext(MessageContext &ctx) const noexcept {
	ctx = header.ctx;
}

void MessageHeader::setContext(const MessageContext ctx) noexcept {
	header.ctx = ctx;
}

void MessageHeader::load(const MessageAddress &address,
		const MessageControl &ctrl, const MessageContext &ctx,
		uint64_t label) noexcept {
	setLabel(label);
	setAddress(address);
	setControl(ctrl);
	setContext(ctx);
}

void MessageHeader::load(uint64_t source, uint64_t destination, uint16_t length,
		uint16_t sequenceNumber, uint8_t session, uint8_t command,
		uint8_t qualifier, uint8_t status, uint64_t label) noexcept {
	setLabel(label);
	//Address fields
	setSource(source);
	setDestination(destination);
	//Control fields
	setLength(length);
	setSequenceNumber(sequenceNumber);
	setSession(session);
	//Context fields
	setCommand(command);
	setQualifier(qualifier);
	setStatus(status);
}

unsigned int MessageHeader::deserialize(const unsigned char *buffer) noexcept {
	setLabel(getLabel(buffer));
	//Address fields
	setSource(getSource(buffer));
	setDestination(getDestination(buffer));
	//Control fields
	setLength(getLength(buffer));
	setSequenceNumber(getSequenceNumber(buffer));
	setSession(getSession(buffer));
	//Context fields
	setCommand(getCommand(buffer));
	setQualifier(getQualifier(buffer));
	setStatus(getStatus(buffer));

	return SIZE;
}

unsigned int MessageHeader::serialize(unsigned char *buffer) const noexcept {
	setLabel(buffer, getLabel());
	//Address fields
	setSource(buffer, getSource());
	setDestination(buffer, getDestination());
	//Control fields
	setLength(buffer, getLength());
	setSequenceNumber(buffer, getSequenceNumber());
	setSession(buffer, getSession());
	//Context fields
	setCommand(buffer, getCommand());
	setQualifier(buffer, getQualifier());
	setStatus(buffer, getStatus());

	return SIZE;
}

void MessageHeader::print() const noexcept {
	fprintf(stderr,
			"LABEL:%#" PRIx64 " SRC:%" PRIu64 " DEST:%" PRIu64 " LENGTH:%u SEQN:%u SESSION:%u CMD:%u QLF:%u STATUS:%u\n",
			getLabel(), getSource(), getDestination(), getLength(),
			getSequenceNumber(), getSession(), getCommand(), getQualifier(),
			getStatus());
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

void MessageHeader::getAddress(const unsigned char *buffer,
		MessageAddress &address) noexcept {
	address.source = getSource(buffer);
	address.destination = getDestination(buffer);
}
void MessageHeader::setAddress(unsigned char *buffer,
		const MessageAddress &address) noexcept {
	setSource(buffer, address.source);
	setDestination(buffer, address.destination);
}

void MessageHeader::getControl(const unsigned char *buffer,
		MessageControl &ctrl) noexcept {
	ctrl.length = getLength(buffer);
	ctrl.sequenceNumber = getSequenceNumber(buffer);
	ctrl.session = getSession(buffer);
}

void MessageHeader::setControl(unsigned char *buffer,
		const MessageControl &ctrl) noexcept {
	setLength(buffer, ctrl.length);
	setSequenceNumber(buffer, ctrl.sequenceNumber);
	setSession(buffer, ctrl.session);
}

void MessageHeader::getContext(const unsigned char *buffer,
		MessageContext &ctx) noexcept {
	ctx.command = getCommand(buffer);
	ctx.qualifier = getQualifier(buffer);
	ctx.status = getStatus(buffer);
}

void MessageHeader::setContext(unsigned char *buffer,
		const MessageContext ctx) noexcept {
	setCommand(buffer, ctx.command);
	setQualifier(buffer, ctx.qualifier);
	setStatus(buffer, ctx.status);
}

unsigned int MessageHeader::serialize(unsigned char *buffer,
		const MessageAddress &address, const MessageControl &ctrl,
		const MessageContext &ctx, uint64_t label) noexcept {
	return serialize(buffer, address.source, address.destination, ctrl.length,
			ctrl.sequenceNumber, ctrl.session, ctx.command, ctx.qualifier,
			ctx.status, label);
}

unsigned int MessageHeader::serialize(unsigned char *buffer, uint64_t source,
		uint64_t destination, uint16_t length, uint16_t sequenceNumber,
		uint8_t session, uint8_t command, uint8_t qualifier, uint8_t status,
		uint64_t label) noexcept {
	setLabel(buffer, label);
	//Address fields
	setSource(buffer, source);
	setDestination(buffer, destination);
	//Control fields
	setLength(buffer, length);
	setSequenceNumber(buffer, sequenceNumber);
	setSession(buffer, session);
	//Context fields
	setCommand(buffer, command);
	setQualifier(buffer, qualifier);
	setStatus(buffer, status);

	return SIZE;
}

} /* namespace wanhive */

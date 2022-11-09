/*
 * MessageContext.cpp
 *
 * Message's context
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "MessageContext.h"
#include "../base/ds/Serializer.h"

namespace wanhive {

MessageContext::MessageContext() noexcept :
		command { }, qualifier { }, status { } {

}

MessageContext::MessageContext(uint8_t command, uint8_t qualifier,
		uint8_t status) noexcept :
		command { command }, qualifier { qualifier }, status { status } {

}

MessageContext::MessageContext(const unsigned char *data) noexcept {
	readContext(data);
}

MessageContext::~MessageContext() {

}

uint8_t MessageContext::getCommand() const noexcept {
	return command;
}

void MessageContext::setCommand(uint8_t command) noexcept {
	this->command = command;
}

uint8_t MessageContext::getQualifier() const noexcept {
	return qualifier;
}

void MessageContext::setQualifier(uint8_t qualifier) noexcept {
	this->qualifier = qualifier;
}

uint8_t MessageContext::getStatus() const noexcept {
	return status;
}

void MessageContext::setStatus(uint8_t status) noexcept {
	this->status = status;
}

void MessageContext::getContext(uint8_t &command, uint8_t &qualifier,
		uint8_t &status) const noexcept {
	command = getCommand();
	qualifier = getQualifier();
	status = getStatus();
}

void MessageContext::setContext(uint8_t command, uint8_t qualifier,
		uint8_t status) noexcept {
	setCommand(command);
	setQualifier(qualifier);
	setStatus(status);
}

void MessageContext::readContext(const unsigned char *data) noexcept {
	setCommand(readCommand(data));
	setQualifier(readQualifier(data));
	setStatus(readStatus(data));
}

void MessageContext::writeContext(unsigned char *data) const noexcept {
	writeCommand(data, getCommand());
	writeQualifier(data, getQualifier());
	writeStatus(data, getStatus());
}

uint8_t MessageContext::readCommand(const unsigned char *data) noexcept {
	return Serializer::unpacku8(data);
}

void MessageContext::writeCommand(unsigned char *data, uint8_t command) noexcept {
	Serializer::packi8(data, command);
}

uint8_t MessageContext::readQualifier(const unsigned char *data) noexcept {
	return Serializer::unpacku8(data + 1);
}

void MessageContext::writeQualifier(unsigned char *data,
		uint8_t qualifier) noexcept {
	Serializer::packi8(data + 1, qualifier);
}

uint8_t MessageContext::readStatus(const unsigned char *data) noexcept {
	return Serializer::unpacku8(data + 2);
}

void MessageContext::writeStatus(unsigned char *data, uint8_t status) noexcept {
	Serializer::packi8(data + 2, status);
}

} /* namespace wanhive */

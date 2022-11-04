/*
 * FlowControl.cpp
 *
 * Message stream flow control
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "FlowControl.h"

namespace wanhive {

FlowControl::FlowControl() noexcept {

}

FlowControl::~FlowControl() {

}

uint64_t FlowControl::getSource() const noexcept {
	return source;
}

void FlowControl::setSource(uint64_t source) noexcept {
	this->source = source;
}

uint16_t FlowControl::getSequenceNumber() const noexcept {
	return this->sequenceNumber;
}

uint16_t FlowControl::nextSequenceNumber() noexcept {
	if (!sequenceNumber) {
		sequenceNumber = 1;
	}
	return sequenceNumber++;
}

void FlowControl::setSequenceNumber(uint16_t sequenceNumber) noexcept {
	this->sequenceNumber = sequenceNumber;
}

uint8_t FlowControl::getSession() const noexcept {
	return session;
}

void FlowControl::setSession(uint8_t session) noexcept {
	this->session = session;
}

} /* namespace wanhive */

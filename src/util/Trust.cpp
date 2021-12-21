/*
 * Trust.cpp
 *
 * Message signing and verification
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Trust.h"
#include "../base/ds/Serializer.h"

namespace wanhive {

Trust::Trust() noexcept :
		pki(nullptr) {

}

Trust::Trust(const PKI *pki) noexcept :
		pki(pki) {

}

Trust::~Trust() {

}

const PKI* Trust::get() const noexcept {
	return pki;
}

void Trust::set(const PKI *pki) noexcept {
	this->pki = pki;
}

bool Trust::sign(unsigned char *msg, unsigned int &length) noexcept {
	if (get() == nullptr) {
		return true;
	}
	//Make sure that we have got enough space for appending the signature
	if (!msg || length < Message::HEADER_SIZE
			|| (length + PKI::SIGNATURE_LENGTH) > Message::MTU) {
		return false;
	}
	//--------------------------------------------------------------------------
	Signature signature;
	//Finalize the buffer, otherwise verification will fail
	MessageHeader::setLength(msg, length + PKI::SIGNATURE_LENGTH);
	if (get()->sign(msg, length, &signature)) {
		Serializer::packib((msg + length), (unsigned char*) &signature,
				PKI::SIGNATURE_LENGTH);
		length += PKI::SIGNATURE_LENGTH;
		return true;
	} else {
		//Roll Back
		MessageHeader::setLength(msg, length);
		return false;
	}
}

bool Trust::sign(Message *msg) noexcept {
	if (msg && msg->validate()) {
		unsigned int length = msg->getLength();
		auto ret = sign(msg->buffer(), length);
		//Update the message length
		msg->putLength(length);
		return ret;
	} else {
		return false;
	}
}

bool Trust::verify(const unsigned char *msg, unsigned int length) noexcept {
	if (get() == nullptr) {
		return true;
	}

	//Make sure that the message is long enough to carry a signature
	if (msg && length <= Message::MTU
			&& length >= (PKI::SIGNATURE_LENGTH + Message::HEADER_SIZE)) {
		auto bufLength = length - PKI::SIGNATURE_LENGTH;
		auto block = msg;
		auto sign = (const Signature*) (block + bufLength);
		return get()->verify(block, bufLength, sign);
	} else {
		return false;
	}
}

bool Trust::verify(const Message *msg) noexcept {
	return msg && msg->validate() && verify(msg->buffer(), msg->getLength());
}

} /* namespace wanhive */

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
#include "Message.h"
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

bool Trust::sign(unsigned char *msg, unsigned int &length) const noexcept {
	if (!get()) {
		return true;
	} else if (msg && (length >= Message::HEADER_SIZE)
			&& (length <= (Message::MTU - PKI::SIGNATURE_LENGTH))) {
		//Finalize the buffer, otherwise verification will fail
		MessageHeader::setLength(msg, length + PKI::SIGNATURE_LENGTH);
		Signature signature;
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
	} else {
		return false;
	}
}

bool Trust::verify(const unsigned char *msg, unsigned int length) const noexcept {
	if (!get()) {
		return true;
	} else if (msg && length <= Message::MTU
			&& length >= (PKI::SIGNATURE_LENGTH + Message::HEADER_SIZE)) {
		//The message is long enough to carry a signature
		auto bufLength = length - PKI::SIGNATURE_LENGTH;
		auto block = msg;
		auto sign = (const Signature*) (block + bufLength);
		return get()->verify(block, bufLength, sign);
	} else {
		return false;
	}
}

} /* namespace wanhive */

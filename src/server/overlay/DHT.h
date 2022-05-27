/*
 * DHT.h
 *
 * Distributed hash table properties
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_OVERLAY_DHT_H_
#define WH_SERVER_OVERLAY_DHT_H_

namespace wanhive {
/**
 * Distributed hash table properties
 */
class DHT {
public:
	DHT() noexcept;
	~DHT();
	static constexpr unsigned int identifierLength() noexcept {
		return IDENTIFIER_LENGTH;
	}
public:
	/** The maximum identifier length in bits */
	static constexpr unsigned int IDENTIFIER_LENGTH = 16;
};

} /* namespace wanhive */

#endif /* WH_SERVER_OVERLAY_DHT_H_ */

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

#ifndef WH_SERVER_CORE_DHT_H_
#define WH_SERVER_CORE_DHT_H_

namespace wanhive {
/**
 * Distributed hash table (DHT) properties
 */
class DHT {
public:
	/**
	 * Constructor
	 */
	DHT() noexcept;
	/**
	 * Destructor
	 */
	~DHT();
	/**
	 * Returns the maximum key length in bits. This is an architectural limit
	 * and the implementations should not use a larger value.
	 * @return maximum identifier length in bits
	 */
	static constexpr unsigned int keyLength() noexcept {
		return KEY_LENGTH;
	}
public:
#ifndef WH_DHT_KEYLEN
#define WH_DHT_KEYLEN 10U
#endif
	/*! The maximum key length in bits */
	static constexpr unsigned int KEY_LENGTH =
			(WH_DHT_KEYLEN > 16 || WH_DHT_KEYLEN <= 0) ? 16 : WH_DHT_KEYLEN;
#undef WH_DHT_KEYLEN
};

} /* namespace wanhive */

#endif /* WH_SERVER_CORE_DHT_H_ */

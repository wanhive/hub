/**
 * @file Things.h
 *
 * Repository of things
 *
 *
 * Copyright (C) 2025 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_AUTH_THINGS_H_
#define WH_SERVER_AUTH_THINGS_H_
#include "../../base/db/DataStore.h"
#include "../../util/Verifier.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Repository of things
 */
class Things final: public DataStore {
public:
	/**
	 * Constructor: creates an empty repository.
	 */
	Things() noexcept;
	/**
	 * Constructor: connects to a database.
	 * @param info connection parameters
	 */
	Things(const DBInfo &info);
	/**
	 * Destructor
	 */
	~Things();
	/**
	 * Sets the database query string (doesn't duplicate). The query must
	 * accept a unique identifier (UID) as the query parameter and return
	 * a tuple (UID, SALT, VERIFIER, TYPE) in that order.
	 * @param command database query string
	 */
	void setCommand(const char *command) noexcept;
	/**
	 * Initializes a mutual authenticator for a given identity.
	 * @param identity thing's identity
	 * @param nonce cryptographic nonce
	 * @param verifier mutual authenticator
	 */
	void get(unsigned long long identity, const Data &nonce,
			Verifier *verifier);
private:
	const char *command { nullptr };
};

} /* namespace wanhive */

#endif /* WH_SERVER_AUTH_THINGS_H_ */

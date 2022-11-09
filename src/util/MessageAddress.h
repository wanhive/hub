/*
 * MessageAddress.h
 *
 * Message's address
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_MESSAGEADDRESS_H_
#define WH_UTIL_MESSAGEADDRESS_H_
#include <cstdint>

namespace wanhive {
/**
 * Message address implementation
 */
class MessageAddress {
public:
	/**
	 * Default constructor: zero-initializes the data members.
	 */
	MessageAddress() noexcept;
	/**
	 * Constructor: initializes data members with the given values.
	 * @param source source identifier's value
	 * @param destination destination identifier's value
	 * @param label label's value
	 */
	MessageAddress(uint64_t source, uint64_t destination,
			uint64_t label = 0) noexcept;
	/**
	 * Constructor: extracts values from a serialized object.
	 * @param data serialized object
	 */
	MessageAddress(const unsigned char *data) noexcept;
	/**
	 * Destructor
	 */
	~MessageAddress();
	//-----------------------------------------------------------------
	/**
	 * Returns the label.
	 * @return label
	 */
	uint64_t getLabel() const noexcept;
	/**
	 * Sets a new label.
	 * @param label new label
	 */
	void setLabel(uint64_t label) noexcept;
	/**
	 * Returns the source identifier.
	 * @return source identifier
	 */
	uint64_t getSource() const noexcept;
	/**
	 * Sets a new source identifier.
	 * @param source new source identifier
	 */
	void setSource(uint64_t source) noexcept;
	/**
	 * Returns the destination identifier.
	 * @return destination identifier
	 */
	uint64_t getDestination() const noexcept;
	/**
	 * Sets a new destination identifier.
	 * @param destination new destination identifier
	 */
	void setDestination(uint64_t destination) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns source and destination identifiers.
	 * @param source stores source identifier's value
	 * @param destination stores destination identifier's value
	 */
	void getAddress(uint64_t &source, uint64_t &destination) const noexcept;
	/**
	 * Sets new source and destination identifiers.
	 * @param source new source identifier
	 * @param destination new destination identifier
	 */
	void setAddress(uint64_t source, uint64_t destination) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Extracts values from a serialized object.
	 * @param data serialized object
	 */
	void readAddress(const unsigned char *data) noexcept;
	/**
	 * Translates address data into a serialized object.
	 * @param data output buffer (MessageAddress::SIZE is the minimum required
	 * buffer size in bytes).
	 */
	void writeAddress(unsigned char *data) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads serialized object's label.
	 * @param data serialized object
	 * @return label
	 */
	static uint64_t readLabel(const unsigned char *data) noexcept;
	/**
	 * Updates serialized object's label.
	 * @param data serialized object
	 * @param label new label
	 */
	static void writeLabel(unsigned char *data, uint64_t label) noexcept;
	/**
	 * Reads serialized object's source identifier.
	 * @param data serialized object
	 * @return source identifier
	 */
	static uint64_t readSource(const unsigned char *data) noexcept;
	/**
	 * Updates serialized object's source identifier.
	 * @param data serialized object
	 * @param source new source identifier
	 */
	static void writeSource(unsigned char *data, uint64_t source) noexcept;
	/**
	 * Reads serialized object's destination identifier.
	 * @param data serialized object
	 * @return destination identifier
	 */
	static uint64_t readDestination(const unsigned char *data) noexcept;
	/**
	 * Updates serialized object's destination identifier.
	 * @param data serialized object
	 * @param destination new destination identifier
	 */
	static void writeDestination(unsigned char *data,
			uint64_t destination) noexcept;
public:
	/** Serialized object's size in bytes */
	static constexpr unsigned int SIZE = 24;
private:
	uint64_t label; //Application dependent
	uint64_t source; //Source identifier
	uint64_t destination; //Destination identifier
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGEADDRESS_H_ */

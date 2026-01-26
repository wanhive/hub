/**
 * @file MessageAddress.h
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

/*! @namespace wanhive */
namespace wanhive {
/**
 * Message address
 */
class MessageAddress {
public:
	/**
	 * Default constructor: sets all fields to zero.
	 */
	MessageAddress() noexcept;
	/**
	 * Constructor: populates the address data.
	 * @param source source identifier
	 * @param destination destination identifier
	 * @param label label's value
	 */
	MessageAddress(uint64_t source, uint64_t destination,
			uint64_t label = 0) noexcept;
	/**
	 * Constructor: reads the serialized address data.
	 * @param data serialized address
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
	 * @return source
	 */
	uint64_t getSource() const noexcept;
	/**
	 * Sets a new source identifier.
	 * @param source new source
	 */
	void setSource(uint64_t source) noexcept;
	/**
	 * Returns the destination identifier.
	 * @return destination
	 */
	uint64_t getDestination() const noexcept;
	/**
	 * Sets a new destination identifier.
	 * @param destination new destination
	 */
	void setDestination(uint64_t destination) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns source and destination identifiers.
	 * @param source stores source
	 * @param destination stores destination
	 */
	void get(uint64_t &source, uint64_t &destination) const noexcept;
	/**
	 * Sets new source and destination identifiers.
	 * @param source new source
	 * @param destination new destination
	 */
	void set(uint64_t source, uint64_t destination) noexcept;
protected:
	/**
	 * Reads the serialized address data.
	 * @param data serialized address
	 */
	void read(const unsigned char *data) noexcept;
	/**
	 * Serializes the address data.
	 * @param data output buffer
	 */
	void write(unsigned char *data) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads serialized address's label.
	 * @param data serialized address
	 * @return label
	 */
	static uint64_t readLabel(const unsigned char *data) noexcept;
	/**
	 * Updates serialized address's label.
	 * @param data serialized address
	 * @param label new label
	 */
	static void writeLabel(unsigned char *data, uint64_t label) noexcept;
	/**
	 * Reads serialized address's source identifier.
	 * @param data serialized address
	 * @return source
	 */
	static uint64_t readSource(const unsigned char *data) noexcept;
	/**
	 * Updates serialized address's source identifier.
	 * @param data serialized address
	 * @param source new source
	 */
	static void writeSource(unsigned char *data, uint64_t source) noexcept;
	/**
	 * Reads serialized address's destination identifier.
	 * @param data serialized address
	 * @return destination
	 */
	static uint64_t readDestination(const unsigned char *data) noexcept;
	/**
	 * Updates serialized address's destination identifier.
	 * @param data serialized address
	 * @param destination new destination
	 */
	static void writeDestination(unsigned char *data,
			uint64_t destination) noexcept;
public:
	/*! Serialized address's size in bytes */
	static constexpr unsigned int SIZE = 24;
private:
	uint64_t label;
	uint64_t source;
	uint64_t destination;
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGEADDRESS_H_ */

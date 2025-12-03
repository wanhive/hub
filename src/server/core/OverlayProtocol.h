/*
 * OverlayProtocol.h
 *
 * Overlay network protocol
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_CORE_OVERLAYPROTOCOL_H_
#define WH_SERVER_CORE_OVERLAYPROTOCOL_H_
#include "../../hub/Protocol.h"
#include "OverlayHubInfo.h"

namespace wanhive {
/**
 * Overlay network protocol implementation
 * @note Supports blocking IO only
 */
class OverlayProtocol: public Protocol {
public:
	/**
	 * Default constructor
	 */
	OverlayProtocol() noexcept;
	/**
	 * Destructor
	 */
	~OverlayProtocol();
	//-----------------------------------------------------------------
	/**
	 * Creates a describe-host request to fetch a host's information.
	 * @param host host's identifier
	 * @return message length on success, 0 on error
	 */
	unsigned int createDescribeRequest(uint64_t host) noexcept;
	/**
	 * Processes the response to a describe-node request to fetch a host's
	 * information.
	 * @param info stores host's information
	 * @return message length on success, 0 on error
	 */
	unsigned int processDescribeResponse(OverlayHubInfo &info) const noexcept;
	/**
	 * Prepares and executes a describe-node request to fetch a host's
	 * information.
	 * @param host host's identifier
	 * @param info stores the host's information
	 * @return true on success, false on error (request denied by the host)
	 */
	bool describeRequest(uint64_t host, OverlayHubInfo &info);
	//-----------------------------------------------------------------
	/**
	 * Creates a get-predecessor request to fetch a host's predecessor.
	 * @param host host's identifier
	 * @return message length on success, 0 on error
	 */
	unsigned int createGetPredecessorRequest(uint64_t host) noexcept;
	/**
	 * Processes the response to a get-predecessor request to fetch a host's
	 * predecessor.
	 * @param key stores predecessor's identifier
	 * @return message length on success, 0 on error
	 */
	unsigned int processGetPredecessorResponse(uint64_t &key) const noexcept;
	/**
	 * Prepares and executes a get-predecessor request to fetch a host's
	 * predecessor.
	 * @param host host's identifier
	 * @param key stores the host's predecessor
	 * @return true on success, false on error (request denied by the host)
	 */
	bool getPredecessorRequest(uint64_t host, uint64_t &key);
	//-----------------------------------------------------------------
	/**
	 * Creates a set-predecessor request to update a host's predecessor.
	 * @param host host's identifier
	 * @param key new predecessor's identifier
	 * @return message length on success, 0 on error
	 */
	unsigned int createSetPredecessorRequest(uint64_t host,
			uint64_t key) noexcept;
	/**
	 * Processes the response to a set-predecessor request to update a host's
	 * predecessor.
	 * @param key expected predecessor's identifier
	 * @return message length on success, 0 on error
	 */
	unsigned int processSetPredecessorResponse(uint64_t key) const noexcept;
	/**
	 * Prepares and executes a set-predecessor request to update a host's
	 * predecessor.
	 * @param host host's identifier
	 * @param key new predecessor's identifier
	 * @return true on success, false on error (request denied by the host)
	 */
	bool setPredecessorRequest(uint64_t host, uint64_t key);
	//-----------------------------------------------------------------
	/**
	 * Creates a get-successor request to fetch a host's successor.
	 * @param host host's identifier
	 * @return message length on success, 0 on error
	 */
	unsigned int createGetSuccessorRequest(uint64_t host) noexcept;
	/**
	 * Processes the response to a get-successor request to fetch a host's
	 * successor.
	 * @param key stores successor's identifier
	 * @return message length on success, 0 on error
	 */
	unsigned int processGetSuccessorResponse(uint64_t &key) const noexcept;
	/**
	 * Prepares and executes a get-successor request to fetch a host's
	 * successor.
	 * @param host host's identifier
	 * @param key stores successor's identifier
	 * @return true on success, false on error (request denied by the host)
	 */
	bool getSuccessorRequest(uint64_t host, uint64_t &key);
	//-----------------------------------------------------------------
	/**
	 * Creates a set-successor request to update a host's successor.
	 * @param host host's identifier
	 * @param key new successor's identifier
	 * @return message length on success, 0 on error
	 */
	unsigned int createSetSuccessorRequest(uint64_t host, uint64_t key) noexcept;
	/**
	 * Processes the response to a set-successor request to update a host's
	 * successor.
	 * @param key expected successor's identifier
	 * @return message length on success, 0 on error
	 */
	unsigned int processSetSuccessorResponse(uint64_t key) const noexcept;
	/**
	 * Prepares and executes a set-successor request to update a host's
	 * successor.
	 * @param host host's identifier
	 * @param key new successor's identifier
	 * @return true on success, false on error (request denied by the host)
	 */
	bool setSuccessorRequest(uint64_t host, uint64_t key);
	//-----------------------------------------------------------------
	/**
	 * Creates a get-finger request to fetch a host's finger table entry.
	 * @param host host's identifier
	 * @param index finger table's index
	 * @return message length on success, 0 on error
	 */
	unsigned int createGetFingerRequest(uint64_t host, uint32_t index) noexcept;
	/**
	 * Processes the response to a get-finger request to fetch a host's finger
	 * table entry.
	 * @param index finger table's index
	 * @param key stores finger table entry's value
	 * @return message length on success, 0 on error
	 */
	unsigned int processGetFingerResponse(uint32_t index,
			uint64_t &key) const noexcept;
	/**
	 * Prepares and executes a get-finger request to fetch a host's finger
	 * table entry.
	 * @param host host's identifier
	 * @param index finger table's index
	 * @param key stores finger table entry's value
	 * @return true on success, false on error (request denied by the host)
	 */
	bool getFingerRequest(uint64_t host, uint32_t index, uint64_t &key);
	//-----------------------------------------------------------------
	/**
	 * Creates a set-finger request to update a host's finger table.
	 * @param host host's identifier
	 * @param index finger table's index
	 * @param key new value at the given index
	 * @return message length on success, 0 on error
	 */
	unsigned int createSetFingerRequest(uint64_t host, uint32_t index,
			uint64_t key) noexcept;
	/**
	 * Processes the response to a set-finger request to update a host's
	 * finger table.
	 * @param index finger table's index
	 * @param key expected value at the given index
	 * @return message length on success, 0 on error
	 */
	unsigned int processSetFingerResponse(uint32_t index,
			uint64_t key) const noexcept;
	/**
	 * Prepares and executes a set-finger request to update a host's finger
	 * table.
	 * @param host host's identifier
	 * @param index finger table's index
	 * @param key new value at the given index
	 * @return true on success, false on error (request denied by the host)
	 */
	bool setFingerRequest(uint64_t host, uint32_t index, uint64_t key);
	//-----------------------------------------------------------------
	/**
	 * Creates a get-neighbours request to fetch a host's immediate neighbors.
	 * @param host host's identifier
	 * @return message length on success, 0 on error
	 */
	unsigned int createGetNeighboursRequest(uint64_t host) noexcept;
	/**
	 * Processes the response to a get-neighbours request to fetch a host's
	 * immediate neighbors.
	 * @param predecessor stores prdecessor's identifier
	 * @param successor stores successor's identifier
	 * @return message length on success, 0 on error
	 */
	unsigned int processGetNeighboursResponse(uint64_t &predecessor,
			uint64_t &successor) const noexcept;
	/**
	 * Prepares and executes a get-neighbours request to fetch a host's
	 * immediate neighbors.
	 * @param host host's identifier
	 * @param predecessor stores predecessor's identifier
	 * @param successor stores successor's identifier
	 * @return true on success, false on error (request denied by the host)
	 */
	bool getNeighboursRequest(uint64_t host, uint64_t &predecessor,
			uint64_t &successor);
	//-----------------------------------------------------------------
	/**
	 * Creates a notify request to inform a host about predecessor change.
	 * @param host host's identifier
	 * @param predecessor new predecessor's identifier
	 * @return message length on success, 0 on error
	 */
	unsigned int createNotifyRequest(uint64_t host,
			uint64_t predecessor) noexcept;
	/**
	 * Processes the response to a notify request to inform a host about
	 * predecessor change.
	 * @return message length on success, 0 on error
	 */
	unsigned int processNotifyResponse() const noexcept;
	/**
	 * Prepares and executes a notify request to inform a host about predecessor
	 * change.
	 * @param host host's identifier
	 * @param predecessor new predecessor's identifier
	 * @return true on success, false on error (request denied by the host)
	 */
	bool notifyRequest(uint64_t host, uint64_t predecessor);
	//-----------------------------------------------------------------
	/**
	 * Creates a find-successor request to recursively find a key's successor.
	 * @param host bootstrap node's identifier
	 * @param uid key's value
	 * @return message length on success, 0 on error
	 */
	unsigned int createFindSuccessorRequest(uint64_t host,
			uint64_t uid) noexcept;
	/**
	 * Processes the response to a find-successor request to recursively find a
	 * key's successor.
	 * @param uid key's value
	 * @param successor stores the key's successor
	 * @return message length on success, 0 on error
	 */
	unsigned int processFindSuccessorResponse(uint64_t uid,
			uint64_t &successor) const noexcept;
	/**
	 * Prepares and executes a find-successor request to recursively find a
	 * key's successor.
	 * @param host bootstrap node's identifier
	 * @param uid key's value
	 * @param key stores the key's successor
	 * @return true on success, false on error (request denied by the host)
	 */
	bool findSuccessorRequest(uint64_t host, uint64_t uid, uint64_t &successor);
	//-----------------------------------------------------------------
	/**
	 * Creates a ping request.
	 * @param host target's identifier
	 * @return message length on success, 0 on error
	 */
	unsigned int createPingRequest(uint64_t host) noexcept;
	/**
	 * Processes the response to a ping request.
	 * @return message length on success, 0 on error
	 */
	unsigned int processPingRequest() const noexcept;
	/**
	 * Prepares and executes a ping request.
	 * @param host target's identifier
	 * @return true on success, false on error (request denied by the host)
	 */
	bool pingRequest(uint64_t host);
	//-----------------------------------------------------------------
	/**
	 * Creates a map request.
	 * @param host entry point's identifier
	 * @return message length on success, 0 on error
	 */
	unsigned int createMapRequest(uint64_t host) noexcept;
	/**
	 * Processes the response to a map request.
	 * @return message length on success, 0 on error
	 */
	unsigned int processMapRequest() const noexcept;
	/**
	 * Prepares and executes a map request.
	 * @param host entry point's identifier
	 * @return true on success, false on error (request denied by the host)
	 */
	bool mapRequest(uint64_t host);
};

} /* namespace wanhive */

#endif /* WH_SERVER_CORE_OVERLAYPROTOCOL_H_ */

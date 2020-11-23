/*
 * OverlayProtocol.h
 *
 * Protocol extension for the Wanhive overlay network clients
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_OVERLAY_OVERLAYPROTOCOL_H_
#define WH_SERVER_OVERLAY_OVERLAYPROTOCOL_H_
#include "../../hub/Protocol.h"
#include "OverlayHubInfo.h"

namespace wanhive {
/**
 * Complete wanhive overlay network protocol implementation
 * Only blocking IO supported, hence the underlying socket must block.
 * Thread safe at class level
 */
class OverlayProtocol: public Protocol {
public:
	OverlayProtocol() noexcept;
	~OverlayProtocol();
	//-----------------------------------------------------------------
	/**
	 * <id> = identifier of the remote host
	 */
	unsigned int createDescribeRequest(uint64_t id) noexcept;
	unsigned int processDescribeResponse(OverlayHubInfo &info) noexcept;
	bool describeRequest(uint64_t id, OverlayHubInfo &info);

	unsigned int createGetPredecessorRequest(uint64_t id) noexcept;
	unsigned int processGetPredecessorResponse(uint64_t &key) noexcept;
	bool getPredecessorRequest(uint64_t id, uint64_t &key);

	unsigned int createSetPredecessorRequest(uint64_t id, uint64_t key) noexcept;
	unsigned int processSetPredecessorResponse(uint64_t key) noexcept;
	bool setPredecessorRequest(uint64_t id, uint64_t key);

	unsigned int createGetSuccessorRequest(uint64_t id) noexcept;
	unsigned int processGetSuccessorResponse(uint64_t &key) noexcept;
	bool getSuccessorRequest(uint64_t id, uint64_t &key);

	unsigned int createSetSuccessorRequest(uint64_t id, uint64_t key) noexcept;
	unsigned int processSetSuccessorResponse(uint64_t key) noexcept;
	bool setSuccessorRequest(uint64_t id, uint64_t key);

	unsigned int createGetFingerRequest(uint64_t id, uint32_t index) noexcept;
	unsigned int processGetFingerResponse(uint32_t index,
			uint64_t &key) noexcept;
	bool getFingerRequest(uint64_t id, uint32_t index, uint64_t &key);

	unsigned int createSetFingerRequest(uint64_t id, uint32_t index,
			uint64_t key) noexcept;
	unsigned int processSetFingerResponse(uint32_t index, uint64_t key) noexcept;
	bool setFingerRequest(uint64_t id, uint32_t index, uint64_t key);

	unsigned int createGetNeighboursRequest(uint64_t id) noexcept;
	unsigned int processGetNeighboursResponse(uint64_t &predecessor,
			uint64_t &successor) noexcept;
	bool getNeighboursRequest(uint64_t id, uint64_t &predecessor,
			uint64_t &successor);

	unsigned int createNotifyRequest(uint64_t id, uint64_t predecessor) noexcept;
	unsigned int processNotifyResponse() noexcept;
	bool notifyRequest(uint64_t id, uint64_t predecessor);

	unsigned int createFindSuccessorRequest(uint64_t id, uint64_t uid) noexcept;
	unsigned int processFindSuccessorResponse(uint64_t uid,
			uint64_t &key) noexcept;
	bool findSuccessorRequest(uint64_t id, uint64_t uid, uint64_t &key);

	unsigned int createPingRequest(uint64_t id) noexcept;
	unsigned int processPingRequest() noexcept;
	bool pingRequest(uint64_t id);

	unsigned int createMapRequest(uint64_t id) noexcept;
	unsigned int processMapRequest() noexcept;
	bool mapRequest(uint64_t id);
};

} /* namespace wanhive */

#endif /* WH_SERVER_OVERLAY_OVERLAYPROTOCOL_H_ */

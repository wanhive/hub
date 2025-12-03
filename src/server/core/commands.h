/*
 * commands.h
 *
 * Overlay network commands
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_CORE_COMMANDS_H_
#define WH_SERVER_CORE_COMMANDS_H_
#include "../../util/commands.h"

namespace wanhive {
/**
 * Enumeration of commands
 */
enum WhpDhtCommand {
	WH_DHT_CMD_NULL = WH_CMD_NULL, /**< null command */
	WH_DHT_CMD_BASIC = WH_CMD_BASIC, /**< basic command */
	WH_DHT_CMD_MULTICAST = WH_CMD_MULTICAST,/**< Pub/Sub command */
	WH_DHT_CMD_NODE, /**< node management command */
	WH_DHT_CMD_OVERLAY /**< network management command */
};

/**
 * Enumeration of qualifiers/functions
 */
enum WhpDhtQualifier {
	//WH_DHT_CMD_NULL
	WH_DHT_QLF_NULL = WH_QLF_NULL, /**< null */
	WH_DHT_QLF_IDENTIFY = WH_QLF_IDENTIFY, /**< identification */
	WH_DHT_QLF_AUTHENTICATE = WH_QLF_AUTHENTICATE,/**< authentication */
	WH_DHT_QLF_DESCRIBE = WH_QLF_DESCRIBE, /**< hub statistics */
	//WH_DHT_CMD_BASIC
	WH_DHT_QLF_REGISTER = WH_QLF_REGISTER, /**< registration */
	WH_DHT_QLF_TOKEN = WH_QLF_TOKEN, /**< session key */
	WH_DHT_QLF_FINDROOT = WH_QLF_FINDROOT, /**< root host */
	WH_DHT_QLF_BOOTSTRAP = WH_QLF_BOOTSTRAP, /**< bootstrap nodes */
	//WH_DHT_CMD_MULTICAST
	WH_DHT_QLF_PUBLISH = WH_QLF_PUBLISH, /**< publish */
	WH_DHT_QLF_SUBSCRIBE = WH_QLF_SUBSCRIBE, /**< subscribe */
	WH_DHT_QLF_UNSUBSCRIBE = WH_QLF_UNSUBSCRIBE, /**< exit a subscription */
	//WH_DHT_CMD_NODE
	WH_DHT_QLF_GETPREDECESSOR = 0, /**< get predecessor */
	WH_DHT_QLF_SETPREDECESSOR = 1, /**< set predecessor */
	WH_DHT_QLF_GETSUCCESSOR = 2, /**< get successor */
	WH_DHT_QLF_SETSUCCESSOR = 3, /**< set successor */
	WH_DHT_QLF_GETFINGER = 4, /**< get a finger table entry */
	WH_DHT_QLF_SETFINGER = 5, /**< set a finger table entry */
	WH_DHT_QLF_GETNEIGHBOURS = 6, /**< get immediate neighbors */
	WH_DHT_QLF_NOTIFY = 7, /**< notify successor */
	//WH_DHT_CMD_OVERLAY
	WH_DHT_QLF_FINDSUCCESSOR = 0, /**< find successor */
	WH_DHT_QLF_PING = 1, /**< ping the host */
	WH_DHT_QLF_MAP = 2 /**< map function */
};

/**
 * Enumeration of status codes
 */
enum WhpDhtStatus {
	WH_DHT_AQLF_REJECTED = WH_AQLF_REJECTED,/**< request rejected */
	WH_DHT_AQLF_ACCEPTED = WH_AQLF_ACCEPTED,/**< request accepted */
	WH_DHT_AQLF_REQUEST = WH_AQLF_REQUEST /**< request initiated */
};

}  // namespace wanhive

#endif /* WH_SERVER_CORE_COMMANDS_H_ */

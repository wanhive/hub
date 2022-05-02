/*
 * commands.h
 *
 * Enumerations of command, qualifier and status flags
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_COMMANDS_H_
#define WH_UTIL_COMMANDS_H_

namespace wanhive {

/**
 * Enumeration of the basic commands
 */
enum WhpCommand {
	WH_CMD_NULL = 0, /**< Null command */
	WH_CMD_BASIC = 1, /**< Basic command */
	WH_CMD_MULTICAST = 2/**< Multicast command */
};

/**
 * Enumeration of basic qualifiers
 */
enum WhpQualifier {
	//WH_CMD_NULL
	WH_QLF_NULL = 0, /**< Null qualifier */
	WH_QLF_IDENTIFY = 1, /**< Identification request */
	WH_QLF_AUTHENTICATE = 2,/**< Authentication request */
	WH_QLF_DESCRIBE = 127, /**< Describe request */
	//WH_CMD_BASIC
	WH_QLF_REGISTER = 0, /**< Registration request */
	WH_QLF_GETKEY = 1, /**< Session key request */
	WH_QLF_FINDROOT = 2, /**< Root identification request */
	WH_QLF_BOOTSTRAP = 3, /**< Bootstrap request */
	//WH_CMD_MULTICAST
	WH_QLF_PUBLISH = 0, /**< Publish request */
	WH_QLF_SUBSCRIBE = 1, /**< Subscribe request */
	WH_QLF_UNSUBSCRIBE = 2 /**< Unsubscribe request */
};

/**
 * Enumeration of basic status codes
 */
enum WhpStatus {
	WH_AQLF_REJECTED = 0,/**< Rejected status */
	WH_AQLF_ACCEPTED = 1,/**< Accepted status */
	WH_AQLF_REQUEST = 127/**< Request status */
};

}  // namespace wanhive

#endif /* WH_UTIL_COMMANDS_H_ */

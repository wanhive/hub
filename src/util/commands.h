/*
 * commands.h
 *
 * Bare minimum commands for Wanhive protocol
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
 * Bare minimum enumerations of command, qualifier and status flags
 */
enum WhpCommand {
	WH_CMD_NULL = 0, WH_CMD_BASIC = 1, WH_CMD_MULTICAST = 2
};

enum WhpQualifier {
	//WH_CMD_NULL
	WH_QLF_NULL = 0,
	WH_QLF_IDENTIFY = 1,
	WH_QLF_AUTHENTICATE = 2,
	WH_QLF_DESCRIBE = 127,
	//WH_CMD_BASIC
	WH_QLF_REGISTER = 0,
	WH_QLF_GETKEY = 1,
	WH_QLF_FINDROOT = 2,
	WH_QLF_BOOTSTRAP = 3,
	//WH_CMD_MULTICAST
	WH_QLF_PUBLISH = 0,
	WH_QLF_SUBSCRIBE = 1,
	WH_QLF_UNSUBSCRIBE = 2
};

enum WhpStatus {
	WH_AQLF_REJECTED = 0, WH_AQLF_ACCEPTED = 1, WH_AQLF_REQUEST = 127
};

}  // namespace wanhive

#endif /* WH_UTIL_COMMANDS_H_ */

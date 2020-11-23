/*
 * CommandLine.h
 *
 * C++ iostream error handling routines
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_COMMANDLINE_H_
#define WH_BASE_COMMON_COMMANDLINE_H_

namespace wanhive {
/**
 * C++ iostream error handling
 */
class CommandLine {
public:
	CommandLine();
	~CommandLine();
	/*
	 * Returns true if an error occurred while processing the input at
	 * std::cin and consumes the cause of the error.
	 */
	static bool inputError(bool verbose = true);
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_COMMANDLINE_H_ */

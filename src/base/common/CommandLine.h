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
	/**
	 * Default constructor
	 */
	CommandLine();
	/**
	 * Destructor
	 */
	~CommandLine();
	/**
	 * Consumes the cause of error while processing the input at std::cin.
	 * @param verbose if true then a message is printed to std::cerr on error,
	 * if false then no message is printed.
	 * @return true if an error occurred, false otherwise
	 */
	static bool inputError(bool verbose = true);
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_COMMANDLINE_H_ */

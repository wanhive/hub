/**
 * @file CommandLine.h
 *
 * C++ iostream error handler
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_COMMANDLINE_H_
#define WH_BASE_COMMON_COMMANDLINE_H_

/*! @namespace wanhive */
namespace wanhive {
/**
 * C++ iostream error handler
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
	 * @param verbose set true to print a message to std::cerr on error, set
	 * false for silent operation.
	 * @return true if an error occurred, false otherwise
	 */
	static bool inputError(bool verbose = true);
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_COMMANDLINE_H_ */

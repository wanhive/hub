/*
 * CommandLine.cpp
 *
 * C++ iostream error handler
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "CommandLine.h"
#include "Exception.h"
#include <iostream>
#include <limits>

namespace wanhive {

CommandLine::CommandLine() {

}

CommandLine::~CommandLine() {

}

bool CommandLine::inputError(bool verbose) {
	try {
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			if (verbose) {
				std::cerr << "Invalid input" << std::endl;
			}
			return true;
		} else {
			return false;
		}
	} catch (...) {
		throw Exception(EX_OPERATION);
	}
}

} /* namespace wanhive */

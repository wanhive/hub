/**
 * @file Manager.h
 *
 * Real stuff happens here
 *
 *
 * Copyright (C) 2020 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_APP_MANAGER_H_
#define WH_APP_MANAGER_H_
#include "../wanhive.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Real stuff happens here
 */
class Manager {
public:
	/**
	 * Default constructor: does nothing
	 */
	Manager() noexcept;
	/**
	 * Destructor
	 */
	~Manager();
	/**
	 * Interactively executes the main application.
	 * @param argc command line arguments count
	 * @param argv command line arguments
	 */
	static void execute(int argc, char *argv[]) noexcept;
private:
	static int parseOptions(int argc, char *argv[]) noexcept;
	static void processOptions() noexcept;
	//-----------------------------------------------------------------
	static void executeHub() noexcept;
	static void runSettingsManager() noexcept;
	static void runCommandTest() noexcept;
	static void runNetworkTest() noexcept;
	static void runComponentsTest() noexcept;
	//-----------------------------------------------------------------
	static void installSignals();
	static void restoreSignals();
	static void shutdown(int signum) noexcept;
	//-----------------------------------------------------------------
	static void printHelp(FILE *stream) noexcept;
	static void printVersion(FILE *stream) noexcept;
	static void printUsage(FILE *stream) noexcept;
	static void printContact(FILE *stream) noexcept;
	static void clear() noexcept;
public:
	struct Context {
		const char *program;
		bool menu;
		char type;
		unsigned long long uid;
		const char *config;
		Hub *hub;
	};
private:
	static Context ctx;
};

} /* namespace wanhive */

#endif /* WH_APP_MANAGER_H_ */

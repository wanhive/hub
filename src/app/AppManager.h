/*
 * AppManager.h
 *
 * Real stuff happens here
 *
 *
 * Copyright (C) 2020 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_APP_APPMANAGER_H_
#define WH_APP_APPMANAGER_H_
#include "../wanhive.h"
#include <cstdio>

namespace wanhive {
/**
 * Real stuff happens here
 */
class AppManager {
public:
	AppManager() noexcept;
	~AppManager();
	static void execute(int argc, char *const*argv) noexcept;
private:
	static void printHelp(FILE *stream) noexcept;
	static int parseOptions(int argc, char *const*argv) noexcept;
	static void processOptions() noexcept;
	//-----------------------------------------------------------------
	static void executeHub() noexcept;
	static void runSettingsManager() noexcept;
	static void runCommandTest() noexcept;
	static void runNetworkTest() noexcept;
	static void runComponentsTest() noexcept;
	static void f() noexcept;
	//-----------------------------------------------------------------
	static void installSignals();
	static void shutdown(int signum) noexcept;
private:
	static const char *programName;
	static bool menu;
	static char hubType;
	static unsigned long long hubId;
	static const char *configPath;
	static Hub *hub;
};

} /* namespace wanhive */

#endif /* WH_APP_APPMANAGER_H_ */

/*
 * ConfigTool.h
 *
 * Settings generator
 *
 *
 * Copyright (C) 2020 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_APP_CONFIGTOOL_H_
#define WH_APP_CONFIGTOOL_H_

namespace wanhive {
/**
 * Command line tool for generating basic settings.
 */
class ConfigTool {
public:
	/**
	 * Default constructor
	 */
	ConfigTool() noexcept;
	/**
	 * Destructor
	 */
	~ConfigTool();
	/**
	 * Interactively generates application settings.
	 */
	void execute() noexcept;
private:
	static void generateKeyPair();
	static void manageHosts();
	static void generateVerifier();
private:
	static void createDummyHostsFile(const char *path);
};

} /* namespace wanhive */

#endif /* WH_APP_CONFIGTOOL_H_ */

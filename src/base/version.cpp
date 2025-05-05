/*
 * version.cpp
 *
 * Package information
 *
 *
 * Copyright (C) 2025 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "version.h"

extern "C" const char* wh_pkg_version() {
#ifdef WH_RELEASE_VERSION
	return WH_RELEASE_VERSION;
#else
	return "";
#endif
}

extern "C" const char* wh_lib_version() {
#ifdef WH_LIB_VERSION
	return WH_LIB_VERSION;
#else
	return "";
#endif
}


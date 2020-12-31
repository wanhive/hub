/*
 * version.h
 *
 * Package information
 *
 *
 * Copyright (C) 2020 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_APP_VERSION_H_
#define WH_APP_VERSION_H_

#undef WH_PRODUCT_NAME
#undef WH_RELEASE_NAME
#undef WH_RELEASE_YEAR
#undef WH_RELEASE_AUTHOR
#undef WH_LICENSE_TEXT

#define WH_PRODUCT_NAME "Wanhive Hub"
#define WH_RELEASE_NAME ""
#define WH_RELEASE_YEAR "2021"
#define WH_RELEASE_AUTHOR "Wanhive Systems Private Limited"
#define WH_LICENSE_TEXT "Apache-2.0 http://www.apache.org/licenses/LICENSE-2.0"

#undef WH_RELEASE_VERSION
#undef WH_RELEASE_EMAIL
#undef WH_RELEASE_URL

#ifdef HAVE_CONFIG_H
#include <config.h>
#define WH_RELEASE_VERSION PACKAGE_VERSION
#define WH_RELEASE_EMAIL PACKAGE_BUGREPORT
#define WH_RELEASE_URL PACKAGE_URL
#else
#define WH_RELEASE_VERSION "VERSION"
#define WH_RELEASE_EMAIL "EMAIL"
#define WH_RELEASE_URL "URL"
#endif

#endif /* WH_APP_VERSION_H_ */

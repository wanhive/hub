/*
 * DynamicHub.cpp
 *
 * Dynamic hub
 *
 *
 * Copyright (C) 2026 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "DynamicHub.h"
#include "../base/common/Logger.h"

namespace wanhive {

DynamicHub::DynamicHub(unsigned long long uid, const char *path) noexcept :
		Hub { uid, path } {
	clear();
}

DynamicHub::~DynamicHub() {

}

Watcher* DynamicHub::bridge(int &sfd, bool blocking, int timeout) {
	Socket *local = nullptr;
	int socket = -1;
	try {
		local = Socket::pair(socket);
		if (blocking) {
			Network::setBlocking(socket, true);
			Network::setTimeout(socket, timeout, timeout);
		}
		attach(local, IO_WR, WATCHER_ACTIVE);
		sfd = socket;
		return local;
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		Network::close(socket);
		delete local;
		throw;
	}
}

void DynamicHub::configure(void *arg) {
	try {
		Hub::configure(arg);
		auto reload = getOptions().getBoolean("OPT", "reload");
		WH_LOG_DEBUG("\nRELOAD=%s\n", WH_BOOLF(reload));
		if (reload) {
			setup();
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		throw Exception(EX_OPERATION);
	}
}

void DynamicHub::cleanup() noexcept {
	clear();
	Hub::cleanup();
}

void DynamicHub::onInotification(unsigned long long uid,
		const InotifyEvent *event) noexcept {
	if (event->wd == -1) { //overflow notification
		return;
	}

	for (unsigned int i = 0; i < TRACKER_CNT; ++i) {
		if (tracker[i].id == event->wd) {
			tracker[i].events |= event->mask;
			refresh(i);
			break;
		}
	}
}

void DynamicHub::refresh(unsigned int index) noexcept {
	//REF: https://github.com/guard/guard/wiki/Analysis-of-inotify-events-for-different-editors
	if (tracker[index].events & IN_IGNORED) {
		//Associated file will no longer be monitored
		tracker[index].id = -1;
		tracker[index].events = 0;
	} else if (!(tracker[index].events & IN_CLOSE_WRITE)) { //Write-close
		return;
	} else if (!(tracker[index].events & (IN_MODIFY | IN_ATTRIB))) { //Modification
		//Closed without modification
		tracker[index].events = 0;
		return;
	} else {
		//Reset for next cycle
		tracker[index].events = 0;
	}
	//-----------------------------------------------------------------
	/*
	 * Reload the settings
	 */
	try {
		switch (tracker[index].context) {
		case Identity::CTX_OPTIONS:
			if (tracker[index].id != -1) {
				WH_LOG_INFO(
						"Configuration file has been modified (restart required)");
			} else {
				WH_LOG_WARNING("Configuration file has been ignored");
			}
			break;
		case Identity::CTX_HOSTS_DB:
			if (tracker[index].id != -1) {
				WH_LOG_INFO("Hosts database has been modified");
			} else {
				WH_LOG_WARNING("Hosts database has been ignored");
			}
			break;
		case Identity::CTX_HOSTS_FILE:
			if (tracker[index].id != -1) {
				WH_LOG_INFO("Hosts file has been modified");
				Identity::refresh(Identity::CTX_HOSTS_FILE);
			} else {
				WH_LOG_WARNING("Hosts file has been ignored");
			}
			break;
		case Identity::CTX_PKI_PRIVATE:
			if (tracker[index].id != -1) {
				WH_LOG_INFO("Private key file has been modified");
				Identity::refresh(Identity::CTX_PKI_PRIVATE);
			} else {
				WH_LOG_WARNING("Private key file has been ignored");
			}
			break;
		case Identity::CTX_PKI_PUBLIC:
			if (tracker[index].id != -1) {
				WH_LOG_INFO("Public key file has been modified");
				Identity::refresh(Identity::CTX_PKI_PUBLIC);
			} else {
				WH_LOG_WARNING("Public key file has been ignored");
			}
			break;
		case Identity::CTX_SSL_ROOT:
			if (tracker[index].id != -1) {
				WH_LOG_INFO(
						"SSL Root CA certificate has been modified (restart required)");
			} else {
				WH_LOG_WARNING("SSL Root CA certificate has been ignored");
			}
			break;
		case Identity::CTX_SSL_CERT:
			if (tracker[index].id != -1) {
				WH_LOG_INFO("SSL certificate has been modified");
				Identity::refresh(Identity::CTX_SSL_CERT);
			} else {
				WH_LOG_WARNING("SSL certificate has been ignored");
			}
			break;
		case Identity::CTX_SSL_PRIVATE:
			if (tracker[index].id != -1) {
				WH_LOG_INFO("SSL private key has been modified");
				Identity::refresh(Identity::CTX_SSL_PRIVATE);
			} else {
				WH_LOG_WARNING("SSL private key has been ignored");
			}
			break;
		default:
			WH_LOG_ERROR("Martian attack!");
			break;
		}
	} catch (const BaseException &e) {
		//Hub has been damaged, report problem and exit
		WH_LOG_EXCEPTION(e);
		exit(EXIT_FAILURE);
	}
}

void DynamicHub::setup() {
	try {
		//Events we are interested in: modify-> close
		const uint32_t events = IN_MODIFY | IN_ATTRIB | IN_CLOSE_WRITE;
		if (auto path = getPath(Identity::CTX_OPTIONS); path) {
			tracker[0].id = track(path, events);
			tracker[0].context = Identity::CTX_OPTIONS;
		}

		if (auto path = getPath(Identity::CTX_HOSTS_DB); path) {
			tracker[1].id = track(path, events);
			tracker[1].context = Identity::CTX_HOSTS_DB;
		}

		if (auto path = getPath(Identity::CTX_HOSTS_FILE); path) {
			tracker[2].id = track(path, events);
			tracker[2].context = Identity::CTX_HOSTS_FILE;
		}

		if (auto path = getPath(Identity::CTX_PKI_PRIVATE); path) {
			tracker[3].id = track(path, events);
			tracker[3].context = Identity::CTX_PKI_PRIVATE;
		}

		if (auto path = getPath(Identity::CTX_PKI_PUBLIC); path) {
			tracker[4].id = track(path, events);
			tracker[4].context = Identity::CTX_PKI_PUBLIC;
		}

		if (auto path = getPath(Identity::CTX_SSL_ROOT); path) {
			tracker[5].id = track(path, events);
			tracker[5].context = Identity::CTX_SSL_ROOT;
		}

		if (auto path = getPath(Identity::CTX_SSL_CERT); path) {
			tracker[6].id = track(path, events);
			tracker[6].context = Identity::CTX_SSL_CERT;
		}

		if (auto path = getPath(Identity::CTX_SSL_PRIVATE); path) {
			tracker[7].id = track(path, events);
			tracker[7].context = Identity::CTX_SSL_PRIVATE;
		}

	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void DynamicHub::clear() noexcept {
	for (unsigned int i = 0; i < TRACKER_CNT; ++i) {
		tracker[i] = { -1, -1, 0 };
	}
}

} /* namespace wanhive */

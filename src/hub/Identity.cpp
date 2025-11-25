/*
 * Identity.cpp
 *
 * Hub's identity
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Identity.h"
#include "../base/System.h"
#include "../base/Timer.h"
#include "../base/common/Exception.h"
#include "../base/common/Logger.h"
#include "../base/ds/MersenneTwister.h"
#include <cstdlib>
#include <cstring>

#ifndef WH_CONF_BASE
#define WH_CONF_BASE "~/.config/wanhive"
#endif

#ifndef WH_CONF_SYSTEM_BASE
#define WH_CONF_SYSTEM_BASE "/etc/wanhive"
#endif

#ifndef WH_CONF_FILE
#define WH_CONF_FILE "wanhive.conf"
#endif

#define WH_CONF_PATH WH_CONF_BASE "/" WH_CONF_FILE
#define WH_CONF_SYSTEM_PATH WH_CONF_SYSTEM_BASE "/" WH_CONF_FILE
#define WH_TEST_DIR WH_CONF_BASE "/testdata"

namespace wanhive {
//=================================================================
const char Identity::CONF_FILE_NAME[] = WH_CONF_FILE;
const char Identity::CONF_PATH[] = WH_CONF_PATH;
const char Identity::CONF_SYSTEM_PATH[] = WH_CONF_SYSTEM_PATH;
//=================================================================
Identity::Identity(const char *path) noexcept {
	paths.config = path ? strdup(path) : nullptr;
}

Identity::~Identity() {
	delete instanceId;
	free(paths.config);
	free(paths.configurationFile);
	free(paths.hostsDB);
	free(paths.hostsFile);
	free(paths.privateKey);
	free(paths.publicKey);
	free(paths.sslRoot);
	free(paths.sslCertificate);
	free(paths.sslHostKey);
}

void Identity::initialize() {
	try {
		generateInstanceId();
		loadConfiguration();
		loadHosts();
		loadKeys();
		loadSSL();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

const Configuration& Identity::getConfiguration() const noexcept {
	return properties;
}

const PKI* Identity::getPKI() const noexcept {
	if (auth.enabled) {
		return &auth.pki;
	} else {
		return nullptr;
	}
}

bool Identity::verifyHost() const noexcept {
	return auth.verify;
}

SSLContext* Identity::getSSLContext() noexcept {
	if (ssl.enabled) {
		return &ssl.ctx;
	} else {
		return nullptr;
	}
}

bool Identity::generateNonce(Hash &hash, uint64_t salt, uint64_t id,
		Digest *nonce) const noexcept {
	if (instanceId) {
		instanceId->generateNonce(hash, salt, id, nonce);
		return true;
	} else {
		return false;
	}
}

bool Identity::verifyNonce(Hash &hash, uint64_t salt, uint64_t id,
		const Digest *nonce) const noexcept {
	if (instanceId) {
		return instanceId->verifyNonce(hash, salt, id, nonce);
	} else {
		return false;
	}
}

void Identity::getAddress(uint64_t uid, NameInfo &ni) {
	if (hosts.get(uid, ni) == 0) {
		return;
	} else {
		throw Exception(EX_OPERATION);
	}
}

unsigned int Identity::getIdentifiers(unsigned long long nodes[],
		unsigned int count, int type) noexcept {
	auto n = count;
	if (hosts.list(nodes, n, type) == 0) {
		return n;
	} else {
		return 0;
	}
}

unsigned int Identity::getIdentifiers(const char *section, const char *option,
		unsigned long long nodes[], unsigned int count) noexcept {
	//--------------------------------------------------------------------------
	//Get these boundary conditions out of the way
	if (!nodes || !count) {
		return 0;
	}
	//--------------------------------------------------------------------------
	memset(nodes, 0, count * sizeof(unsigned long long));
	auto filename = properties.getPathName(section, option, nullptr);
	if (Storage::testFile(filename) != 1) {
		free(filename);
		return 0;
	}

	auto fp = Storage::openStream(filename, "r");
	free(filename);
	if (!fp) {
		return 0;
	}
	//--------------------------------------------------------------------------
	unsigned int i = 0;
	while (i < count && fscanf(fp, "%llu", &nodes[i]) == 1) {
		++i;
	}
	Storage::closeStream(fp);
	//--------------------------------------------------------------------------
	//Apply Fisher–Yates shuffle algorithm
	MersenneTwister mt(Timer::timeSeed());
	for (unsigned int x = 0; x < i; ++x) {
		auto j = mt.next() % (x + 1);
		auto tmp = nodes[j];
		nodes[j] = nodes[x];
		nodes[x] = tmp;
	}
	return i;
}

const char* Identity::dataPathName(int context) const noexcept {
	switch (context) {
	case CTX_CONFIGURATION:
		return paths.configurationFile;
	case CTX_HOSTS_DB:
		return paths.hostsDB;
	case CTX_HOSTS_FILE:
		return paths.hostsFile;
	case CTX_PKI_PRIVATE:
		return paths.privateKey;
	case CTX_PKI_PUBLIC:
		return paths.publicKey;
	case CTX_SSL_ROOT:
		return paths.sslRoot;
	case CTX_SSL_CERTIFICATE:
		return paths.sslCertificate;
	case CTX_SSL_PRIVATE:
		return paths.sslHostKey;
	default:
		return nullptr;
	}
}

void Identity::reload(int context) {
	switch (context) {
	case CTX_CONFIGURATION:
		initialize();
		break;
	case CTX_HOSTS_DB:
		loadHostsDatabase();
		break;
	case CTX_HOSTS_FILE:
		loadHostsFile();
		break;
	case CTX_PKI_PRIVATE:
		loadPrivateKey();
		break;
	case CTX_PKI_PUBLIC:
		loadPublicKey();
		break;
	case CTX_SSL_ROOT:
		loadSSL();
		break;
	case CTX_SSL_CERTIFICATE:
		loadSSLCertificate();
		break;
	case CTX_SSL_PRIVATE:
		loadSSLHostKey();
		break;
	default:
		throw Exception(EX_ARGUMENT);
		break;
	}
}

void Identity::generateInstanceId() {
	try {
		delete instanceId;
		instanceId = nullptr;
		instanceId = new InstanceID();
		WH_LOG_INFO("Instance identifier generated");
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		throw Exception(EX_MEMORY);
	}
}

void Identity::loadConfiguration() {
	try {
		free(paths.configurationFile);
		paths.configurationFile = locateConfigurationFile();
		properties.clear();

		if (!paths.configurationFile) {
			WH_LOG_WARNING("No configuration file");
		} else if (properties.load(paths.configurationFile)) {
			WH_LOG_INFO("Configuration loaded from %s",
					paths.configurationFile);
		} else {
			WH_LOG_ERROR("Could not read the configuration file %s",
					paths.configurationFile);
			free(paths.configurationFile);
			paths.configurationFile = nullptr;
			throw Exception(EX_ARGUMENT);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Identity::loadHosts() {
	free(paths.hostsDB);
	free(paths.hostsFile);
	paths.hostsDB = properties.getPathName("HOSTS", "database");
	if (!paths.hostsDB) {
		paths.hostsFile = properties.getPathName("HOSTS", "file");
	} else {
		paths.hostsFile = nullptr;
	}
	//-----------------------------------------------------------------
	try {
		if (!paths.hostsDB && !paths.hostsFile) {
			WH_LOG_WARNING("No hosts file or database");
		} else if (paths.hostsDB) {
			loadHostsDatabase();
		} else {
			loadHostsFile();
		}
		WH_LOG_INFO("Hosts initialized");
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		free(paths.hostsDB);
		paths.hostsDB = nullptr;
		free(paths.hostsFile);
		paths.hostsFile = nullptr;
		throw;
	}
}

void Identity::loadKeys() {
	free(paths.privateKey);
	free(paths.publicKey);
	paths.privateKey = properties.getPathName("KEYS", "private");
	paths.publicKey = properties.getPathName("KEYS", "public");

	auth.verify = properties.getBoolean("KEYS", "verify");
	if (!auth.verify) {
		WH_LOG_WARNING("Host verification disabled");
	} else if (!paths.publicKey) {
		WH_LOG_WARNING("Host verification enabled but no public key");
		auth.verify = false;
	} else {
		WH_LOG_INFO("Host verification enabled");
	}
	//-----------------------------------------------------------------
	try {
		if (!paths.publicKey && !paths.privateKey) {
			WH_LOG_WARNING("Public key infrastructure disabled");
			auth.enabled = false;
			auth.verify = false;
		} else {
			auth.enabled = auth.pki.initialize(paths.privateKey,
					paths.publicKey);
			if (auth.enabled) {
				WH_LOG_INFO("Public key infrastructure enabled");
			} else {
				throw Exception(EX_SECURITY);
			}
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		auth.enabled = false;
		auth.verify = false;
		free(paths.privateKey);
		paths.privateKey = nullptr;
		free(paths.publicKey);
		paths.publicKey = nullptr;
		throw;
	}
}

void Identity::loadSSL() {
	ssl.enabled = properties.getBoolean("SSL", "enable");
	if (!ssl.enabled) {
		WH_LOG_WARNING("SSL/TLS disabled");
		return;
	}

	free(paths.sslRoot);
	free(paths.sslCertificate);
	free(paths.sslHostKey);
	paths.sslRoot = properties.getPathName("SSL", "trust");
	paths.sslCertificate = properties.getPathName("SSL", "certificate");
	paths.sslHostKey = properties.getPathName("SSL", "key");
	//-----------------------------------------------------------------
	try {
		ssl.ctx.initialize(paths.sslCertificate, paths.sslHostKey);
		ssl.ctx.loadTrustedPaths(paths.sslRoot, nullptr);
		WH_LOG_INFO("SSL/TLS enabled");
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		free(paths.sslRoot);
		paths.sslRoot = nullptr;
		free(paths.sslCertificate);
		paths.sslCertificate = nullptr;
		free(paths.sslHostKey);
		paths.sslHostKey = nullptr;
		throw;
	}
}

void Identity::loadHostsDatabase() {
	try {
		if (!paths.hostsDB) {
			WH_LOG_WARNING("No hosts database");
		} else {
			//Load the database file from the disk in read-only mode
			hosts.open(paths.hostsDB, true);
			WH_LOG_DEBUG("Hosts loaded from %s", paths.hostsDB);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Identity::loadHostsFile() {
	try {
		if (!paths.hostsFile) {
			WH_LOG_WARNING("No hosts file");
		} else {
			//Load the hosts into in-memory database
			hosts.open(Hosts::IN_MEMORY);
			hosts.load(paths.hostsFile);
			WH_LOG_DEBUG("Hosts loaded from %s", paths.hostsFile);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Identity::loadPrivateKey() {
	try {
		if (!paths.privateKey) {
			WH_LOG_WARNING("No private key file");
		} else if (auth.pki.loadHostKey(paths.privateKey)) {
			WH_LOG_DEBUG("Private key loaded from %s", paths.privateKey);
			return;
		} else {
			throw Exception(EX_SECURITY);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Identity::loadPublicKey() {
	try {
		if (!paths.publicKey) {
			WH_LOG_WARNING("No public key file");
		} else if (auth.pki.loadPublicKey(paths.publicKey)) {
			WH_LOG_DEBUG("Public key loaded from %s", paths.publicKey);
			return;
		} else {
			throw Exception(EX_SECURITY);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Identity::loadSSLCertificate() {
	try {
		if (!ssl.enabled) {
			throw Exception(EX_OPERATION);
		} else if (!paths.sslCertificate) {
			WH_LOG_WARNING("No SSL certificate file");
			return;
		} else if (paths.sslHostKey
				&& strcmp(paths.sslCertificate, paths.sslHostKey) == 0) {
			ssl.ctx.initialize(paths.sslCertificate, paths.sslHostKey);
			WH_LOG_DEBUG("SSL/TLS certificate and private key loaded from %s",
					paths.sslCertificate);
		} else {
			ssl.ctx.initialize(paths.sslCertificate, nullptr);
			WH_LOG_DEBUG("SSL/TLS certificate loaded from %s",
					paths.sslCertificate);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Identity::loadSSLHostKey() {
	try {
		if (!ssl.enabled) {
			throw Exception(EX_OPERATION);
		} else if (!paths.sslHostKey) {
			WH_LOG_WARNING("No SSL private key file");
			return;
		} else {
			ssl.ctx.initialize(nullptr, paths.sslHostKey);
			WH_LOG_DEBUG("SSL/TLS private key loaded from %s",
					paths.sslHostKey);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

char* Identity::locateConfigurationFile() noexcept {
	try {
		//Path to configuration file supplied from the command line, take it as it is
		if (paths.config) {
			return Storage::expandPathName(paths.config);
		}

		//For resolution of default configuration file paths
		char buffer[PATH_MAX] = { };

		//STEP 1: Search in the current working directory
		System::currentWorkingDirectory(buffer, sizeof(buffer));
		strncat(buffer, "/" WH_CONF_FILE, sizeof(buffer) - strlen(buffer) - 1);
		if (Storage::testFile(buffer) == 1) {
			return strdup(buffer);
		}

		//STEP 2: Search in executable's directory
		System::executableDirectory(buffer, sizeof(buffer));
		strncat(buffer, "/" WH_CONF_FILE, sizeof(buffer) - strlen(buffer) - 1);
		if (Storage::testFile(buffer) == 1) {
			return strdup(buffer);
		}

		//STEP 3: Search in home
		auto cfgPath = Storage::expandPathName(WH_CONF_PATH);
		if (Storage::testFile(cfgPath) == 1) {
			return cfgPath;
		} else {
			free(cfgPath);
			cfgPath = nullptr;
		}

		//STEP 4: Search in /etc/wanhive
		if (Storage::testFile(WH_CONF_SYSTEM_PATH) == 1) {
			return strdup(WH_CONF_SYSTEM_PATH);
		}

		//All attempts exhausted
		return nullptr;
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return nullptr;
	}
}

} /* namespace wanhive */

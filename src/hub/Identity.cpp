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
#include "../base/common/Memory.h"
#include "../base/ds/MersenneTwister.h"
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
const char *Identity::CONF_FILE = (const char*) WH_CONF_FILE;
const char *Identity::CONF_PATH = (const char*) WH_CONF_PATH;
const char *Identity::CONF_SYSTEM_PATH = (const char*) WH_CONF_SYSTEM_PATH;
//=================================================================
Identity::Identity(const char *path) noexcept {
	paths.config = path ? WH_strdup(path) : nullptr;
}

Identity::~Identity() {
	delete instanceId;
	WH_free(paths.config);
	WH_free(paths.configurationFileName);
	WH_free(paths.hostsDatabaseName);
	WH_free(paths.hostsFileName);
	WH_free(paths.privateKeyFileName);
	WH_free(paths.publicKeyFileName);
	WH_free(paths.sslTrustedCertificateFileName);
	WH_free(paths.sslCertificateFileName);
	WH_free(paths.sslHostKeyFileName);
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
		throw Exception(EX_INVALIDOPERATION);
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
		WH_free(filename);
		return 0;
	}

	auto fp = Storage::openStream(filename, "r", false);
	WH_free(filename);
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

const char* Identity::getConfigurationFile() const noexcept {
	return paths.configurationFileName;
}

const char* Identity::getHostsDatabase() const noexcept {
	return paths.hostsDatabaseName;
}

const char* Identity::getHostsFile() const noexcept {
	return paths.hostsFileName;
}

const char* Identity::getPrivateKeyFile() const noexcept {
	return paths.privateKeyFileName;
}

const char* Identity::getPublicKeyFile() const noexcept {
	return paths.publicKeyFileName;
}

const char* Identity::getSSLTrustedCertificateFile() const noexcept {
	return paths.sslTrustedCertificateFileName;
}

const char* Identity::getSSLCertificateFile() const noexcept {
	return paths.sslCertificateFileName;
}

const char* Identity::getSSLHostKeyFile() const noexcept {
	return paths.sslHostKeyFileName;
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
		throw Exception(EX_ALLOCFAILED);
	}
}

void Identity::loadConfiguration() {
	try {
		WH_free(paths.configurationFileName);
		paths.configurationFileName = locateConfigurationFile();
		properties.clear();

		if (!paths.configurationFileName) {
			WH_LOG_WARNING("No configuration file");
		} else if (properties.load(paths.configurationFileName)) {
			WH_LOG_INFO("Configuration loaded from %s",
					paths.configurationFileName);
		} else {
			WH_LOG_ERROR("Could not read the configuration file %s",
					paths.configurationFileName);
			WH_free(paths.configurationFileName);
			paths.configurationFileName = nullptr;
			throw Exception(EX_INVALIDPARAM);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Identity::loadHosts() {
	WH_free(paths.hostsDatabaseName);
	WH_free(paths.hostsFileName);
	paths.hostsDatabaseName = properties.getPathName("HOSTS", "hostsDb");
	if (!paths.hostsDatabaseName) {
		paths.hostsFileName = properties.getPathName("HOSTS", "hostsFile");
	} else {
		paths.hostsFileName = nullptr;
	}
	//-----------------------------------------------------------------
	try {
		if (!paths.hostsDatabaseName && !paths.hostsFileName) {
			WH_LOG_WARNING("No hosts file or database");
		} else if (paths.hostsDatabaseName) {
			loadHostsDatabase();
		} else {
			loadHostsFile();
		}
		WH_LOG_INFO("Hosts initialized");
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		WH_free(paths.hostsDatabaseName);
		paths.hostsDatabaseName = nullptr;
		WH_free(paths.hostsFileName);
		paths.hostsFileName = nullptr;
		throw;
	}
}

void Identity::loadKeys() {
	WH_free(paths.privateKeyFileName);
	WH_free(paths.publicKeyFileName);
	paths.privateKeyFileName = properties.getPathName("KEYS", "privateKey");
	paths.publicKeyFileName = properties.getPathName("KEYS", "publicKey");

	auth.verify = properties.getBoolean("KEYS", "verifyHost");
	if (!auth.verify) {
		WH_LOG_WARNING("Host verification disabled");
	} else if (!paths.publicKeyFileName) {
		WH_LOG_WARNING("Host verification enabled but no public key");
		auth.verify = false;
	} else {
		WH_LOG_INFO("Host verification enabled");
	}
	//-----------------------------------------------------------------
	try {
		if (!paths.publicKeyFileName && !paths.privateKeyFileName) {
			WH_LOG_WARNING("Public key infrastructure disabled");
			auth.enabled = false;
			auth.verify = false;
		} else {
			auth.enabled = auth.pki.initialize(paths.privateKeyFileName,
					paths.publicKeyFileName);
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
		WH_free(paths.privateKeyFileName);
		paths.privateKeyFileName = nullptr;
		WH_free(paths.publicKeyFileName);
		paths.publicKeyFileName = nullptr;
		throw;
	}
}

void Identity::loadSSL() {
	ssl.enabled = properties.getBoolean("SSL", "enable");
	if (!ssl.enabled) {
		WH_LOG_WARNING("SSL/TLS disabled");
		return;
	}

	WH_free(paths.sslTrustedCertificateFileName);
	WH_free(paths.sslCertificateFileName);
	WH_free(paths.sslHostKeyFileName);
	paths.sslTrustedCertificateFileName = properties.getPathName("SSL",
			"trust");
	paths.sslCertificateFileName = properties.getPathName("SSL", "certificate");
	paths.sslHostKeyFileName = properties.getPathName("SSL", "key");
	//-----------------------------------------------------------------
	try {
		ssl.ctx.initialize(paths.sslCertificateFileName,
				paths.sslHostKeyFileName);
		ssl.ctx.loadTrustedPaths(paths.sslTrustedCertificateFileName, nullptr);
		WH_LOG_INFO("SSL/TLS enabled");
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		WH_free(paths.sslTrustedCertificateFileName);
		paths.sslTrustedCertificateFileName = nullptr;
		WH_free(paths.sslCertificateFileName);
		paths.sslCertificateFileName = nullptr;
		WH_free(paths.sslHostKeyFileName);
		paths.sslHostKeyFileName = nullptr;
		throw;
	}
}

void Identity::loadHostsDatabase() {
	try {
		if (!paths.hostsDatabaseName) {
			WH_LOG_WARNING("No hosts database");
		} else {
			//Load the database file from the disk in read-only mode
			hosts.open(paths.hostsDatabaseName, true);
			WH_LOG_DEBUG("Hosts loaded from %s", paths.hostsDatabaseName);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Identity::loadHostsFile() {
	try {
		if (!paths.hostsFileName) {
			WH_LOG_WARNING("No hosts file");
		} else {
			//Load the hosts into in-memory database
			hosts.open(":memory:");
			hosts.batchUpdate(paths.hostsFileName);
			WH_LOG_DEBUG("Hosts loaded from %s", paths.hostsFileName);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Identity::loadPrivateKey() {
	try {
		if (!paths.privateKeyFileName) {
			WH_LOG_WARNING("No private key file");
		} else if (auth.pki.loadHostKey(paths.privateKeyFileName)) {
			WH_LOG_DEBUG("Private key loaded from %s",
					paths.privateKeyFileName);
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
		if (!paths.publicKeyFileName) {
			WH_LOG_WARNING("No public key file");
		} else if (auth.pki.loadPublicKey(paths.publicKeyFileName)) {
			WH_LOG_DEBUG("Public key loaded from %s", paths.publicKeyFileName);
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
			throw Exception(EX_INVALIDOPERATION);
		} else if (!paths.sslCertificateFileName) {
			WH_LOG_WARNING("No SSL certificate file");
			return;
		} else if (paths.sslHostKeyFileName
				&& strcmp(paths.sslCertificateFileName,
						paths.sslHostKeyFileName) == 0) {
			ssl.ctx.initialize(paths.sslCertificateFileName,
					paths.sslHostKeyFileName);
			WH_LOG_DEBUG("SSL/TLS certificate and private key loaded from %s",
					paths.sslCertificateFileName);
		} else {
			ssl.ctx.initialize(paths.sslCertificateFileName, nullptr);
			WH_LOG_DEBUG("SSL/TLS certificate loaded from %s",
					paths.sslCertificateFileName);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Identity::loadSSLHostKey() {
	try {
		if (!ssl.enabled) {
			throw Exception(EX_INVALIDOPERATION);
		} else if (!paths.sslHostKeyFileName) {
			WH_LOG_WARNING("No SSL private key file");
			return;
		} else {
			ssl.ctx.initialize(nullptr, paths.sslHostKeyFileName);
			WH_LOG_DEBUG("SSL/TLS private key loaded from %s",
					paths.sslHostKeyFileName);
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
			return WH_strdup(buffer);
		}

		//STEP 2: Search in executable's directory
		System::executableDirectory(buffer, sizeof(buffer));
		strncat(buffer, "/" WH_CONF_FILE, sizeof(buffer) - strlen(buffer) - 1);
		if (Storage::testFile(buffer) == 1) {
			return WH_strdup(buffer);
		}

		//STEP 3: Search in home
		auto cfgPath = Storage::expandPathName(WH_CONF_PATH);
		if (Storage::testFile(cfgPath) == 1) {
			return cfgPath;
		} else {
			WH_free(cfgPath);
			cfgPath = nullptr;
		}

		//STEP 4: Search in /etc/wanhive
		if (Storage::testFile(WH_CONF_SYSTEM_PATH) == 1) {
			return WH_strdup(WH_CONF_SYSTEM_PATH);
		}

		//All attempts exhausted
		return nullptr;
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return nullptr;
	}
}

} /* namespace wanhive */

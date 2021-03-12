/*
 * Identity.h
 *
 * Identity management of the Wanhive hubs
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_IDENTITY_H_
#define WH_UTIL_IDENTITY_H_
#include "Host.h"
#include "InstanceID.h"
#include "PKI.h"
#include "../base/Configuration.h"
#include "../base/security/SSLContext.h"

namespace wanhive {
/**
 * Hub configuration, database and security keys
 * Thread safe at class level
 */
class Identity {
public:
	/*
	 * If the <path> is nullptr then the initialization routine will attempt
	 * to load the configuration file from a set of predefined paths.
	 */
	Identity(const char *path = nullptr) noexcept;
	~Identity();

	//Initialize the object
	void initialize();
	const Configuration& getConfiguration() const noexcept;
	const PKI* getPKI() const noexcept;
	bool verifyHost() const noexcept;
	SSLContext* getSSLContext() noexcept;
	bool allowSSL() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * For prevention of replay attacks during authentication
	 */
	//Generate cryptographically secure nonce
	bool generateNonce(Hash &hash, uint64_t salt, uint64_t id,
			Digest *nonce) const noexcept;
	//Verify a nonce during mutual authentication
	bool verifyNonce(Hash &hash, uint64_t salt, uint64_t id,
			const Digest *nonce) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Hosts management
	 */
	//Returns network address of the host <uid> into <ni>
	void getAddress(uint64_t uid, NameInfo &ni);
	/*
	 * Returns a randomized list of identifiers of the given <type> from the
	 * hosts database. At most <count> identifiers are read. Returns the actual
	 * number of identifiers transferred into <nodes>.
	 */
	unsigned int getIdentifiers(unsigned long long nodes[], unsigned int count,
			int type);
	/*
	 * Returns a 0-terminated list of identifiers read from a file defined
	 * in the configuration. At most (<count> - 1) identifiers are read.
	 * Returns the actual number of identifiers transferred into <nodes>
	 * (excluding the 0-terminator).
	 */
	unsigned int getIdentifiers(const char *section, const char *option,
			unsigned long long nodes[], unsigned int count) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Absolute paths
	 */
	const char* getConfigurationFile() const noexcept;
	const char* getHostsDatabase() const noexcept;
	const char* getHostsFile() const noexcept;
	const char* getPrivateKeyFile() const noexcept;
	const char* getPublicKeyFile() const noexcept;
	const char* getSSLTrustedCertificateFile() const noexcept;
	const char* getSSLCertificateFile() const noexcept;
	const char* getSSLHostKeyFile() const noexcept;
	//-----------------------------------------------------------------
	void generateInstanceId();
	void loadConfiguration();
	void loadHosts();
	void loadKeys();
	void loadSSL();

	void loadHostsDatabase();
	void loadHostsFile();

	void loadPrivateKey();
	void loadPublicKey();

	void loadSSLCertificate();
	void loadSSLHostKey();
private:
	char* locateConfigurationFile() noexcept;
public:
	static const char *CONF_FILE;
	static const char *CONF_PATH;
	static const char *CONF_SYSTEM_PATH;
private:
	//Unique ID of the currently running instance
	InstanceID *instanceId;
	//The application configuration
	Configuration cfg;
	//The hosts database
	Host host;

	//For authentication
	struct {
		PKI pki;
		bool enabled { false };
		bool verify { false };
	} auth;

	//For SSL/TLS
	struct {
		SSLContext ctx;
		bool enabled { false };
	} ssl;

	struct {
		//Path to the configuration file supplied from the command line
		char *path;
		//Absolute path to the configuration file
		char *configurationFileName;
		//Absolute path to the hosts database file
		char *hostsDatabaseName;
		//Absolute path to the clear text hosts file
		char *hostsFileName;
		//Absolute path to the private key file
		char *privateKeyFileName;
		//Absolute path to the public key
		char *publicKeyFileName;

		//SSL trusted certificate chain
		char *SSLTrustedCertificateFileName;
		//SSL certificate chain
		char *SSLCertificateFileName;
		//SSL private key
		char *SSLHostKeyFileName;
	} paths;
};

} /* namespace wanhive */

#endif /* WH_UTIL_IDENTITY_H_ */

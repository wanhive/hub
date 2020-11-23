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
class Identity: private Host {
public:
	/*
	 * If <path> is set to nullptr then the (re-)initialization routine will
	 * attempt to load the configuration file from a set of predefined paths(s).
	 */
	Identity(const char *path = nullptr) noexcept;
	~Identity();

	//(Re-)initialize the object
	void initialize();
	/*
	 * Returns a 0-terminated list of identifiers from the path pointed to by the
	 * configuration file entries. At max (length-1) entries are transferred.
	 * Returns the number of entries transferred, excluding the 0-terminator.
	 * NOTE: Can be safely called by an external thread if the external thread is
	 * spawned after this object's initialization and if the configuration is
	 * never modified/reloaded after initialization.
	 */
	unsigned int loadIdentifiers(const char *section, const char *option,
			unsigned long long nodes[], unsigned int length);
protected:
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
	//Returns host <uid>'s address in <ni>
	void getAddress(uint64_t uid, nameInfo &ni);
	//Fails if a file used instead of database
	void setAddress(uint64_t uid, nameInfo &ni);
	//Fails if a file used instead of database
	void removeAddress(uint64_t uid);
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

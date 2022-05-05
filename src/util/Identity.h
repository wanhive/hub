/*
 * Identity.h
 *
 * Identity and configuration manager
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_IDENTITY_H_
#define WH_UTIL_IDENTITY_H_
#include "Hosts.h"
#include "InstanceID.h"
#include "PKI.h"
#include "../base/Configuration.h"
#include "../base/security/SSLContext.h"

namespace wanhive {
/**
 * Hub configuration, database and keys manager
 * Thread safe at class level
 */
class Identity {
public:
	/**
	 * Constructor: call Identity::initialize() explicitly to perform complete
	 * initialization.
	 * @param path pathname of the configuration file. Can be nullptr in which
	 * case the initialization routine will attempt to load the configuration
	 * file from a set of predefined paths.
	 */
	Identity(const char *path = nullptr) noexcept;
	/**
	 * Destructor
	 */
	~Identity();

	/**
	 * Initializes the object after performing any necessary cleanup (if the
	 * object was initialized previously).
	 */
	void initialize();
	//-----------------------------------------------------------------
	/**
	 * Returns the configuration data.
	 * @return a configuration data object
	 */
	const Configuration& getConfiguration() const noexcept;
	/**
	 * Returns the asymmetric cryptography facility
	 * @return pointer to the PKI object that implements the asymmetric
	 * encryption algorithm
	 */
	const PKI* getPKI() const noexcept;
	/**
	 * Checks if the host verification is enabled.
	 * @return true is host verification is enabled, false otherwise
	 */
	bool verifyHost() const noexcept;
	/**
	 * Returns the context for SSL/TLS connections.
	 * @return pointer to the SSL/TLS context
	 */
	SSLContext* getSSLContext() noexcept;
	/**
	 * Checks if SSL/TLS is enabled.
	 * @return true if SSL/TLS is enabled, false otherwise
	 */
	bool allowSSL() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates cryptographically secure nonce to prevent replay attacks during
	 * authentication.
	 * @param hash an object providing the hash algorithm
	 * @param salt the salt
	 * @param id the identifier (salt, id) pair should be unique
	 * @param nonce the object for storing the result (nonce)
	 * @return true if a nonce was successfully generated, false otherwise
	 */
	bool generateNonce(Hash &hash, uint64_t salt, uint64_t id,
			Digest *nonce) const noexcept;
	/**
	 * Verifies a nonce during mutual authentication
	 * @param hash an object providing the hash algorithm
	 * @param salt the salt
	 * @param id the identifier
	 * @param nonce the nonce to verify
	 * @return true on successful verification, false otherwise
	 */
	bool verifyNonce(Hash &hash, uint64_t salt, uint64_t id,
			const Digest *nonce) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Hosts management: returns the network address associated with the given
	 * host identifier.
	 * @param uid the host identifier
	 * @param ni object for storing the result
	 */
	void getAddress(uint64_t uid, NameInfo &ni);

	/**
	 * Returns a randomized list of host identifiers of a given type from the
	 * hosts database.
	 * @param nodes array for storing the output (list of identifiers)
	 * @param count capacity of the output array
	 * @param type the host type
	 * @return the actual number of identifiers stored in the array
	 */
	unsigned int getIdentifiers(unsigned long long nodes[], unsigned int count,
			int type);
	/**
	 * Reads a list host identifiers from a file. Input file's pathname is
	 * resolved through the configuration data.
	 * @param section the section name of configuration data
	 * @param option the option name under the given section for resolving the
	 * input file's location.
	 * @param nodes array for storing the output (list of identifiers)
	 * @param count capacity of the output array
	 * @return the actual number of identifiers stored in the array
	 */
	unsigned int getIdentifiers(const char *section, const char *option,
			unsigned long long nodes[], unsigned int count) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the pathname of the configuration file.
	 * @return pathname of the configuration file
	 */
	const char* getConfigurationFile() const noexcept;
	/**
	 * Returns the pathname of the hosts database file.
	 * @return pathname of the hosts database, nullptr if not available
	 */
	const char* getHostsDatabase() const noexcept;
	/**
	 * Returns the pathname of the hosts file (a tab-delimited text file).
	 * @return pathname of the hosts file, nullptr if not available
	 */
	const char* getHostsFile() const noexcept;
	/**
	 * Returns the pathname of the private key file.
	 * @return pathname of the private key file, nullptr if not available
	 */
	const char* getPrivateKeyFile() const noexcept;
	/**
	 * Returns the pathname of the public key file.
	 * @return pathname of the public key file, nullptr if not available
	 */
	const char* getPublicKeyFile() const noexcept;
	/**
	 * Returns the pathname of the trusted certificate (root CA) file.
	 * @return pathname of the trusted certificate file, nullptr if not available
	 */
	const char* getSSLTrustedCertificateFile() const noexcept;
	/**
	 * Returns the pathname of the SSL certificate file.
	 * @return pathname of the SSL certificate file, nullptr if not available
	 */
	const char* getSSLCertificateFile() const noexcept;
	/**
	 * Returns the pathname of the private SSL key file.
	 * @return pathname of the private SSL key file, nullptr if not available
	 */
	const char* getSSLHostKeyFile() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates a new instance ID (deletes any existing one)
	 */
	void generateInstanceId();
	/**
	 * Reloads the configuration data
	 */
	void loadConfiguration();
	/**
	 * Reloads the hosts database (either from a sqlite3 database file or
	 * from a text file).
	 */
	void loadHosts();
	/**
	 * Reinitializes the asymmetric cryptography facility
	 */
	void loadKeys();
	/**
	 * Reconfigures SSL/TLS
	 */
	void loadSSL();
	/**
	 * Reloads the hosts database from a sqlite3 database file
	 */
	void loadHostsDatabase();
	/**
	 * Reloads the hosts database from a text file
	 */
	void loadHostsFile();
	/**
	 * Reloads the private key of the asymmetric cryptography facility
	 */
	void loadPrivateKey();
	/**
	 * Reloads the public key of the asymmetric cryptography facility
	 */
	void loadPublicKey();
	/**
	 * Reloads the SSL certificates
	 */
	void loadSSLCertificate();
	/**
	 * Reloads the private SSL key
	 */
	void loadSSLHostKey();
private:
	char* locateConfigurationFile() noexcept;
public:
	/** The default configuration file name */
	static const char *CONF_FILE;
	/** The default configuration file pathname (global) */
	static const char *CONF_PATH;
	/** The default configuration file pathname (system) */
	static const char *CONF_SYSTEM_PATH;
private:
	//Unique ID of the currently running instance
	InstanceID *instanceId;
	//The application configuration
	Configuration cfg;
	//The hosts database
	Hosts hosts;

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

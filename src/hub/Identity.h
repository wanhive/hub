/*
 * Identity.h
 *
 * Hub's identity
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_IDENTITY_H_
#define WH_HUB_IDENTITY_H_
#include "../util/Hosts.h"
#include "../util/InstanceID.h"
#include "../util/PKI.h"
#include "../base/Configuration.h"
#include "../base/security/SSLContext.h"

namespace wanhive {
/**
 * Hub's identity and properties
 */
class Identity {
public:
	/**
	 * Constructor: creates an empty object, call Identity::initialize() to load
	 * the configuration data and other properties.
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
	 * Initializes the object (overwrites the existing data).
	 */
	void initialize();
	//-----------------------------------------------------------------
	/**
	 * Returns the configuration data.
	 * @return a configuration data object
	 */
	const Configuration& getConfiguration() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the asymmetric cryptography facility.
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
	 * @return pointer to the SSL/TLS context, nullptr if SSL is not enabled
	 */
	SSLContext* getSSLContext() noexcept;
	/**
	 * Generates a secure nonce to prevent replay attack during handshaking and
	 * authentication.
	 * @param hash object providing the hash algorithm
	 * @param salt the salt value
	 * @param id the identifier (salt, id) pair should be unique
	 * @param nonce object for storing the output (nonce)
	 * @return true on success (nonce generated), false otherwise
	 */
	bool generateNonce(Hash &hash, uint64_t salt, uint64_t id,
			Digest *nonce) const noexcept;
	/**
	 * Verifies a secure nonce during mutual authentication.
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
	 * Hosts management: returns a randomized list of host identifiers of the
	 * given type from the hosts database.
	 * @param nodes array for storing the output (list of identifiers)
	 * @param count capacity of the output array
	 * @param type the host type
	 * @return the actual number of identifiers stored in the array
	 */
	unsigned int getIdentifiers(unsigned long long nodes[], unsigned int count,
			int type) noexcept;
	/**
	 * Hosts management: reads a list of host identifiers from a text file. Input
	 * file's pathname is resolved using the configuration data.
	 * @param section configuration data's section name
	 * @param option configuration data's option name for resolving the input
	 * file's location.
	 * @param nodes array for storing the output (list of identifiers)
	 * @param count capacity of the output array
	 * @return the actual number of identifiers stored in the array
	 */
	unsigned int getIdentifiers(const char *section, const char *option,
			unsigned long long nodes[], unsigned int count) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns configuration file's pathname.
	 * @return pathname of the configuration file
	 */
	const char* getConfigurationFile() const noexcept;
	/**
	 * Returns host database file's pathname.
	 * @return host database file's pathname, nullptr if not available
	 */
	const char* getHostsDatabase() const noexcept;
	/**
	 * Returns tab-delimited host file's pathname.
	 * @return tab-delimited host file's pathname, nullptr if not available
	 */
	const char* getHostsFile() const noexcept;
	/**
	 * Returns private key file's pathname.
	 * @return private key file's pathname, nullptr if not available
	 */
	const char* getPrivateKeyFile() const noexcept;
	/**
	 * Returns public key file's pathname.
	 * @return public key file's pathname, nullptr if not available
	 */
	const char* getPublicKeyFile() const noexcept;
	/**
	 * Returns trusted certificate (root CA) file's pathname.
	 * @return trusted certificate file's pathname, nullptr if not available
	 */
	const char* getSSLTrustedCertificateFile() const noexcept;
	/**
	 * Returns SSL certificate file's pathname.
	 * @return SSL certificate file's pathname, nullptr if not available
	 */
	const char* getSSLCertificateFile() const noexcept;
	/**
	 * Returns private SSL key file's pathname.
	 * @return private SSL key file's pathname, nullptr if not available
	 */
	const char* getSSLHostKeyFile() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates a new instance identifier.
	 */
	void generateInstanceId();
	/**
	 * Reloads the configuration data.
	 */
	void loadConfiguration();
	/**
	 * Reloads the hosts database (either from a database file or a text file).
	 */
	void loadHosts();
	/**
	 * Reloads the private and public keys (for asymmetric cryptography).
	 */
	void loadKeys();
	/**
	 * Reconfigures SSL/TLS.
	 */
	void loadSSL();
	/**
	 * Reloads the hosts database from a database file.
	 */
	void loadHostsDatabase();
	/**
	 * Reloads the hosts database from a text file.
	 */
	void loadHostsFile();
	/**
	 * Reloads the private key (for asymmetric cryptography).
	 */
	void loadPrivateKey();
	/**
	 * Reloads the public key (for asymmetric cryptography).
	 */
	void loadPublicKey();
	/**
	 * Reloads the SSL certificates.
	 */
	void loadSSLCertificate();
	/**
	 * Reloads the private SSL key.
	 */
	void loadSSLHostKey();
private:
	char* locateConfigurationFile() noexcept;
public:
	/** Configuration file's default name */
	static const char *CONF_FILE;
	/** Configuration file's default pathname (global) */
	static const char *CONF_PATH;
	/** Configuration file's default pathname (system) */
	static const char *CONF_SYSTEM_PATH;
private:
	//Unique ID of the currently running instance
	InstanceID *instanceId { nullptr };
	//Application's properties
	Configuration properties;
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
		//Configuration file's pathname from the command line
		char *config { nullptr };
		//Configuration file's resolved pathname
		char *configurationFileName { nullptr };
		//Hosts database file's pathname
		char *hostsDatabaseName { nullptr };
		//Clear text hosts file's pathname
		char *hostsFileName { nullptr };
		//Private key file's pathname
		char *privateKeyFileName { nullptr };
		//Public key file's pathname
		char *publicKeyFileName { nullptr };
		//SSL trusted certificate chain file's pathname
		char *sslTrustedCertificateFileName { nullptr };
		//SSL certificate chain file's pathname
		char *sslCertificateFileName { nullptr };
		//SSL private key file's pathname
		char *sslHostKeyFileName { nullptr };
	} paths;
};

} /* namespace wanhive */

#endif /* WH_HUB_IDENTITY_H_ */

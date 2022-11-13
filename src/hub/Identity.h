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
	 * Constructor: creates an uninitialized object, call Identity::initialize()
	 * to load configuration data and other settings.
	 * @param path configuration file's pathname (nullptr for default)
	 */
	Identity(const char *path = nullptr) noexcept;
	/**
	 * Destructor
	 */
	~Identity();
	/**
	 * Initializes the object (overwrites existing data).
	 */
	void initialize();
	//-----------------------------------------------------------------
	/**
	 * Returns configuration data.
	 * @return configuration data
	 */
	const Configuration& getConfiguration() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the asymmetric cryptography facility.
	 * @return asymmetric cryptography object
	 */
	const PKI* getPKI() const noexcept;
	/**
	 * Checks whether the host verification is enabled.
	 * @return true if host verification is enabled, false otherwise
	 */
	bool verifyHost() const noexcept;
	/**
	 * Returns secure connections' context.
	 * @return pointer to the SSL/TLS context, nullptr if SSL is disabled
	 */
	SSLContext* getSSLContext() noexcept;
	/**
	 * Generates a secure nonce to prevent replay attack during handshaking and
	 * authentication (see Identity::verifyNonce()).
	 * @param hash object providing the hash algorithm
	 * @param salt salt's value
	 * @param id identifier's value, [salt, id] pair should be unique
	 * @param nonce stores the generated nonce
	 * @return true on success, false otherwise
	 */
	bool generateNonce(Hash &hash, uint64_t salt, uint64_t id,
			Digest *nonce) const noexcept;
	/**
	 * Verifies a secure nonce (see Identity::generateNonce()).
	 * @param hash an object providing the hash algorithm
	 * @param salt salt's value
	 * @param id identifier's value
	 * @param nonce nonce's value for verification
	 * @return true on successful verification, false otherwise
	 */
	bool verifyNonce(Hash &hash, uint64_t salt, uint64_t id,
			const Digest *nonce) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Hosts management: returns network address associated with a host.
	 * @param uid host's identifier
	 * @param ni stores host's address
	 */
	void getAddress(uint64_t uid, NameInfo &ni);
	/**
	 * Hosts management: reads a randomized list of host identifiers of a
	 * given type from the hosts database and stores them in an array.
	 * @param nodes output array
	 * @param count output array's capacity
	 * @param type host's type
	 * @return actual number of identifiers stored in the output array
	 */
	unsigned int getIdentifiers(unsigned long long nodes[], unsigned int count,
			int type) noexcept;
	/**
	 * Hosts management: reads a list of host identifiers from a text file
	 * specified by the configuration data and stores them in an array.
	 * @param section configuration data's section name
	 * @param option configuration data's option name
	 * @param nodes output array
	 * @param count output array's capacity
	 * @return actual number of identifiers stored in the output array
	 */
	unsigned int getIdentifiers(const char *section, const char *option,
			unsigned long long nodes[], unsigned int count) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns configuration file's pathname.
	 * @return configuration file's pathname
	 */
	const char* getConfigurationFile() const noexcept;
	/**
	 * Returns host database file's pathname.
	 * @return host database file's pathname, nullptr if not available
	 */
	const char* getHostsDatabase() const noexcept;
	/**
	 * Returns tab-delimited host file's pathname.
	 * @return host file's pathname, nullptr if not available
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
	 * Reloads configuration data.
	 */
	void loadConfiguration();
	/**
	 * Reloads hosts database (either from a database file or a text file).
	 */
	void loadHosts();
	/**
	 * Reloads private and public keys (for asymmetric cryptography).
	 */
	void loadKeys();
	/**
	 * Reconfigures SSL/TLS.
	 */
	void loadSSL();
	/**
	 * Reloads hosts database from a database file.
	 */
	void loadHostsDatabase();
	/**
	 * Reloads hosts database from a text file.
	 */
	void loadHostsFile();
	/**
	 * Reloads private key (for asymmetric cryptography).
	 */
	void loadPrivateKey();
	/**
	 * Reloads public key (for asymmetric cryptography).
	 */
	void loadPublicKey();
	/**
	 * Reloads SSL certificates.
	 */
	void loadSSLCertificate();
	/**
	 * Reloads private SSL key.
	 */
	void loadSSLHostKey();
private:
	char* locateConfigurationFile() noexcept;
public:
	/** Configuration file's default name */
	static const char *CONF_FILE_NAME;
	/** Configuration file's default pathname (user) */
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

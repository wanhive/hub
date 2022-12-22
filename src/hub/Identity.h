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
	 * Returns an application data file's pathname.
	 * @param context application data's context
	 * @return file's pathname if exists, nullptr otherwise
	 */
	const char* dataPathName(int context) const noexcept;
	/**
	 * Partially reloads settings.
	 * @param context application data's context
	 */
	void reload(int context);
private:
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
	/** Configuration file's default name */
	static const char CONF_FILE_NAME[];
	/** Configuration file's default pathname (user) */
	static const char CONF_PATH[];
	/** Configuration file's default pathname (system) */
	static const char CONF_SYSTEM_PATH[];
	/**
	 * Application data contexts (non-negative integral values).
	 */
	enum {
		CTX_CONFIGURATION, /**< Configuration data */
		CTX_HOSTS_DB, /**< Hosts database */
		CTX_HOSTS_FILE, /**< Hosts file */
		CTX_PKI_PRIVATE, /**< Private key */
		CTX_PKI_PUBLIC, /**< Public key */
		CTX_SSL_ROOT, /**< Root CA certificate */
		CTX_SSL_CERTIFICATE, /**< SSL certificate */
		CTX_SSL_PRIVATE /**< SSL private key */
	};
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
		char *configurationFile { nullptr };
		//Hosts database file's pathname
		char *hostsDB { nullptr };
		//Clear text hosts file's pathname
		char *hostsFile { nullptr };
		//Private key file's pathname
		char *privateKey { nullptr };
		//Public key file's pathname
		char *publicKey { nullptr };
		//SSL trusted certificate chain file's pathname
		char *sslRoot { nullptr };
		//SSL certificate chain file's pathname
		char *sslCertificate { nullptr };
		//SSL private key file's pathname
		char *sslHostKey { nullptr };
	} paths;
};

} /* namespace wanhive */

#endif /* WH_HUB_IDENTITY_H_ */

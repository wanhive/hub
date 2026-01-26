/**
 * @file Identity.h
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
#include "../base/Options.h"
#include "../base/security/SSLContext.h"
#include "../util/Hosts.h"
#include "../util/InstanceID.h"
#include "../util/PKI.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Hub's identity and properties
 */
class Identity {
public:
	/**
	 * Constructor: sets configuration file's path.
	 * @param path configuration file's path
	 */
	Identity(const char *path = nullptr) noexcept;
	/**
	 * Destructor
	 */
	~Identity();
	/**
	 * Resets all settings and overrides existing configurations if necessary.
	 */
	void reset();
	//-----------------------------------------------------------------
	/**
	 * Returns the configuration options.
	 * @return configuration object
	 */
	const Options& getOptions() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the asymmetric cryptography facility.
	 * @return asymmetric cryptography object
	 */
	PKI* getPKI() noexcept;
	/**
	 * Checks whether the host verification is enabled.
	 * @return true if enabled, false if disabled
	 */
	bool verifyHost() const noexcept;
	/**
	 * Returns the SSL context.
	 * @return SSL context (nullptr if SSL is disabled)
	 */
	SSLContext* getSSLContext() noexcept;
	/**
	 * Generates cryptographic nonce.
	 * @param hash cryptographic hash function provider
	 * @param salt salt's value
	 * @param id identifier's value, [salt, id] pair should be unique
	 * @param nonce stores the generated nonce
	 * @return true on success, false otherwise
	 */
	bool generateNonce(Hash &hash, uint64_t salt, uint64_t id,
			Digest *nonce) const noexcept;
	/**
	 * Verifies cryptographic nonce.
	 * @param hash cryptographic hash function provider
	 * @param salt salt's value
	 * @param id identifier's value
	 * @param nonce nonce's value
	 * @return true on successful verification, false otherwise
	 */
	bool verifyNonce(Hash &hash, uint64_t salt, uint64_t id,
			const Digest *nonce) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Hosts management: returns a host's network address.
	 * @param uid host's identifier
	 * @param ni stores network address
	 */
	void getAddress(uint64_t uid, NameInfo &ni);
	/**
	 * Hosts management: reads a randomized list of host identifiers of a
	 * given type from the hosts database.
	 * @param nodes output array
	 * @param count output array's capacity
	 * @param type host's type
	 * @return identifiers count
	 */
	unsigned int getIdentifiers(unsigned long long nodes[], unsigned int count,
			int type) noexcept;
	/**
	 * Hosts management: reads a randomized list of host identifiers from a text
	 * file specified by configuration data.
	 * @param section section's name
	 * @param option option's name
	 * @param nodes output array
	 * @param count output array's capacity
	 * @return identifiers count
	 */
	unsigned int getIdentifiers(const char *section, const char *option,
			unsigned long long nodes[], unsigned int count) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns an application data file's path.
	 * @param context context identifier
	 * @return data file's path if exists, nullptr otherwise
	 */
	const char* getPath(int context) const noexcept;
	/**
	 * Partially reloads settings.
	 * @param context context identifier
	 */
	void refresh(int context);
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
	void loadSSLRoot();
	void loadSSLCertificate();
	void loadSSLKey();
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
		CTX_OPTIONS, /**< Configuration data */
		CTX_HOSTS_DB, /**< Hosts database */
		CTX_HOSTS_FILE, /**< Hosts file */
		CTX_PKI_PRIVATE, /**< Private key */
		CTX_PKI_PUBLIC, /**< Public key */
		CTX_SSL_ROOT, /**< Root CA certificate */
		CTX_SSL_CERT, /**< SSL certificate */
		CTX_SSL_PRIVATE /**< SSL private key */
	};
private:
	InstanceID *instanceId { };
	Options options;
	Hosts hosts;

	struct {
		PKI pki;
		bool enabled { };
		bool verify { };
	} auth;

	struct {
		SSLContext ctx;
		bool enabled { };
	} ssl;

	struct {
		char *config { };
		char *options { };
		char *hostsDB { };
		char *hostsFile { };
		char *privateKey { };
		char *publicKey { };
		char *sslRoot { };
		char *sslCertificate { };
		char *sslKey { };
	} paths;
};

} /* namespace wanhive */

#endif /* WH_HUB_IDENTITY_H_ */

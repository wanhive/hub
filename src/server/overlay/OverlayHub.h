/*
 * OverlayHub.h
 *
 * Overlay hub (server) implementation
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_OVERLAY_OVERLAYHUB_H_
#define WH_SERVER_OVERLAY_OVERLAYHUB_H_
#include "Topics.h"
#include "OverlayService.h"
#include "../../hub/Hub.h"

namespace wanhive {
/**
 * Wanhive's overlay hub implementation
 * An overlay network of hubs can be scaled out
 */
class OverlayHub: public Node, public Hub {
public:
	OverlayHub(unsigned long long uid, const char *path = nullptr);
	virtual ~OverlayHub();
private:
	//=================================================================
	/**
	 * Overridden base class methods
	 */
	void stop(Watcher *w) noexcept override final;
	void configure(void *arg) override final;
	void cleanup() noexcept override final;
	bool trapMessage(Message *message) noexcept override final;
	void route(Message *message) noexcept override final;
	void maintain() noexcept override final;
	void processInotification(unsigned long long uid,
			const InotifyEvent *event) noexcept override final;
	bool enableWorker() const noexcept override final;
	void doWork(void *arg) noexcept override final;
	void stopWork() noexcept override final;
	//=================================================================
	//Used by <configure>
	void installSettingsMonitor();
	void installService();

	//Used by <maintain>
	bool fixController() noexcept;
	bool fixRoutingTable() noexcept;
	bool connectToRoute(unsigned long long id, Digest *hc) noexcept;
	//=================================================================
	/*
	 * Create and register a local unix socket, return the other end in <sfd>
	 * If <blocking> is set to true then <sfd> is configured as a blocking socket
	 * with the given <timeout>. Setting <timeout> to zero (0) will make <sfd> to
	 * block forever. The returned Watcher is always nonblocking.
	 */
	Watcher* connect(int &sfd, bool blocking = false, int timeout = 0);
	//Establish connection with the remote hub <id> asynchronously
	Watcher* connect(unsigned long long id, Digest *hc);
	/*
	 * Helper function for <connect>
	 * Creates an outgoing Socket connection from the local node to a remote node
	 * specified by <id>. Sets CONN_PROXY type in the Socket connection object.
	 * Returns a unique session ID in <hc> which should be stored by the derived
	 * class to complete the registration process (see nonceToId).
	 */
	Watcher* createProxyConnection(unsigned long long id, Digest *nonce);
	/*
	 * <mode>: 0[TEMPORARY]; 1[INVALID]; 2[CLIENT];DEF[TEMPORARY|CLIENT]
	 * <target>: If non-zero then at most <target> connections will be purged.
	 */
	unsigned int purgeConnections(int mode = 0,
			unsigned int target = 0) noexcept;
	//=================================================================
	/*
	 * Processes a registration request
	 * Returns 0 on success and an ACK must be sent,
	 * Returns 1 on success but no ACK must be sent
	 * Returns -1 on error
	 * The registered watcher (Socket) is returned in w on success
	 */
	int processRegistrationRequest(Message *message) noexcept;
	/*
	 * Called when a new registration request is received.
	 * Return values:
	 * 0: Just activate the connection
	 * 1: Try to register with new ID (fail if exists)
	 * 2: Try to register with new ID (replace if exists)
	 * Any other value deactivates the connection
	 */
	int getModeOfRegistration(unsigned long long oldUid,
			unsigned long long newUid) noexcept;
	//Called by (processRegistrationRequest)
	void onRegistration(Watcher *w) noexcept;
	//Called by (handleStop)
	void onRecycle(Watcher *w) noexcept;
	//Get the ID of the connection associated with the worker task (hub's ID if no worker)
	unsigned long long getWorkerId() const noexcept;
	//Check whether the ID belongs to the worker task's connection
	bool isWorkerId(unsigned long long uid) const noexcept;
	//Returns true if this particular node is part of overlay network (excl. Controller)
	bool isSupernode() const noexcept;
	/*
	 * Reload the updated settings from the disk
	 * Limitations: detects only the modify->close sequence on a file, that makes it incompatible
	 * with several text editors which write to a temporary file and then overwrite/replace
	 * the original file via mv(1) or equivalent operation leading to the original file been ignored.
	 * Ref: https://github.com/guard/guard/wiki/Analysis-of-inotify-events-for-different-editors
	 * touch(1) and cp(1) commands are acceptable.
	 */
	void updateSettings(int index) noexcept;
	//Remove the active connections which no longer belong to this hub
	static int removeIfInvalid(Watcher *w, void *arg) noexcept;
	//Remove client connections
	static int removeIfClient(Watcher *w, void *arg) noexcept;
	//=================================================================
	//Used by route and request handlers
	int createRoute(Message *message) noexcept;
	unsigned long long getNextHop(unsigned long long destination) const noexcept;
	bool allowCommunication(unsigned long long source,
			unsigned long long destination) const noexcept;
	//Process a command (called by OverlayHub::route)
	int process(Message *message) noexcept;
	int handleInvalidRequest(Message *msg) noexcept;
	//=================================================================
	/*
	 * AUTHENTICATION and INFORMATION
	 */
	int handleDescribeNodeRequest(Message *msg) noexcept;
	//-----------------------------------------------------------------
	/*
	 * CONNECTION MANAGEMENT
	 */
	int handleRegistrationRequest(Message *msg) noexcept;
	int handleGetKeyRequest(Message *msg) noexcept;
	int handleFindRootRequest(Message *msg) noexcept;
	int handleBootstrapRequest(Message *msg) noexcept;
	//-----------------------------------------------------------------
	/*
	 * MULTICASTING (PUBLISH-SUBSCRIBE)
	 */
	int handlePublishRequest(Message *msg) noexcept;
	int handleSubscribeRequest(Message *msg) noexcept;
	int handleUnsubscribeRequest(Message *msg) noexcept;
	//-----------------------------------------------------------------
	/*
	 * ROUTE MANAGEMENT
	 */
	int handleGetPredecessorRequest(Message *msg) noexcept;
	int handleSetPredecessorRequest(Message *msg) noexcept;
	int handleGetSuccessorRequest(Message *msg) noexcept;
	int handleSetSuccessorRequest(Message *msg) noexcept;
	int handleGetFingerRequest(Message *msg) noexcept;
	int handleSetFingerRequest(Message *msg) noexcept;
	int handleGetNeighboursRequest(Message *msg) noexcept;
	int handleNotifyRequest(Message *msg) noexcept;
	//-----------------------------------------------------------------
	/*
	 * OVERLAY MANAGEMENT
	 */
	int handleFindSuccesssorRequest(Message *msg) noexcept;
	int handlePingNodeRequest(Message *msg) noexcept;
	int handleMapRequest(Message *msg) noexcept;
	//0: continue; 1: success/discontinue; -1: error/discontinue
	int mapFunction(Message *msg) noexcept;
	//-----------------------------------------------------------------
	//Check the netmask if the request originated from a client
	bool checkMask(unsigned long long source,
			unsigned long long destination) const noexcept;
	//Can the connection <uid> send privileged requests
	bool isPrivileged(unsigned long long uid) const noexcept;
	//Check the registration request
	bool isValidRegistrationRequest(const Message *msg) noexcept;
	//Check the registration request parameters
	bool allowRegistration(unsigned long long source,
			unsigned long long requestedId) const noexcept;
	//Check the stabilization response header
	bool isValidStabilizationResponse(const Message *msg) const noexcept;
	//Return the identifier associated with the given hash code
	unsigned long long nonceToId(const Digest *hc) const noexcept;
	/*
	 * Builds the response message's header if the response is to be sent
	 * directly. Message's source is set to this hub's identifier.
	 * If <length> is not zero then message length is updated to <length>.
	 */
	void buildResponseHeader(Message *msg, unsigned int length = 0) noexcept;
	//Temporarily add the identifier <id> into the local cache
	void cacheNode(unsigned long long id) noexcept;
	//Reset the internal state
	void clear() noexcept;
	//-----------------------------------------------------------------
	//Returns true if <uid> equals this hub's key
	bool isHostId(unsigned long long uid) const noexcept;
	//Returns true if <uid> belongs to the overlay nodes (incl. controller)
	static bool isInternalNode(unsigned long long uid) noexcept;
	//Returns true if <uid> belongs to controller
	static bool isController(unsigned long long uid) noexcept;
	//Returns true if <uid> doesn't belong to the overlay nodes
	static bool isExternalNode(unsigned long long uid) noexcept;
	//Map an arbitrary 64-bit key to the dht key-space
	static unsigned int mapKey(unsigned long long key) noexcept;
private:
	/**
	 * For worker thread management
	 */
	//The object which runs the stabilization protocol
	OverlayService stabilizer;
	struct {
		//To guard against invalid responses
		MessageHeader header;
		//Service watcher's identifier (defaults to hub's UID)
		unsigned long long id;
	} worker;
	//-----------------------------------------------------------------
	/**
	 * Overlay configuration
	 */
	struct {
		//Enable client Registration
		bool enableRegistration;
		//Authenticate every Client Registration Request
		bool authenticateClient;
		//Sign the getKey response
		bool mutualAuthentication;
		//If true, server will try to connect to the overlay network
		bool connectToOverlay;
		//Frequency of Routing Table Update
		unsigned int updateCycle;
		//Timeout for blocking I/O
		unsigned int requestTimeout;
		//Waiting period after stabilization error
		unsigned int retryInterval;
		//64-bit bitmask to restrict client<->client communication
		unsigned long long netMask;
		//Group ID of the hub
		unsigned int groupId;
		//Bootstrap nodes
		unsigned long long bootstrapNodes[128];
	} ctx;

	/**
	 * Cache of recently seen overlay nodes
	 */
	//Must be power of 2
	static constexpr unsigned int NODECACHE_SIZE = 32;
	struct {
		unsigned int index;
		unsigned long long cache[NODECACHE_SIZE];
	} nodes;
	//-----------------------------------------------------------------
	/**
	 * For authentication
	 */
	//For authentication of proxy connections, +1 for the controller
	Digest sKeys[TABLESIZE + 1];
	//For generation of message digests
	Hash hashFn;
	//-----------------------------------------------------------------
	/**
	 * For cleaning up of the connections
	 */
	struct {
		unsigned int target;
		unsigned int count;
	} counter;
	//-----------------------------------------------------------------
	/**
	 * Files being monitored
	 * [0]: Configuration file
	 * [1]: Hosts database
	 * [2]: Hosts file
	 * [3]: Private key file
	 * [4]: Public key file
	 * [5]: SSL trusted certificate
	 * [6]: SSL certificate
	 * [7]: SSL host key
	 */
	struct {
		int identifier;
		uint32_t events;
	} wd[8];
	//-----------------------------------------------------------------
	/**
	 * For multicasting: 256 topics are available in total ranging between 0-255
	 */
	Topics topics;
};

} /* namespace wanhive */

#endif /* WH_SERVER_OVERLAY_OVERLAYHUB_H_ */

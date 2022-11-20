/*
 * OverlayHub.h
 *
 * Overlay hub
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_OVERLAY_OVERLAYHUB_H_
#define WH_SERVER_OVERLAY_OVERLAYHUB_H_
#include "OverlayService.h"
#include "Topics.h"
#include "../../hub/Hub.h"

namespace wanhive {
/**
 * Overlay hub (messaging middleware)
 */
class OverlayHub final: public Hub, private Node {
public:
	/**
	 * Constructor: creates a new hub.
	 * @param uid hub's identifier
	 * @param path configuration file's pathname (nullptr for default)
	 */
	OverlayHub(unsigned long long uid, const char *path = nullptr);
	/**
	 * Destructor
	 */
	~OverlayHub();
private:
	//-----------------------------------------------------------------
	void stop(Watcher *w) noexcept override;
	void configure(void *arg) override;
	void cleanup() noexcept override;
	bool trapMessage(Message *message) noexcept override;
	void route(Message *message) noexcept override;
	void maintain() noexcept override;
	void processInotification(unsigned long long uid,
			const InotifyEvent *event) noexcept override;
	bool enableWorker() const noexcept override;
	void doWork(void *arg) noexcept override;
	void stopWork() noexcept override;
	//-----------------------------------------------------------------
	void installService();
	void installSettingsMonitor();
	void updateSettings(unsigned int index) noexcept;
	bool fixController() noexcept;
	bool fixRoutingTable() noexcept;
	bool connectToRoute(unsigned long long id, Digest *hc) noexcept;
	//-----------------------------------------------------------------
	//Called on successful registration
	void onRegistration(Watcher *w) noexcept;
	//Called before removal
	void onRecycle(Watcher *w) noexcept;
	//Temporarily memorize the identifier
	void addToCache(unsigned long long id) noexcept;
	//-----------------------------------------------------------------
	//Check the stabilization response header
	bool isValidStabilizationResponse(const Message *msg) const noexcept;
	//Check the registration request
	bool isValidRegistrationRequest(const Message *msg) noexcept;
	/*
	 * Processes a registration request.
	 * Returns 0 on success if an ACK must be sent,
	 * Returns 1 on success if no ACK must be sent,
	 * Returns -1 on error
	 */
	int processRegistrationRequest(Message *message) noexcept;
	//Check the registration request parameters
	bool allowRegistration(unsigned long long source,
			unsigned long long requestedId) const noexcept;
	/*
	 * Determines how a registration response should proceed. Return values:
	 * 0: Just activate the connection
	 * 1: Try to register with the new ID (fail on conflict)
	 * 2: Try to register with the new ID (replace on conflict)
	 * [Any other value]: deactivate the connection
	 */
	int getModeOfRegistration(unsigned long long current,
			unsigned long long requested) noexcept;
	//-----------------------------------------------------------------
	bool interceptMessage(Message *message) noexcept;
	void applyFlowControl(Message *message) noexcept;
	//Generates a route for the given message
	bool createRoute(Message *message) noexcept;
	unsigned long long getNextHop(unsigned long long destination) const noexcept;
	bool allowCommunication(unsigned long long source,
			unsigned long long destination) const noexcept;
	//Checks the netmask
	bool checkMask(unsigned long long source,
			unsigned long long destination) const noexcept;
	//-----------------------------------------------------------------
	//Processes a direct request
	bool process(Message *message) noexcept;

	bool processNullRequest(Message *message) noexcept;
	bool processBasicRequest(Message *message) noexcept;
	bool processMulticastRequest(Message *message) noexcept;
	bool processNodeRequest(Message *message) noexcept;
	bool processOverlayRequest(Message *message) noexcept;

	bool handleInvalidRequest(Message *msg) noexcept;
	bool handleDescribeNodeRequest(Message *msg) noexcept;

	bool handleRegistrationRequest(Message *msg) noexcept;
	bool handleGetKeyRequest(Message *msg) noexcept;
	bool handleFindRootRequest(Message *msg) noexcept;
	bool handleBootstrapRequest(Message *msg) noexcept;

	bool handlePublishRequest(Message *msg) noexcept;
	bool handleSubscribeRequest(Message *msg) noexcept;
	bool handleUnsubscribeRequest(Message *msg) noexcept;

	bool handleGetPredecessorRequest(Message *msg) noexcept;
	bool handleSetPredecessorRequest(Message *msg) noexcept;
	bool handleGetSuccessorRequest(Message *msg) noexcept;
	bool handleSetSuccessorRequest(Message *msg) noexcept;
	bool handleGetFingerRequest(Message *msg) noexcept;
	bool handleSetFingerRequest(Message *msg) noexcept;
	bool handleGetNeighboursRequest(Message *msg) noexcept;
	bool handleNotifyRequest(Message *msg) noexcept;

	bool handleFindSuccesssorRequest(Message *msg) noexcept;
	bool handlePingNodeRequest(Message *msg) noexcept;
	bool handleMapRequest(Message *msg) noexcept;
	//-----------------------------------------------------------------
	//0: continue; 1: success/discontinue; -1: error/discontinue
	int mapFunction(Message *msg) noexcept;
	/*
	 * Builds a direct response header. Message's source is set to this hub's
	 * identifier. If <length> isn't zero then the message length is updated.
	 */
	void buildDirectResponse(Message *msg, unsigned int length = 0) noexcept;
	//-----------------------------------------------------------------
	//Map an arbitrary 64-bit key to the dht key-space
	static unsigned int mapKey(unsigned long long key) noexcept;
	//Returns the identifier associated with the given hash code
	unsigned long long nonceToId(const Digest *hc) const noexcept;
	//Worker task's connection ID (hub's ID if no worker)
	unsigned long long getWorkerId() const noexcept;
	//Check whether the ID belongs to the worker task's connection
	bool isWorkerId(unsigned long long uid) const noexcept;
	//Can the connection <uid> send privileged requests
	bool isPrivileged(unsigned long long uid) const noexcept;
	//Returns true if this hub is part of overlay network (excl. Controller)
	bool isSupernode() const noexcept;
	//Returns true if <uid> equals this hub's key
	bool isHostId(unsigned long long uid) const noexcept;
	//Returns true if <uid> belongs to controller
	static bool isController(unsigned long long uid) noexcept;
	//Returns true if <uid> belongs to the overlay nodes (incl. controller)
	static bool isInternalNode(unsigned long long uid) noexcept;
	//Returns true if <uid> doesn't belong to the overlay nodes
	static bool isExternalNode(unsigned long long uid) noexcept;
	//Returns true if <uid> is an ephemeral (temporary) value
	static bool isEphemeralId(unsigned long long uid) noexcept;
	//-----------------------------------------------------------------
	/*
	 * Create and register a local unix socket, return the other end in <sfd>.
	 * If <blocking> is true then <sfd> is configured as a blocking socket with
	 * the given <timeout>. Setting <timeout> to zero (0) will make <sfd> to
	 * block forever. The returned Watcher is always nonblocking.
	 */
	Watcher* connect(int &sfd, bool blocking = false, int timeout = 0);
	//Establish connection with the remote hub <id> asynchronously
	Watcher* connect(unsigned long long id, Digest *hc);
	/*
	 * Creates an outgoing Socket connection to a remote node specified by <id>.
	 * Returns a unique session identifier in <hc>.
	 */
	Watcher* createProxyConnection(unsigned long long id, Digest *hc);
	/*
	 * Purges connections of particular type.
	 * <mode>: 0[TEMPORARY]; 1[INVALID]; DEFAULT[TEMPORARY|CLIENT]
	 * <target>: If not zero then at most these many connections will be purged.
	 */
	unsigned int purgeConnections(int mode, unsigned int target = 0) noexcept;
	//Remove active connections which no longer belong here
	static int removeIfInvalid(Watcher *w, void *arg) noexcept;
	//Remove client connections
	static int removeIfClient(Watcher *w, void *arg) noexcept;
	//-----------------------------------------------------------------
	//Resets the internal state
	void clear() noexcept;
	//Returns the runtime metrics
	void metrics(OverlayHubInfo &info) const noexcept;
private:
	/*
	 * Worker thread management
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
	/*
	 * Configuration data
	 */
	struct {
		//Enable client Registration
		bool enableRegistration;
		//Authenticate every Client Registration Request
		bool authenticateClient;
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
	//-----------------------------------------------------------------
	/*
	 * Cache of recently seen overlay nodes
	 */
	static constexpr unsigned int NODECACHE_SIZE = 32; //Must be power of 2
	struct {
		unsigned int index;
		unsigned long long cache[NODECACHE_SIZE];
	} nodes;
	//-----------------------------------------------------------------
	/*
	 * For authentication
	 */
	//For authentication of proxy connections, +1 for the controller
	Digest sessions[TABLESIZE + 1];
	//Message digests generator
	Hash hash;
	//-----------------------------------------------------------------
	/*
	 * Watch list of application data files
	 * [0]: Configuration file
	 * [1]: Hosts database
	 * [2]: Hosts file
	 * [3]: Private key file
	 * [4]: Public key file
	 * [5]: SSL trusted certificate
	 * [6]: SSL certificate
	 * [7]: SSL host key
	 */
	static constexpr unsigned int WATCHLIST_SIZE = 8;
	struct {
		int context;
		int identifier;
		uint32_t events;
	} watchlist[WATCHLIST_SIZE];
	//-----------------------------------------------------------------
	/*
	 * For multicasting: 256 topics in the range [0-255] are available
	 */
	Topics topics;
};

} /* namespace wanhive */

#endif /* WH_SERVER_OVERLAY_OVERLAYHUB_H_ */

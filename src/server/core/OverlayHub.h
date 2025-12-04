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

#ifndef WH_SERVER_CORE_OVERLAYHUB_H_
#define WH_SERVER_CORE_OVERLAYHUB_H_
#include "OverlayService.h"
#include "Topics.h"
#include "../../base/ds/Tokens.h"
#include "../../hub/Hub.h"

namespace wanhive {
/**
 * Overlay hub implementation
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
	void expel(Watcher *w) noexcept override;
	void configure(void *arg) override;
	void cleanup() noexcept override;
	void maintain() noexcept override;
	bool probe(Message *message) noexcept override;
	void route(Message *message) noexcept override;
	void onAlarm(unsigned long long uid, unsigned long long ticks) noexcept
			override;
	void onInotification(unsigned long long uid,
			const InotifyEvent *event) noexcept override;
	bool doable() const noexcept override;
	void act(void *arg) noexcept override;
	void cease() noexcept override;
	//-----------------------------------------------------------------
	void installService();
	void installTracker();
	void refresh(unsigned int context) noexcept;
	//-----------------------------------------------------------------
	//Routing table maintenance
	bool converge() noexcept;
	//Connect to a route
	bool bridge(unsigned long long id, Digest *hc) noexcept;
	//-----------------------------------------------------------------
	//Called after registration
	void onboard(Watcher *w) noexcept;
	//Called before removal
	void offboard(Watcher *w) noexcept;
	//Temporarily memorize an identifier
	void memorize(unsigned long long id) noexcept;
	//-----------------------------------------------------------------
	//Approve a registration request
	int enroll(const Message *request) noexcept;
	//Approve a registration request
	int enroll(unsigned long long source, unsigned long long request) noexcept;
	//Authenticate a registration request
	bool authenticate(const Message *request) noexcept;
	//Validate a registration request
	bool validate(unsigned long long source,
			unsigned long long request) const noexcept;
	//-----------------------------------------------------------------
	//Intercept registration and session key requests
	bool intercept(Message *message) noexcept;
	//Annotate a message before forwarding
	void annotate(Message *message) noexcept;
	//Plot a course for a message
	bool plot(Message *message) noexcept;
	//Corroborate a stabilization response
	bool corroborate(const Message *response) const noexcept;
	//Find the next hop towards a destination
	unsigned long long gateway(unsigned long long to) const noexcept;
	//Approve a communication
	bool approve(unsigned long long from, unsigned long long to) const noexcept;
	//Permit a communication
	bool permit(unsigned long long from, unsigned long long to) const noexcept;
	//-----------------------------------------------------------------
	//Process a direct request
	bool serve(Message *request) noexcept;
	bool serveNullRequest(Message *request) noexcept;
	bool serveBasicRequest(Message *request) noexcept;
	bool serveMulticastRequest(Message *request) noexcept;
	bool serveNodeRequest(Message *request) noexcept;
	bool serveOverlayRequest(Message *request) noexcept;

	bool handleInvalidRequest(Message *msg) noexcept;
	bool handleDescribeNodeRequest(Message *msg) noexcept;

	bool handleRegistrationRequest(Message *msg) noexcept;
	bool handleTokenRequest(Message *msg) noexcept;
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
	 * Build a direct response header. Set to this hub's identifier as message's
	 * source. If <length> isn't zero then update message's length.
	 */
	void buildDirectResponse(Message *msg, unsigned int length = 0) noexcept;
	//-----------------------------------------------------------------
	//Map an arbitrary 64-bit key to the dht key-space
	static unsigned int mapKey(unsigned long long key) noexcept;
	//Returns the identifier associated with the given hash code
	unsigned long long nonceToId(const Digest *hc) const noexcept;
	//Worker task's connection ID (hub's ID if no worker)
	unsigned long long getWorker() const noexcept;
	//Check whether the ID belongs to the worker task's connection
	bool isWorker(unsigned long long uid) const noexcept;
	//Can the connection <uid> send privileged requests
	bool isPrivileged(unsigned long long uid) const noexcept;
	//Returns true if this hub is part of overlay network (excl. Controller)
	bool isSuperNode() const noexcept;
	//Returns true if <uid> equals this hub's key
	bool isHost(unsigned long long uid) const noexcept;
	//Returns true if <uid> belongs to controller
	static bool isController(unsigned long long uid) noexcept;
	//Returns true if <uid> belongs to the overlay nodes (incl. controller)
	static bool isInternal(unsigned long long uid) noexcept;
	//Returns true if <uid> doesn't belong to the overlay nodes
	static bool isExternal(unsigned long long uid) noexcept;
	//Returns true if <uid> is an ephemeral (temporary) value
	static bool isEphemeral(unsigned long long uid) noexcept;
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
	unsigned int reap(int mode, unsigned int target = 0) noexcept;
	//Remove active connections which no longer belong here
	static int reapInvalid(Watcher *w, void *arg) noexcept;
	//Remove client connections
	static int reapClient(Watcher *w, void *arg) noexcept;
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
		bool enroll;
		//Authenticate every Client Registration Request
		bool authenticate;
		//If true, server will try to connect to the overlay network
		bool join;
		//Frequency of Routing Table Update
		unsigned int period;
		//Timeout for blocking I/O
		unsigned int timeout;
		//Waiting period after stabilization error
		unsigned int pause;
		//64-bit bitmask to restrict client<->client communication
		unsigned long long netmask;
		//Group ID of the hub
		unsigned int group;
		//Bootstrap nodes
		unsigned long long nodes[128];
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
	//-----------------------------------------------------------------
	/*
	 * TODO: This is an EXPERIMENTAL FEATURE.
	 * Registration request flood prevention.
	 */
	Tokens tokens;
};

} /* namespace wanhive */

#endif /* WH_SERVER_CORE_OVERLAYHUB_H_ */

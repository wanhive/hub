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
	 * Constructor: creates an overlay hub.
	 * @param uid hub's identifier
	 * @param path configuration file's path (nullptr for default)
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
	bool converge() noexcept;
	bool bridge(unsigned long long id, Digest *hc) noexcept;
	//-----------------------------------------------------------------
	void onboard(Watcher *w) noexcept;
	void offboard(Watcher *w) noexcept;
	void memorize(unsigned long long id) noexcept;
	//-----------------------------------------------------------------
	int enroll(const Message *request) noexcept;
	int enroll(unsigned long long source, unsigned long long request) noexcept;
	bool authenticate(const Message *request) noexcept;
	bool validate(unsigned long long source,
			unsigned long long request) const noexcept;
	//-----------------------------------------------------------------
	bool intercept(Message *message) noexcept;
	void annotate(Message *message) noexcept;
	bool plot(Message *message) noexcept;
	bool corroborate(const Message *response) const noexcept;
	unsigned long long gateway(unsigned long long to) const noexcept;
	bool approve(unsigned long long from, unsigned long long to) const noexcept;
	bool permit(unsigned long long from, unsigned long long to) const noexcept;
	//-----------------------------------------------------------------
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
	int mapFunction(Message *msg) noexcept;
	void buildDirectResponse(Message *msg, unsigned int length = 0) noexcept;
	//-----------------------------------------------------------------
	static unsigned int mapKey(unsigned long long key) noexcept;
	unsigned long long nonceToId(const Digest *hc) const noexcept;
	unsigned long long getWorker() const noexcept;
	bool isWorker(unsigned long long uid) const noexcept;
	bool isPrivileged(unsigned long long uid) const noexcept;
	bool isSuperNode() const noexcept;
	bool isHost(unsigned long long uid) const noexcept;
	static bool isController(unsigned long long uid) noexcept;
	static bool isInternal(unsigned long long uid) noexcept;
	static bool isExternal(unsigned long long uid) noexcept;
	static bool isEphemeral(unsigned long long uid) noexcept;
	//-----------------------------------------------------------------
	Watcher* connect(int &sfd, bool blocking = false, int timeout = 0);
	Watcher* connect(unsigned long long id, Digest *hc);
	Watcher* createProxyConnection(unsigned long long id, Digest *hc);
	unsigned int reap(int mode, unsigned int target = 0) noexcept;
	static int reapInvalid(Watcher *w, void *arg) noexcept;
	static int reapClient(Watcher *w, void *arg) noexcept;
	//-----------------------------------------------------------------
	void clear() noexcept;
	void metrics(OverlayHubInfo &info) const noexcept;
private:
	//-----------------------------------------------------------------
	OverlayService stabilizer;

	struct {
		MessageHeader header;
		unsigned long long id;
	} worker;
	//-----------------------------------------------------------------
	struct {
		bool enroll;
		bool authenticate;
		unsigned int refill;
		bool join;
		unsigned int period;
		unsigned int timeout;
		unsigned int pause;
		unsigned long long netmask;
		unsigned int group;
		unsigned long long nodes[128];
	} ctx;
	//-----------------------------------------------------------------
	static constexpr unsigned int NODECACHE_SIZE = 32;
	struct {
		unsigned int index;
		unsigned long long cache[NODECACHE_SIZE];
	} nodes;
	//-----------------------------------------------------------------
	Digest sessions[TABLESIZE + 1];
	Hash hash;
	//-----------------------------------------------------------------
	static constexpr unsigned int WATCHLIST_SIZE = 8;
	struct {
		int context;
		int identifier;
		uint32_t events;
	} watchlist[WATCHLIST_SIZE];
	//-----------------------------------------------------------------
	Topics topics;
	Tokens tokens;
};

} /* namespace wanhive */

#endif /* WH_SERVER_CORE_OVERLAYHUB_H_ */

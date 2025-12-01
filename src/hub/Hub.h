/*
 * Hub.h
 *
 * The base class for wanhive hubs
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_HUB_H_
#define WH_HUB_HUB_H_
#include "Alarm.h"
#include "Event.h"
#include "HubInfo.h"
#include "Identity.h"
#include "Inotifier.h"
#include "Interrupt.h"
#include "Job.h"
#include "Logic.h"
#include "Socket.h"
#include "Stream.h"
#include "Watchers.h"
#include "../base/Timer.h"
#include "../base/Thread.h"
#include "../base/ds/Buffer.h"
#include "../base/ds/CircularBuffer.h"
#include "../reactor/Handler.h"
#include "../reactor/Reactor.h"

namespace wanhive {
/**
 * Hub implementation
 */
class Hub: public Handler<Alarm>,
		public Handler<Event>,
		public Handler<Inotifier>,
		public Handler<Interrupt>,
		public Handler<Logic>,
		public Handler<Socket>,
		public Handler<Stream>,
		protected Identity,
		protected Reactor,
		private Job,
		private Task {
public:
	/**
	 * Constructor: creates a new hub.
	 * @param uid hub's identifier
	 * @param path configuration file's pathname, if not provided (pathname is
	 * nullptr) then configuration data is loaded from the default location(s).
	 */
	Hub(unsigned long long uid, const char *path = nullptr) noexcept;
	/**
	 * Destructor
	 */
	~Hub();
	//-----------------------------------------------------------------
	/**
	 * Returns hub's unique identifier.
	 * @return hub's identifier
	 */
	unsigned long long getUid() const noexcept;
	/**
	 * Executes this hub, returns only after termination of the event loop.
	 * @param arg additional argument
	 * @return true on normal termination, false on error
	 */
	bool execute(void *arg) noexcept;
	/**
	 * Cancels the event loop. The event loop may still block while waiting for
	 * an IO event or signal. This method is reentrant and signal-safe.
	 */
	void cancel() noexcept;
protected:
	/**
	 * Reads the default periodic timer's settings. This method can be safely
	 * called by the worker thread.
	 * @param expiration initial expiration in milliseconds
	 * @param interval interval in milliseconds
	 */
	void periodic(unsigned int &expiration, unsigned int &interval) noexcept;
	/**
	 * Reports events to the events counter. This method can be safely called
	 * by the worker thread.
	 * @param events events count
	 */
	void alert(unsigned long long events);
	/**
	 * Starts monitoring the file system events. This method can be safely
	 * called by the worker thread.
	 * @param path file or directory to monitor
	 * @param mask events of interest (inotify(7))
	 * @return unique watch descriptor
	 */
	int track(const char *path, uint32_t mask);
	/**
	 * Stops monitoring the file system events. This method can be safely
	 * called by the worker thread.
	 * @param identifier watch descriptor
	 */
	void untrack(int identifier) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads the runtime metrics.
	 * @param info stores the runtime metrics
	 */
	void metrics(HubInfo &info) const noexcept;
	/**
	 * Remove sensitive information from logs.
	 * @return true to remove, false otherwise
	 */
	bool redact() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Watcher management: checks whether a key is associated with a watcher.
	 * @param id key's value
	 * @return true if the key exists, false otherwise
	 */
	bool attached(unsigned long long id) const noexcept;
	/**
	 * Watcher management: returns the watcher associated with a key.
	 * @param id key's value
	 * @return associated watcher if the key exists, nullptr otherwise
	 */
	Watcher* find(unsigned long long id) const noexcept;
	/**
	 * Watcher management: registers a watcher, watcher's unique identifier is
	 * set as its key.
	 * @param w watcher to register
	 * @param events events of interest
	 * @param flags watcher's flags to set on success
	 */
	void attach(Watcher *w, uint32_t events, uint32_t flags);
	/**
	 * Watcher management: removes a key and its associated watcher.
	 * @param id key's value
	 * @return true if the operation could not complete immediately (scheduled
	 * for future), false otherwise.
	 */
	bool detach(unsigned long long id) noexcept;
	/**
	 * Watcher management: moves a watcher from its old key to a new key. If a
	 * watcher already exists at the new key and replacement is allowed, the
	 * conflicting watcher is disabled. On success, the moving watcher's
	 * WATCHER_ACTIVE flag is set and its UID is updated to match the new key.
	 * On failure, the old key's watcher is disabled.
	 * @param from old key
	 * @param to new key
	 * @param replace true to replace on conflict, false to fail on conflict
	 * @return the watcher at the new key on success, nullptr on failure
	 */
	Watcher* move(unsigned long long from, unsigned long long to,
			bool replace) noexcept;
	/**
	 * Watcher management: iterates through the registered watchers. Callback
	 * function's returned value controls the iteration's behavior:
	 * [0]: continue iteration,
	 * [1]: dissociate the watcher from it's key and continue iteration (call
	 * Reactor::disable() explicitly inside the callback function to remove the
	 * watcher from event loop),
	 * [Any other value]: stop iteration.
	 * @param fn callback function
	 * @param arg callback function's additional argument
	 */
	void iterate(int (*fn)(Watcher *w, void *arg), void *arg);
	/**
	 * Watcher management: purges expired temporary connections.
	 * @param target maximum connections to purge (0 for no limit)
	 * @param force true to expire all temporary connections; false for the
	 * normal operation.
	 * @return purged connections count
	 */
	unsigned int reap(unsigned int target = 0, bool force = false) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Message queuing: puts a message directly into the incoming queue.
	 * @param message a message to insert
	 * @return true on success, false on error
	 */
	bool collect(Message *message) noexcept;
	/**
	 * Message queuing: puts a message directly into the outgoing queue.
	 * @param message a message to insert
	 * @return true on success, false on error
	 */
	bool forward(Message *message) noexcept;
	//-----------------------------------------------------------------
	/*
	 * Reactor interface implementation
	 */
	void admit(Watcher *w) override;
	bool react(Watcher *w) noexcept override;
	void expel(Watcher *w) noexcept override;
	//-----------------------------------------------------------------
	/**
	 * Configuration: configures the hub before entering the event loop. Invoke
	 * it as a pre-operation in the derived class' version.
	 * @param arg additional argument
	 */
	virtual void configure(void *arg);
	/**
	 * Cleanup: cleans up the hub after exiting the event loop. Invoke it as a
	 * post-operation in the derived class' version.
	 */
	virtual void cleanup() noexcept;
private:
	/**
	 * Adapter: the hub maintenance routine.
	 */
	virtual void maintain() noexcept;
	/**
	 * Adapter: handles messages that have the MSG_PROBE flag enabled.
	 * @param message a message to probe
	 * @return true to discard (recycle) the message, false otherwise
	 */
	virtual bool probe(Message *message) noexcept;
	/**
	 * Adapter: processes an incoming message and creates a route for it.
	 * @param message incoming message
	 */
	virtual void route(Message *message) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Adapter: callback for periodic timer expiration.
	 * @param uid the source identifier
	 * @param ticks timer expiration count
	 */
	virtual void onAlarm(unsigned long long uid,
			unsigned long long ticks) noexcept;
	/**
	 * Adapter: callback for user-space events.
	 * @param uid the source identifier
	 * @param events the events count
	 */
	virtual void onEvent(unsigned long long uid,
			unsigned long long events) noexcept;
	/**
	 * Adapter: callback for file system events.
	 * @param uid the source identifier
	 * @param event the file system event
	 */
	virtual void onInotification(unsigned long long uid,
			const InotifyEvent *event) noexcept;
	/**
	 * Adapter: callback for software interrupt.
	 * @param uid the source identifier
	 * @param signum signal's number
	 */
	virtual void onInterrupt(unsigned long long uid, int signum) noexcept;
	/**
	 * Adapter: callback for digital logic.
	 * @param uid the source identifier
	 * @param event the edge transition
	 */
	virtual void onLogic(unsigned long long uid,
			const LogicEvent &event) noexcept;
	/**
	 * Adapter: callback for byte stream.
	 * @param id stream's identifier
	 * @param sink stream's sink
	 * @param source stream's source
	 */
	virtual void onStream(unsigned long long id, Sink<unsigned char> &sink,
			Source<unsigned char> &source) noexcept;
	//-----------------------------------------------------------------
	/*
	 * Handler interface implementations
	 */
	bool handle(Alarm *alarm) noexcept final;
	bool handle(Event *event) noexcept final;
	bool handle(Inotifier *inotifier) noexcept final;
	bool handle(Interrupt *interrupt) noexcept final;
	bool handle(Logic *logic) noexcept final;
	bool handle(Socket *socket) noexcept final;
	bool handle(Stream *stream) noexcept final;
	//-----------------------------------------------------------------
	/*
	 * Task interface implementation
	 */
	void run(void *arg) noexcept final;
	int getStatus() const noexcept final;
	void setStatus(int status) noexcept final;
	//-----------------------------------------------------------------
	/*
	 * Event loop management
	 */
	void setup(void *arg);
	void loop();
	//-----------------------------------------------------------------
	/*
	 * These functions are called during configuration
	 */
	void initBuffers();
	void initReactor();
	void initListener();
	void initAlarm();
	void initEvent();
	void initInotifier();
	void initInterrupt();
	//-----------------------------------------------------------------
	/*
	 * Job (asynchronous task) management
	 */
	void async(void *arg);
	void await();
	//-----------------------------------------------------------------
	/*
	 * Message processing
	 */
	void publish() noexcept;
	void process() noexcept;
	//-----------------------------------------------------------------
	/*
	 * Connection and stream management
	 */
	bool acceptConnection(Socket *listener) noexcept;
	bool processConnection(Socket *connection) noexcept;
	bool processStream(Stream *stream) noexcept;
	//-----------------------------------------------------------------
	/*
	 * Traffic limiting, shaping and policing
	 */
	bool drop(Message *message) const noexcept;
	unsigned int throttle(const Socket *connection) const noexcept;
	void countReceived(unsigned int bytes) noexcept;
	void countDropped(unsigned int bytes) noexcept;
	//-----------------------------------------------------------------
	/*
	 * Cleanup helpers
	 */
	void clear() noexcept;
	static int deleteWatchers(Watcher *w, void *arg) noexcept;
private:
	//Hub's unique identifier
	const unsigned long long uid;
	//Hub's termination status
	bool healthy;
	//Event loop's running status
	volatile int running;
	//-----------------------------------------------------------------
	//Watchers being monitored
	Watchers watchers;
	//Incoming messages ready for processing
	CircularBuffer<Message*> in;
	//Outgoing messages ready for dispatch
	CircularBuffer<Message*> out;
	//Temporary connections
	Buffer<unsigned long long> guests;
	//-----------------------------------------------------------------
	/*
	 * Hub statistics
	 */
	Timer uptime;
	struct {
		TrafficInfo received;
		TrafficInfo dropped;
	} traffic;
	//-----------------------------------------------------------------
	/*
	 * Special watchers, first among equals
	 */
	struct {
		Socket *listener;
		Alarm *alarm;
		Event *event;
		Inotifier *inotifier;
		Interrupt *interrupt;
	} prime;
	//-----------------------------------------------------------------
	/*
	 * Hub configuration
	 */
	struct {
		//Accept incoming connections
		bool listen;
		//Listening backlog
		int backlog;
		//Default binding address (for listening socket)
		char name[128];
		//Default binding address' type (unix/inet)
		char type[8];
		//Maximum number of IO events to process in each event loop
		unsigned int events;
		//Timer settings: initial expiration in milliseconds
		unsigned int expiration;
		//Timer settings: periodic expiration in milliseconds
		unsigned int interval;
		//Enable/disable semaphore-like behavior for the event notifier
		bool semaphore;
		//Enable/disable synchronous signal handling
		bool signal;
		//Maximum number of Connections we can create
		unsigned int connections;
		//Maximum number of Message Objects we can create
		unsigned int messages;
		//Maximum number of new connections the server can store
		unsigned int guests;
		//Time-out for temporary connections in milliseconds
		unsigned int lease;

		//Limit on incoming messages from each connection each cycle
		unsigned int inward;
		//Limit on outgoing messages a connection is allowed to hold on to
		unsigned int outward;
		//Throttle incoming packets under load
		bool throttle;
		//These number of messages will be reserved for internal purposes
		unsigned int reserved;
		//Server may drop messages to reduce congestion
		bool policing;
		//Cycles to live for a message
		unsigned int ttl;
		//Reservation ratios
		double answer;		//Reserved for answering
		double forward;	//Reserved for routing
		//Log verbosity
		unsigned int logging;
		//Remove/hide sensitive information
		bool redact;
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_HUB_HUB_H_ */

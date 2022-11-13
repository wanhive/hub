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
#include "Logic.h"
#include "Socket.h"
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
		protected Identity,
		protected Reactor,
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
	 * Returns the periodic timer settings with milliseconds resolution. This
	 * method can be safely called by the worker thread.
	 * @param expiration object for storing the initial expiration
	 * @param interval object for storing the interval of periodic timer
	 */
	void getAlarmSettings(unsigned int &expiration,
			unsigned int &interval) noexcept;
	/**
	 * Adds the given value to the events counter. This method can be safely
	 * called by the worker thread.
	 * @param events the value to add to the events counter
	 */
	void reportEvents(unsigned long long events);
	/**
	 * Starts monitoring of the given file or directory. This method can be
	 * safely called by the worker thread.
	 * @param path pathname of a file or directory
	 * @param mask events of interest
	 * @return a unique identifier (watch descriptor)
	 */
	int addToInotifier(const char *path, uint32_t mask);
	/**
	 * Stops monitoring of the given file or directory. This method can be
	 * safely called by the worker thread.
	 * @param identifier the watch descriptor (see Hub::addToInotifier())
	 */
	void removeFromInotifier(int identifier) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the runtime metrics.
	 * @param info object for storing the runtime metrics
	 */
	void metrics(HubInfo &info) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Watcher management: checks whether a watcher is associated with a
	 * given key.
	 * @param id key's value
	 * @return true if the key exists, false otherwise
	 */
	bool attached(unsigned long long id) const noexcept;
	/**
	 * Watcher management: returns watcher associated with a given key.
	 * @param id key's value
	 * @return associated watcher if the key exists, nullptr otherwise
	 */
	Watcher* fetch(unsigned long long id) const noexcept;
	/**
	 * Watcher management: registers a watcher, watcher's unique identifier is
	 * used as its key.
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
	 * Watcher management: shifts a watcher from it's old key to a new key. If
	 * a watcher is associated with the new key and replacement is allowed then
	 * the conflicting watcher is disabled. On success, the watcher being moved
	 * has it's WATCHER_ACTIVE flag set and it's UID is updated to match the new
	 * key. On failure, watcher associated with the old key is disabled.
	 * @param from old key's value
	 * @param to new key's value
	 * @param replace true to replace on conflict, false to fail on conflict
	 * @return watcher assigned to the new key on success, nullptr on failure
	 */
	Watcher* shift(unsigned long long from, unsigned long long to,
			bool replace = false) noexcept;
	/**
	 * Watcher management: iterates through the registered watchers. Callback
	 * function's returned value controls the iteration's behavior:
	 * [0]: continue iteration,
	 * [1]: dissociate the watcher from it's key and continue iteration (call
	 * Reactor::disable() explicitly inside the callback function to remove the
	 * watcher from event loop),
	 * [Any other value]: stop iteration.
	 * @param fn the callback function
	 * @param arg additional argument for the callback function
	 */
	void iterate(int (*fn)(Watcher *w, void *arg), void *arg);
	/**
	 * Watcher management: purge (remove) the timed-out temporary connections.
	 * @param target the maximum number of connections to purge (0 for no limit)
	 * @param force true to treat all the temporary connection as timed-out
	 * connections, false otherwise.
	 * @return number of purged connections
	 */
	unsigned int purgeTemporaryConnections(unsigned int target = 0, bool force =
			false) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Message queuing: inserts a message directly into the incoming queue.
	 * @param message the message to insert
	 * @return true on success, false otherwise
	 */
	bool retainMessage(Message *message) noexcept;
	/**
	 * Message queuing: inserts a message directly into the outgoing queue.
	 * @param message the message to insert
	 * @return true on success, false otherwise
	 */
	bool sendMessage(Message *message) noexcept;
	//-----------------------------------------------------------------
	/*
	 * Reactor interface implementation
	 */
	void adapt(Watcher *w) override;
	bool react(Watcher *w) noexcept override;
	void stop(Watcher *w) noexcept override;
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
	 * Adapter: processes a message which has it's MSG_TRAP flag set.
	 * @param message the message to process
	 * @return true to discard (recycle) the message, false otherwise
	 */
	virtual bool trapMessage(Message *message) noexcept;
	/**
	 * Adapter: processes an incoming message and creates a route for it.
	 * @param message the message to process
	 */
	virtual void route(Message *message) noexcept;
	/**
	 * Adapter: the hub maintenance routine.
	 */
	virtual void maintain() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Adapter: callback for periodic timer expiration.
	 * @param uid the source identifier
	 * @param ticks timer expiration count
	 */
	virtual void processAlarm(unsigned long long uid,
			unsigned long long ticks) noexcept;
	/**
	 * Adapter: callback for user-space events.
	 * @param uid the source identifier
	 * @param events the events count
	 */
	virtual void processEvent(unsigned long long uid,
			unsigned long long events) noexcept;
	/**
	 * Adapter: callback for file system events.
	 * @param uid the source identifier
	 * @param event the file system event
	 */
	virtual void processInotification(unsigned long long uid,
			const InotifyEvent *event) noexcept;
	/**
	 * Adapter: callback for software interrupt.
	 * @param uid the source identifier
	 * @param signum signal's number
	 */
	virtual void processInterrupt(unsigned long long uid, int signum) noexcept;
	/**
	 * Adapter: callback for digital logic.
	 * @param uid the source identifier
	 * @param event the edge transition
	 */
	virtual void processLogic(unsigned long long uid,
			const LogicEvent &event) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Adapter: allow worker thread creation.
	 * @return true to create a worker thread, false otherwise
	 */
	virtual bool enableWorker() const noexcept;
	/**
	 * Adapter: the worker thread's entry point.
	 * @param arg the additional argument for the worker thread
	 */
	virtual void doWork(void *arg) noexcept;
	/**
	 * Adapter: worker thread's cleanup routine (after exit).
	 */
	virtual void stopWork() noexcept;
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
	//-----------------------------------------------------------------
	/*
	 * Task interface implementation
	 */
	void run(void *arg) noexcept final;
	int getStatus() const noexcept final;
	void setStatus(int status) noexcept final;
	//-----------------------------------------------------------------
	//Configure the hub and start the worker thread
	void setup(void *arg);
	//The event loop
	void loop();
	//-----------------------------------------------------------------
	/*
	 * These functions are called during configuration (see Hub::configure()).
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
	 * Worker thread management
	 */
	//Starts the worker thread
	void startWorker(void *arg);
	//Stops the worker thread
	void stopWorker();
	//-----------------------------------------------------------------
	//Publish the outgoing messages to their intended destinations
	void publish() noexcept;
	/*
	 * Process all incoming messages, calls worker.run() if worker is installed,
	 * otherwise calls <route>.
	 */
	void processMessages() noexcept;
	//-----------------------------------------------------------------
	/*
	 * Connection management
	 */
	//Accept an incoming connection
	bool acceptConnection(Socket *listener) noexcept;
	//Read/write data to and from a connected Socket
	bool processConnection(Socket *connection) noexcept;
	//-----------------------------------------------------------------
	/*
	 * Traffic limiting, shaping and policing
	 */
	//Returns true if the <message> outlived it's TTL, false otherwise
	bool dropMessage(Message *message) const noexcept;
	//Sets admission limit (congestion control)
	unsigned int throttle(const Socket *connection) const noexcept;
	void countReceived(unsigned int bytes) noexcept;
	void countDropped(unsigned int bytes) noexcept;
	//=================================================================
	//Clean up the internal structures
	void clear() noexcept;
	//Iterate through internal record and delete all the watchers
	static int deleteWatchers(Watcher *w, void *arg) noexcept;
private:
	//Hub's unique identifier
	const unsigned long long uid;
	//Indicates whether the event loop terminated normally
	volatile bool healthy;
	//Event loop executes as long as this value is non-zero
	volatile int running;
	//-----------------------------------------------------------------
	//Collection of watchers currently being monitored
	Watchers watchers;
	//List of all incoming messages waiting for processing by the hub
	CircularBuffer<Message*> incomingMessages;
	//List of outgoing messages ready for dispatch to their respective destinations
	CircularBuffer<Message*> outgoingMessages;
	//List of incoming temporary connections
	Buffer<unsigned long long> temporaryConnections;
	//-----------------------------------------------------------------
	/*
	 * Hub statistics
	 */
	Timer uptime;	//Hub uptime
	struct { //Traffic metrics
		TrafficInfo received;
		TrafficInfo dropped;
	} traffic;
	//-----------------------------------------------------------------
	/*
	 * Special watchers, single instance of each type for each hub
	 */
	struct {
		Socket *listener; //The listener
		Alarm *alarm; //Clock watcher
		Event *event; //Events watcher
		Inotifier *inotifier;	//File system watcher
		Interrupt *interrupt; //Signal watcher
	} notifiers;
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
		char serviceName[128];
		//Default binding address' type (unix/inet)
		char serviceType[8];
		//Maximum number of IO events to process in each event loop
		unsigned int maxIOEvents;
		//Timer settings: initial expiration in miliseconds
		unsigned int timerExpiration;
		//Timer settings: periodic expiration in miliseconds
		unsigned int timerInterval;
		//Enable/disable semaphore-like behavior for the event notifier
		bool semaphore;
		//Enable/disable synchronous signal handling
		bool signal;
		//Maximum number of Connections we can create
		unsigned int connectionPoolSize;
		//Maximum number of Message Objects we can create
		unsigned int messagePoolSize;
		//Maximum number of new connections the server can store
		unsigned int maxNewConnnections;
		//Time-out for temporary connections in miliseconds
		unsigned int connectionTimeOut;

		//Limit on incoming messages from each connection each cycle
		unsigned int cycleInputLimit;
		//Limit on outgoing messages a connection is allowed to hold on to
		unsigned int outputQueueLimit;
		//Throttle incoming packets under load
		bool throttle;
		//These number of messages will be reserved for internal purposes
		unsigned int reservedMessages;
		//Server may drop messages to reduce congestion
		bool allowPacketDrop;
		//Cycles to live for a message
		unsigned int messageTTL;
		//Reservation ratios
		double answerRatio;		//Reserved for answering
		double forwardRatio;	//Reserved for routing
		//Log verbosity
		unsigned int verbosity;
	} ctx;
	//-----------------------------------------------------------------
	/*
	 * Worker thread
	 */
	class Worker final: public Task {
	public:
		Worker(Hub *hub) noexcept;
		~Worker();

		void run(void *arg) noexcept override;
		int getStatus() const noexcept override;
		void setStatus(int status) noexcept override;
	private:
		Hub *hub;
	};

	Worker worker;
	Thread *workerThread;
};

} /* namespace wanhive */

#endif /* WH_HUB_HUB_H_ */

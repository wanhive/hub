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
#include "Identity.h"
#include "Inotifier.h"
#include "Interrupt.h"
#include "Socket.h"
#include "../base/Timer.h"
#include "../base/Thread.h"
#include "../base/ds/Buffer.h"
#include "../base/ds/CircularBuffer.h"
#include "../reactor/Handler.h"
#include "../reactor/Reactor.h"
#include "../reactor/Watchers.h"

namespace wanhive {
/**
 * Hub implementation
 */
class Hub: public Handler<Alarm>,
		public Handler<Event>,
		public Handler<Inotifier>,
		public Handler<Interrupt>,
		public Handler<Socket>,
		protected Identity,
		protected Reactor,
		private Task {
public:
	/**
	 * Constructor: creates a new hub.
	 * @param uid the unique identifier of this hub
	 * @param path pathname of the configuration file, if nullptr then a search
	 * will be performed at predefined locations to find the configuration file.
	 */
	Hub(unsigned long long uid, const char *path = nullptr) noexcept;
	/**
	 * Destructor
	 */
	~Hub();
	//-----------------------------------------------------------------
	/**
	 * Returns this hub's identity.
	 * @return the unique identifier of this hub
	 */
	unsigned long long getUid() const noexcept;
	/**
	 * Executes this hub, this method returns after the termination of this hub's
	 * event loop.
	 * @param arg additional argument
	 * @return true on normal termination, false if the event loop was terminated
	 * due to error.
	 */
	bool execute(void *arg) noexcept;
	/**
	 * Terminates the event loop (see Hub::execute()). The event loop may still
	 * block until a signal delivery. This method is reentrant and signal-safe.
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
	 * Hub statistics: returns this hub's uptime.
	 * @return the uptime in seconds
	 */
	double getUptime() const noexcept;
	/**
	 * Hub statistics: returns the number of incoming messages.
	 * @return the received messages count
	 */
	unsigned long long messagesReceived() const noexcept;
	/**
	 * Hub statistics: returns the number of bytes received from the network.
	 * @return the total number of bytes received
	 */
	unsigned long long bytesReceived() const noexcept;
	/**
	 * Hub statistics: returns the number of incoming messages dropped during
	 * routing/forwarding due to network traffic congestion.
	 * @return the dropped messages count
	 */
	unsigned long long messagesDropped() const noexcept;
	/**
	 * Hub statistics: returns the number of bytes dropped during the message
	 * routing/forwarding due to network traffic congestion.
	 * @return the dropped bytes count
	 */
	unsigned long long bytesDropped() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Watcher management: checks whether a watcher is associated with the given
	 * key.
	 * @param id the key (identifier) to search for
	 * @return true if a watcher is associated with the given key, false otherwise
	 */
	bool containsWatcher(unsigned long long id) const noexcept;
	/**
	 * Watcher management: returns the watcher associated with the given key.
	 * @param id the key (identifier) to search for
	 * @return the watcher on successful match, nullptr if the key doesn't exist
	 */
	Watcher* getWatcher(unsigned long long id) const noexcept;
	/**
	 * Watcher management: adds a watcher to this hub's records, the watcher's
	 * unique identifier will be used as the key.
	 * @param w the watcher to register
	 * @param events the IO events of interest
	 * @param flags the flags to set in the watcher on successful registration
	 */
	void putWatcher(Watcher *w, uint32_t events, uint32_t flags);
	/**
	 * Watcher management: removes the key and the associated watcher from this
	 * hub's internal records.
	 * @param id the key (identifier) to remove
	 * @return true if the operation could not be completed immediately (the
	 * operation scheduled for silent completion), false otherwise.
	 */
	bool removeWatcher(unsigned long long id) noexcept;
	/**
	 * Watcher management: associates the watcher assigned to the given (old) key
	 * with a new key. If another watcher is already associated with the new key
	 * and replacement is allowed then the existing watcher will be removed. On
	 * success, the effected watcher has it's WATCHER_ACTIVE flag set and it's
	 * UID is updated to match the new key.
	 * @param id the old key
	 * @param newId the new (desired) key
	 * @param replace true to allow replacement on conflict, false to fail on
	 * conflict.
	 * @return the watcher assigned to the new key (originally associated with
	 * the old key) on success, nullptr on failure.
	 */
	Watcher* registerWatcher(unsigned long long id, unsigned long long newId,
			bool replace = false) noexcept;
	/**
	 * Watcher management: iterates through the list of registered watchers. The
	 * return value of callback function controls the behavior:
	 * [0]: continue iteration
	 * [1]: dissociate the watcher from it's key and continue iteration (call
	 * Reactor::disable() explicitly inside the callback function to remove the
	 * watcher from the event loop).
	 * [Any other value]: stop iteration
	 * @param fn the callback function
	 * @param arg additional argument for the callback function
	 */
	void iterateWatchers(int (*fn)(Watcher *w, void *arg), void *arg);
	/**
	 * Watcher management: Purge (remove) the timed-out temporary connections.
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
	 * Implementation of the Reactor interface
	 */
	void adapt(Watcher *w) override;
	bool react(Watcher *w) noexcept override;
	void stop(Watcher *w) noexcept override;
	//-----------------------------------------------------------------
	/**
	 * Initialization routine: configures the hub before entering the event loop.
	 * Always call the base class instance first in the derived classes.
	 * @param arg additional argument
	 */
	virtual void configure(void *arg);
	/**
	 * Cleanup routine: cleans up the hub after exiting the event loop. Always
	 * call the base class instance at last in the derived classes.
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
	 * Adapter: callback for interrupt.
	 * @param uid the source identifier
	 * @param info information about the delivered signal
	 */
	virtual void processInterrupt(unsigned long long uid,
			const SignalInfo *info) noexcept;
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
	 * Implementations of the Handler interfaces
	 */
	bool handle(Alarm *alarm) noexcept final;
	bool handle(Event *event) noexcept final;
	bool handle(Inotifier *inotifier) noexcept final;
	bool handle(Interrupt *interrupt) noexcept final;
	bool handle(Socket *socket) noexcept final;
	//-----------------------------------------------------------------
	/*
	 * Implementation of the Task interface
	 */
	void run(void *arg) noexcept final;
	int getStatus() const noexcept final;
	void setStatus(int status) noexcept final;
	//-----------------------------------------------------------------
	//Configure the hub and start the worker thread
	void setup(void *arg);
	//The event loop [monitor->publish->dispatch->processMessages->maintain]
	void loop();
	//-----------------------------------------------------------------
	/*
	 * These functions are called during configuration (see Hub::configure()).
	 */
	void initBuffers();
	void initReactor();
	void initListener();
	void initAlarm();
	void initEventNotifier();
	void initInotifier();
	void initSignalWatcher();
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
	struct {
		unsigned long long msgReceived;
		unsigned long long msgDropped;
		unsigned long long bytesReceived;
		unsigned long long bytesDropped;
	} stats;
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
		//If set then the hub will create a listening socket
		bool listen;
		//Listener Backlog
		int backlog;
		//Listener binds to this address if provided
		const char *serviceName;
		//Type of the service (unix/tcp)
		const char *serviceType;
		//Maximum number of IO events the selector must return
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
	 * The worker thread
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

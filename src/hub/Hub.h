/*
 * Hub.h
 *
 * The base class for the Wanhive hubs
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_HUB_H_
#define WH_HUB_HUB_H_
#include "Clock.h"
#include "EventNotifier.h"
#include "Inotifier.h"
#include "SignalWatcher.h"
#include "Socket.h"
#include "../base/Timer.h"
#include "../base/Thread.h"
#include "../base/ds/Buffer.h"
#include "../base/ds/CircularBuffer.h"
#include "../reactor/Handler.h"
#include "../reactor/Reactor.h"
#include "../reactor/Watchers.h"
#include "../util/Identity.h"

namespace wanhive {
/**
 * The base class for the Wanhive hubs
 * Thread safe at class level
 * NOTE: Few methods are conditionally safe for the worker thread
 */
class Hub: public Handler<Clock>,
		public Handler<EventNotifier>,
		public Handler<Inotifier>,
		public Handler<SignalWatcher>,
		public Handler<Socket>,
		protected Identity,
		protected Reactor,
		private Thread {
public:
	Hub(unsigned long long uid, const char *path = nullptr) noexcept;
	virtual ~Hub();

	//Unique identifier of this hub
	unsigned long long getUid() const noexcept;
	//=================================================================
	/*
	 * Execute the hub
	 * Returns true if the hub terminated normally, false otherwise
	 */
	bool execute(void *arg) noexcept;
	/*
	 * Cancel the event loop
	 * Event loop may still block on the selector if this function is called
	 * from outside the event loop. This function is reentrant.
	 */
	void cancel() noexcept;
protected:
	//=================================================================
	/**
	 * Alerts and events
	 * NOTE: Worker thread can safely call these methods
	 */
	//Retrieve the internal clock settings
	void getClockSettings(unsigned int &expiration,
			unsigned int &interval) noexcept;
	//Add <events> to the number of pending events
	void reportEvents(unsigned long long events);
	//Start monitoring given file/directory, returns unique identifier
	int addToInotifier(const char *path, uint32_t mask);
	//Stop monitoring a file/directory associated with <identifier>
	void removeFromInotifier(int identifier) noexcept;
	//Deliver a signal to this hub
	void interrupt(int signum = SIGUSR1) noexcept;
	//=================================================================
	/**
	 * Hub statistics
	 */
	//Hub uptime in seconds
	double getUptime() const noexcept;
	//Number of messages received
	unsigned long long messagesReceived() const noexcept;
	//Number of bytes received
	unsigned long long bytesReceived() const noexcept;
	//Number of messages dropped
	unsigned long long messagesDropped() const noexcept;
	//Number of bytes dropped
	unsigned long long bytesDropped() const noexcept;
	//=================================================================
	/**
	 * Watcher management
	 */
	//Return true if a Watcher with identifier <id> is registered on this hub
	bool containsWatcher(unsigned long long id) const noexcept;
	//Return the Watcher associated with the <id> (nullptr if doesn't exist)
	Watcher* getWatcher(unsigned long long id) const noexcept;
	/*
	 * Add Watcher <w> to hub's event loop, <flags> are set on success.
	 * The Watcher will be monitored for IO events described by <events>.
	 */
	void putWatcher(Watcher *w, uint32_t events, uint32_t flags);
	/*
	 * Removes the Watcher identified by <id> from this hub's event loop.
	 * If the watcher can not be removed immediately then the operation
	 * will be silently completed in the next iteration.
	 */
	void removeWatcher(unsigned long long id) noexcept;
	/*
	 * Register a Watcher currently registered as <id> with the <newId> and
	 * activate it. if <replace> is set to true and another Watcher is already
	 * registered with the identifier <newId> then it is replaced and disabled,
	 * otherwise the operation fails. Returns the newly registered Watcher on
	 * success, nullptr otherwise.
	 */
	Watcher* registerWatcher(unsigned long long id, unsigned long long newId,
			bool replace = false) noexcept;
	//Iterate over all the watchers monitored by this hub
	void iterateWatchers(int (*fn)(Watcher *w, void *arg), void *arg);
	/*
	 * Purge timed out temporary Socket connections. If <target> is not
	 * zero (0) then at most <target> number of Watchers will be removed.
	 */
	unsigned int purgeTemporaryConnections(unsigned int target = 0) noexcept;
	//=================================================================
	/**
	 * Message queuing
	 * Existing message flags are cleared/overridden.
	 */
	//Inserts a newly created  message directly into the incoming queue
	bool retainMessage(Message *message) noexcept;
	//Inserts a newly created message directly into the outgoing queue
	bool sendMessage(Message *message) noexcept;
	//=================================================================
	/**
	 * Implementation of the Reactor interface
	 */
	void adapt(Watcher *w) override;
	bool react(Watcher *w) noexcept override;
	void stop(Watcher *w) noexcept override;
	//=================================================================
	/**
	 * Base class initialization and cleanup routines
	 * Can be overridden in the derived classes
	 */
	//Load settings on startup: always call the base class instance first
	virtual void configure(void *arg);
	//Cleanup before shutdown: always call the base class instance at last
	virtual void cleanup() noexcept;
private:
	//=================================================================
	/**
	 * Base class adapters
	 * Override in the derived classes to build something useful
	 * Default implementations provided here do absolutely nothing
	 */

	/*
	 * Called by Hub::publish if the <message> has MSG_TRAP flag set.
	 * Return true to drop the message, false otherwise.
	 */
	virtual bool trapMessage(Message *message) noexcept;
	//Process the messages
	virtual void route(Message *message) noexcept;
	//Internal maintenance routine called at the end of each event loop
	virtual void maintain() noexcept;
	//Callback for the timer notification, <uid> is the source identifier
	virtual void processClockNotification(unsigned long long uid,
			unsigned long long ticks) noexcept;
	//Callback for the event notification, <uid> is the source identifier
	virtual void processEventNotification(unsigned long long uid,
			unsigned long long events) noexcept;
	//Callback for the file system notification, <uid> is the source identifier
	virtual void processInotification(unsigned long long uid,
			const InotifyEvent *event) noexcept;
	//Callback for the signal notification, <uid> is the source identifier
	virtual void processSignalNotification(unsigned long long uid,
			const SignalInfo *info) noexcept;

	//Return true to allow creation of the worker thread
	virtual bool enableWorker() const noexcept;
	//Worker thread's entry point
	virtual void doWork(void *arg) noexcept;
	//Worker thread's cleanup after it's exit (always called)
	virtual void stopWork() noexcept;
	//=================================================================
	/**
	 * Implementations of the Handler interface
	 * Used by Reactor's callback mechanism for handling IO operations on
	 * various watchers transparently.
	 */
	//Process the timer expiration notifications
	bool handle(Clock *clock) noexcept override final;
	//Process application level event notifications
	bool handle(EventNotifier *enotifier) noexcept override final;
	//Process the file system notifications
	bool handle(Inotifier *inotifier) noexcept override final;
	//Process the signal notifications
	bool handle(SignalWatcher *signalWatcher) noexcept override final;
	//Processes the Socket notifications
	bool handle(Socket *connection) noexcept override final;
	//=================================================================
	/**
	 * Override the Thread class
	 */
	void run(void *arg) noexcept override final;
	int getStatus() const noexcept override final;
	void setStatus(int status) noexcept override final;
	//=================================================================
	//Configure the hub and start the worker thread
	void setup(void *arg);
	//The event loop [monitor-> publish ->dispatch ->processMessages ->maintain]
	void loop();
	//-----------------------------------------------------------------
	/**
	 * These functions are called during configuration
	 */
	void initBuffers();
	void initReactor();
	void initListener();
	void initClock();
	void initEventNotifier();
	void initInotifier();
	void initSignalWatcher();
	//-----------------------------------------------------------------
	/**
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
	/**
	 * Following two methods are used for processing Socket events
	 */
	//Accept an incoming connection
	bool acceptConnection(Socket *listener) noexcept;
	//Read/write data to and from a connected Socket
	bool processConnection(Socket *connection) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Traffic limiting, shaping and policing sub-routines
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
		Clock *clock; //Clock watcher
		EventNotifier *enotifier; //Events watcher
		Inotifier *inotifier;	//File system watcher
		SignalWatcher *signalWatcher; //Signal watcher
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
	/**
	 * The worker thread
	 */
	class Worker: public Task {
	public:
		Worker(Hub *hub) noexcept;
		virtual ~Worker();

		void run(void *arg) noexcept override final;
		int getStatus() const noexcept override final;
		void setStatus(int status) noexcept override final;
	private:
		Hub *hub;
	};

	Worker worker;
	Thread *workerThread;
};

} /* namespace wanhive */

#endif /* WH_HUB_HUB_H_ */

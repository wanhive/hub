/*
 * Hub.cpp
 *
 * The base class for the Wanhive hubs
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Hub.h"
#include "../base/Logger.h"
#include "../base/Signal.h"
#include <unistd.h>

namespace wanhive {

Hub::Worker::Worker(Hub *hub) noexcept :
		hub(hub) {
}

Hub::Worker::~Worker() {

}

void Hub::Worker::run(void *arg) noexcept {
	hub->doWork(arg);
}

int Hub::Worker::getStatus() const noexcept {
	return 0;
}

void Hub::Worker::setStatus(int status) noexcept {

}

}  // namespace wanhive

namespace wanhive {

Hub::Hub(unsigned long long uid, const char *path) noexcept :
		Identity(path), uid(uid), healthy(true), worker(this) {
	clear();
}

Hub::~Hub() {

}

unsigned long long Hub::getUid() const noexcept {
	return uid;
}

bool Hub::execute(void *arg) noexcept {
	setStatus(1);
	run(arg);
	return healthy;
}

void Hub::cancel() noexcept {
	setStatus(0);
}

void Hub::getClockSettings(unsigned int &expiration,
		unsigned int &interval) noexcept {
	if (notifiers.clock) {
		expiration = notifiers.clock->getExpiration();
		interval = notifiers.clock->getInterval();
	} else {
		expiration = 0;
		interval = 0;
	}
}

void Hub::reportEvents(unsigned long long events) {
	if (notifiers.enotifier) {
		notifiers.enotifier->write(events);
	} else {
		throw Exception(EX_RESOURCE);
	}
}

int Hub::addToInotifier(const char *path, uint32_t mask) {
	if (notifiers.inotifier) {
		return notifiers.inotifier->add(path, mask);
	} else {
		throw Exception(EX_RESOURCE);
	}
}

void Hub::removeFromInotifier(int identifier) noexcept {
	try {
		if (notifiers.inotifier) {
			notifiers.inotifier->remove(identifier);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
	}
}

void Hub::interrupt(int signum) noexcept {
	Thread::interrupt(signum);
}

double Hub::getUptime() const noexcept {
	return uptime.elapsed();
}

unsigned long long Hub::messagesReceived() const noexcept {
	return stats.msgReceived;
}

unsigned long long Hub::bytesReceived() const noexcept {
	return stats.bytesReceived;
}

unsigned long long Hub::messagesDropped() const noexcept {
	return stats.msgDropped;
}

unsigned long long Hub::bytesDropped() const noexcept {
	return stats.bytesDropped;
}

bool Hub::containsWatcher(unsigned long long id) const noexcept {
	return watchers.contains(id);
}

Watcher* Hub::getWatcher(unsigned long long id) const noexcept {
	return watchers.get(id);
}

void Hub::putWatcher(Watcher *w, uint32_t events, uint32_t flags) {
	if (w && !watchers.contains(w->getUid())) {
		add(w, events);
		watchers.put(w);
		w->setFlags(flags);
	} else {
		throw Exception(EX_INVALIDOPERATION);
	}
}

bool Hub::removeWatcher(unsigned long long id) noexcept {
	return disable(getWatcher(id));
}

Watcher* Hub::registerWatcher(unsigned long long id, unsigned long long newId,
		bool replace) noexcept {
	Watcher *w[2] = { nullptr, nullptr };
	if (!watchers.contains(id)) {
		return nullptr;
	} else if (watchers.move(id, newId, w, replace)) {
		if (w[0] && (w[0] != w[1])) {
			//Disable the old watcher which originally owned newId
			disable(w[0]);
		}
		w[1]->setFlags(WATCHER_ACTIVE);
		return w[1];
	} else {
		disable(w[0]);
		return nullptr;
	}
}

void Hub::iterateWatchers(int (*fn)(Watcher *w, void *arg), void *arg) {
	watchers.iterate(fn, arg);
}

unsigned int Hub::purgeTemporaryConnections(unsigned int target,
		bool force) noexcept {
	//Prepare the buffer for reading
	temporaryConnections.rewind();
	auto timeout = force ? 0 : ctx.connectionTimeOut;

	unsigned int count = 0;
	unsigned long long id;
	while (temporaryConnections.get(id)) {
		//This conversion is always safe
		auto conn = static_cast<Socket*>(getWatcher(id));
		if (!conn) {
			continue;
		} else if (conn->hasTimedOut(timeout)) {
			disable(conn);
			++count;
			if (target && count >= target) {
				break;
			}
		} else {
			/*
			 * Move back and break-out. Connections are added in chronological
			 * order, hence if this connection hasn't timed-out then neither
			 * have the successors.
			 */
			temporaryConnections.setIndex(temporaryConnections.getIndex() - 1);
			break;
		}
	}
	//Prepare the buffer for adding more data towards rear
	temporaryConnections.pack();
	return count;
}

bool Hub::retainMessage(Message *message) noexcept {
	if (message && !message->isMarked() && message->validate()
			&& incomingMessages.put(message)) {
		message->putFlags(MSG_WAIT_PROCESSING);
		message->setMarked();
		return true;
	} else {
		return false;
	}
}

bool Hub::sendMessage(Message *message) noexcept {
	if (message && !message->isMarked() && outgoingMessages.put(message)) {
		message->putFlags(MSG_PROCESSED);
		message->setMarked();
		return true;
	} else {
		return false;
	}
}

void Hub::adapt(Watcher *w) {
	w->start();
	/*
	 * Always cast to the base type before converting to (void *). That will
	 * allow safe conversion of void * back to the base or the derived type
	 * For example, in a derived class the following line should look like:
	 * w->setReference(static_cast<Hub*>(this));
	 */
	w->setReference(this);
}

bool Hub::react(Watcher *w) noexcept {
	return w->callback(nullptr);
}

void Hub::stop(Watcher *w) noexcept {
	/*
	 * Bail out if any one of these ever fails. This provision allows the
	 * worker thread to safely interact with these watchers.
	 */
	auto error = (w == notifiers.listener) || (w == notifiers.clock)
			|| (w == notifiers.enotifier) || (w == notifiers.inotifier)
			|| (w == notifiers.signalWatcher);

	if (error) {
		WH_LOG_ERROR("Fatal component failure, exiting.");
		exit(EXIT_FAILURE);
	} else {
		auto id = w->getUid();
		watchers.remove(id);
		w->stop();
		delete w;
		WH_LOG_DEBUG("Watcher %llu recycled", id);
	}
}

void Hub::configure(void *arg) {
	try {
		uptime.now();
		Identity::initialize();
		decltype(auto) conf = Identity::getConfiguration();

		ctx.listen = conf.getBoolean("HUB", "listen");
		ctx.backlog = conf.getNumber("HUB", "backlog");
		ctx.serviceName = conf.getString("HUB", "serviceName");
		ctx.serviceType = conf.getString("HUB", "serviceType");
		ctx.maxIOEvents = conf.getNumber("HUB", "maxIOEvents");

		ctx.timerExpiration = conf.getNumber("HUB", "timerExpiration");
		ctx.timerInterval = conf.getNumber("HUB", "timerInterval");
		ctx.semaphore = conf.getBoolean("HUB", "semaphore");
		ctx.signal = conf.getBoolean("HUB", "signal");

		ctx.connectionPoolSize = conf.getNumber("HUB", "connectionPoolSize");
		ctx.messagePoolSize = conf.getNumber("HUB", "messagePoolSize");
		//Take care of the "special" cases: [1, 2, power-of-two]
		if (ctx.messagePoolSize == 1 || ctx.messagePoolSize == 2) {
			ctx.messagePoolSize = 3;
		} else if (Twiddler::isPower2(ctx.messagePoolSize)) { //false for 0
			ctx.messagePoolSize -= 1;
		}

		ctx.maxNewConnnections = conf.getNumber("HUB", "maxNewConnnections");
		//Take care of the special case: Hub not listening
		if (ctx.listen) {
			ctx.maxNewConnnections = Twiddler::min(ctx.maxNewConnnections,
					ctx.connectionPoolSize);
		} else {
			ctx.maxNewConnnections = 0;
		}
		ctx.connectionTimeOut = conf.getNumber("HUB", "connectionTimeOut",
				2000);

		ctx.cycleInputLimit = conf.getNumber("HUB", "cycleInputLimit");
		ctx.outputQueueLimit = conf.getNumber("HUB", "outputQueueLimit");
		ctx.outputQueueLimit = Twiddler::min(ctx.outputQueueLimit,
				(Socket::OUT_QUEUE_SIZE - 1));

		ctx.throttle = conf.getBoolean("HUB", "throttle");
		ctx.reservedMessages = conf.getNumber("HUB", "reservedMessages");
		ctx.reservedMessages = Twiddler::min(ctx.reservedMessages,
				ctx.messagePoolSize);

		ctx.allowPacketDrop = conf.getBoolean("HUB", "allowPacketDrop");
		ctx.messageTTL = conf.getNumber("HUB", "messageTTL");

		ctx.answerRatio = conf.getDouble("HUB", "answerRatio", 0.5);
		ctx.forwardRatio = conf.getDouble("HUB", "forwardRatio", 0);

		ctx.verbosity = conf.getNumber("HUB", "verbosity", WH_LOGLEVEL_DEBUG);
		Logger::getDefault().setLevel(ctx.verbosity);
		ctx.verbosity = Logger::getDefault().getLevel();
		//-----------------------------------------------------------------
		WH_LOG_DEBUG(
				"Hub setings:\n" "LISTEN=%s, BACKLOG=%d, SERVICENAME=%s, SERVICETYPE=%s,\n" "MAX_IO_EVENTS=%u, TIMER_EXPIRATION=%ums, TIMER_INTERVAL=%ums, SEMAPHORE=%s,\n" "SYNCHRONOUS_SIGNAL=%s, CONNECTION_POOL_SIZE=%u, MESSAGE_POOL_SIZE=%u,\n" "MAX_NEW_CONNECTIONS=%u, TMP_CONNECTION_TIMEOUT=%ums, CYCLEINLIMIT=%u,\n" "OUTQUEUELIMIT=%u THROTTLE=%s, RESERVED_MESSAGES=%u, ALLOW_PACKET_DROP=%s,\n" "MESSAGE_TTL=%u, ANSWER_RATIO=%f, FORWARD_RATIO=%f, LOG_LEVEL=%s\n",
				WH_BOOLF(ctx.listen), ctx.backlog, ctx.serviceName,
				ctx.serviceType, ctx.maxIOEvents, ctx.timerExpiration,
				ctx.timerInterval, WH_BOOLF(ctx.semaphore),
				WH_BOOLF(ctx.signal), ctx.connectionPoolSize,
				ctx.messagePoolSize, ctx.maxNewConnnections,
				ctx.connectionTimeOut, ctx.cycleInputLimit,
				ctx.outputQueueLimit, WH_BOOLF(ctx.throttle),
				ctx.reservedMessages, WH_BOOLF(ctx.allowPacketDrop),
				ctx.messageTTL, ctx.answerRatio, ctx.forwardRatio,
				Logger::levelString(Logger::getDefault().getLevel()));
		//-----------------------------------------------------------------
		/*
		 * Initialization of the core data structures
		 */
		initBuffers();
		initReactor();
		initListener();
		initClock();
		initEventNotifier();
		initInotifier();
		initSignalWatcher();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Hub::cleanup() noexcept {
	/*
	 * Clean up and reset this object. Any exceptional condition should
	 * abort the process to prevent memory leak. Maintain the sequence.
	 */
	try {
		WH_LOG_INFO("Shutdown initiated....");
		//-----------------------------------------------------------------
		//1. Stop the worker thread
		stopWorker();
		//-----------------------------------------------------------------
		//2. Disconnect: recycle all watchers
		iterateWatchers(deleteWatchers, nullptr);
		//-----------------------------------------------------------------
		//3. Clean up all the containers
		temporaryConnections.clear();
		Message *msg;
		while (outgoingMessages.get(msg)) {
			Message::recycle(msg);
		}
		while (incomingMessages.get(msg)) {
			Message::recycle(msg);
		}
		//-----------------------------------------------------------------
		//4. Destroy all the memory pools
		Socket::destroyPool();
		Message::destroyPool();
		//-----------------------------------------------------------------
		//5. Clear the internal structures
		clear();
		//-----------------------------------------------------------------
		//6. Print goodbye message
		WH_LOG_INFO("Shutdown completed.\n\n");
	} catch (const BaseException &e) {
		//Memory leak, do not try to recover
		WH_LOG_EXCEPTION(e);
		WH_LOG_ERROR("Resource leaked, aborting.");
		abort();
	} catch (...) {
		//Memory leak, do not try to recover
		WH_LOG_EXCEPTION_U();
		WH_LOG_ERROR("Resource leaked, aborting.");
		abort();
	}

	/*
	 * If we are here then we can reuse this object.
	 */
}

bool Hub::trapMessage(Message *message) noexcept {
	return false;
}

void Hub::route(Message *message) noexcept {

}

void Hub::maintain() noexcept {

}

void Hub::processClockNotification(unsigned long long uid,
		unsigned long long ticks) noexcept {

}

void Hub::processEventNotification(unsigned long long uid,
		unsigned long long events) noexcept {

}

void Hub::processInotification(unsigned long long uid,
		const InotifyEvent *event) noexcept {

}

void Hub::processSignalNotification(unsigned long long uid,
		const SignalInfo *info) noexcept {

}

bool Hub::enableWorker() const noexcept {
	return false;
}

void Hub::doWork(void *arg) noexcept {

}

void Hub::stopWork() noexcept {

}

bool Hub::handle(Clock *clock) noexcept {
	try {
		if (clock == nullptr) {
			return false;
		} else if (clock->testEvents(IO_CLOSE)) {
			return disable(clock);
		} else if (clock->testEvents(IO_READ) && clock->read() == -1) {
			return disable(clock);
		}
		//-----------------------------------------------------------------
		if (clock->getCount()) {
			auto uid = (clock == notifiers.clock ? 0 : clock->getUid());
			processClockNotification(uid, clock->getCount());
		}
		return clock->isReady();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return disable(clock);
	}
}

bool Hub::handle(EventNotifier *enotifier) noexcept {
	try {
		if (enotifier == nullptr) {
			return false;
		} else if (enotifier->testEvents(IO_CLOSE)) {
			return disable(enotifier);
		} else if (enotifier->testEvents(IO_READ) && enotifier->read() == -1) {
			return disable(enotifier);
		}
		//-----------------------------------------------------------------
		if (enotifier->getCount()) {
			auto uid = (
					enotifier == notifiers.enotifier ? 0 : enotifier->getUid());
			processEventNotification(uid, enotifier->getCount());
		}
		return enotifier->isReady();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return disable(enotifier);
	}
}

bool Hub::handle(Inotifier *inotifier) noexcept {
	try {
		if (inotifier == nullptr) {
			return false;
		} else if (inotifier->testEvents(IO_CLOSE)) {
			return disable(inotifier);
		} else if (inotifier->testEvents(IO_READ) && inotifier->read() == -1) {
			return disable(inotifier);
		}
		//-----------------------------------------------------------------
		const InotifyEvent *event = nullptr;
		while ((event = inotifier->next())) {
			auto uid = (
					inotifier == notifiers.inotifier ? 0 : inotifier->getUid());
			processInotification(uid, event);
		}
		return inotifier->isReady();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return disable(inotifier);
	}
}

bool Hub::handle(SignalWatcher *signalWatcher) noexcept {
	try {
		ssize_t nRead = 0;
		if (signalWatcher == nullptr) {
			return false;
		} else if (signalWatcher->testEvents(IO_CLOSE)) {
			return disable(signalWatcher);
		} else if (signalWatcher->testEvents(IO_READ)
				&& (nRead = signalWatcher->read()) == -1) {
			return disable(signalWatcher);
		}
		//-----------------------------------------------------------------
		if (nRead > 0) {
			auto uid = (
					signalWatcher == notifiers.signalWatcher ?
							0 : signalWatcher->getUid());
			processSignalNotification(uid, signalWatcher->getSignalInfo());
		}
		return signalWatcher->isReady();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return disable(signalWatcher);
	}
}

bool Hub::handle(Socket *connection) noexcept {
	if (connection == nullptr) {
		return false;
	} else if (connection->testEvents(IO_CLOSE)) {
		return disable(connection);
	} else if (connection->isType(SOCKET_LISTENER)) {
		return acceptConnection(connection);
	} else {
		return processConnection(connection);
	}
}

void Hub::run(void *arg) noexcept {
	try {
		setup(arg);
		loop();
		healthy = true; //terminated without error
	} catch (const BaseException &e) {
		healthy = false; //terminated due to error
		WH_LOG_EXCEPTION(e);
	}
	cleanup();
}

int Hub::getStatus() const noexcept {
	return running;
}

void Hub::setStatus(int status) noexcept {
	this->running = status;
}

void Hub::setup(void *arg) {
	WH_LOG_INFO("Starting....");
	configure(arg);
	startWorker(arg);
	WH_LOG_INFO("Hub %llu [PID: %d] started in %f seconds", getUid(), getpid(),
			uptime.elapsed());
}

void Hub::loop() {
	while (running) {
		monitor(outgoingMessages.isEmpty());
		publish();
		dispatch();
		processMessages();
		maintain();
	}
}

void Hub::initBuffers() {
	try {
		//Set up SSL/TLS
		Socket::setSSLContext(getSSLContext());
		//Initialize the connections pool
		Socket::initPool(ctx.connectionPoolSize);
		//Initialize the message Pool
		Message::initPool(ctx.messagePoolSize);
		//Stores incoming messages for processing
		incomingMessages.initialize(ctx.messagePoolSize);
		//Stores messages ready for publishing
		outgoingMessages.initialize(ctx.messagePoolSize);
		//Stores temporary connection identifiers
		temporaryConnections.initialize(ctx.maxNewConnnections);
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Hub::initReactor() {
	try {
		Reactor::initialize(ctx.maxIOEvents, !ctx.signal);
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Hub::initListener() {
	Socket *listener = nullptr;
	try {
		if (!ctx.listen) {
			return;
		}
		//-----------------------------------------------------------------
		auto serviceName = ctx.serviceName;
		auto isUnixSocket = false;
		NameInfo nodeAddress;
		if (!serviceName) {
			Identity::getAddress(getUid(), nodeAddress);
			if (!strcasecmp(nodeAddress.service, "unix")) {
				isUnixSocket = true;
				serviceName = nodeAddress.host;
			} else {
				serviceName = nodeAddress.service;
				isUnixSocket = false;
			}
		} else {
			isUnixSocket = ctx.serviceType
					&& !strcasecmp(ctx.serviceType, "unix");
		}
		//-----------------------------------------------------------------
		listener = new Socket(serviceName, ctx.backlog, isUnixSocket);
		listener->setUid(getUid());
		putWatcher(listener, IO_READ, WATCHER_ACTIVE);
		notifiers.listener = listener;
		WH_LOG_INFO("Hub %llu listening on port: %s", getUid(), serviceName);
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete listener;
		throw;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		delete listener;
		throw Exception(EX_ALLOCFAILED);
	}
}

void Hub::initClock() {
	Clock *clock = nullptr;
	try {
		if (ctx.timerExpiration) {
			clock = new Clock(ctx.timerExpiration, ctx.timerInterval);
			putWatcher(clock, IO_READ, WATCHER_ACTIVE);
			notifiers.clock = clock;
		} else {
			WH_LOG_DEBUG("Internal clock disabled");
			notifiers.clock = nullptr;
			return;
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete clock;
		throw;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		delete clock;
		throw Exception(EX_ALLOCFAILED);
	}
}

void Hub::initEventNotifier() {
	EventNotifier *enotifier = nullptr;
	try {
		enotifier = new EventNotifier(ctx.semaphore);
		putWatcher(enotifier, IO_READ, WATCHER_ACTIVE);
		notifiers.enotifier = enotifier;
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete enotifier;
		throw;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		delete enotifier;
		throw Exception(EX_ALLOCFAILED);
	}
}

void Hub::initInotifier() {
	Inotifier *inotifier = nullptr;
	try {
		inotifier = new Inotifier();
		putWatcher(inotifier, IO_READ, WATCHER_ACTIVE);
		notifiers.inotifier = inotifier;
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete inotifier;
		throw;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		delete inotifier;
		throw Exception(EX_ALLOCFAILED);
	}
}

void Hub::initSignalWatcher() {
	SignalWatcher *signalWatcher = nullptr;
	try {
		if (ctx.signal) {
			signalWatcher = new SignalWatcher();
			putWatcher(signalWatcher, IO_READ, WATCHER_ACTIVE);
			notifiers.signalWatcher = signalWatcher;
		} else {
			WH_LOG_DEBUG("Synchronous signal disabled");
			notifiers.signalWatcher = nullptr;
			return;
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete signalWatcher;
		throw;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		delete signalWatcher;
		throw Exception(EX_ALLOCFAILED);
	}
}

void Hub::startWorker(void *arg) {
	try {
		if (enableWorker() && !workerThread) {
			workerThread = new Thread(&worker);
			workerThread->start(arg);
			WH_LOG_INFO("Worker thread started");
		} else {
			WH_LOG_DEBUG("No worker thread");
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		throw Exception(EX_ALLOCFAILED);
	}
}

void Hub::stopWorker() {
	try {
		if (workerThread) {
			WH_LOG_INFO("Waiting for the worker thread to finish....");
			workerThread->join();
			delete workerThread;
			workerThread = nullptr;
			stopWork();
			WH_LOG_INFO("Worker thread stopped");
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Hub::publish() noexcept {
	//-----------------------------------------------------------------
	/*
	 * Incoming Allocation Strategy (IAS)
	 */
	auto capacity = Message::unallocated() + outgoingMessages.readSpace();
	//Limit on the number of queries that can be answered
	auto answerCapacity = (unsigned int) (capacity * ctx.answerRatio);
	//Limit on the number of queries that can be forwarded
	auto forwardCapacity = (unsigned int) (capacity * ctx.forwardRatio);
	//-----------------------------------------------------------------
	Message *msg = nullptr;
	Watcher *w = nullptr;
	while (outgoingMessages.get(msg)) {
		//-----------------------------------------------------------------
		//Sanity check
		if (!msg->validate()) {
			Message::recycle(msg);
			continue;
		}

		//Trap the message (e.g. registration request)
		if (msg->testFlags(MSG_TRAP) && trapMessage(msg)) {
			//Do not forward
			Message::recycle(msg);
			continue;
		}

		//Verify the destination
		if (msg->getDestination() == getUid()
				|| !(w = getWatcher(msg->getDestination()))
				|| w->testGroup(msg->getGroup())) {
			//Destination is sink or not found or group conflict
			Message::recycle(msg);
			continue;
		}
		//-----------------------------------------------------------------
		/*
		 * Answer First Priority (AFP) and Random Drop
		 */
		if (!w->testFlags(SOCKET_OVERLAY) && answerCapacity) {
			answerCapacity--;
		} else if (forwardCapacity) {
			forwardCapacity--;
		} else if (dropMessage(msg)) {
			//Message can be dropped
			countDropped(msg->getLength());
			Message::recycle(msg);
			continue;
		}
		//-----------------------------------------------------------------
		if (!w->publish(msg)) {
			//Recipient's queue is full, retry later
			incomingMessages.put(msg);
		} else if (w->testEvents(IO_WRITE)) {
			retain(w);
		}
	}
}

void Hub::processMessages() noexcept {
	Message *message;
	while (incomingMessages.get(message)) {
		if (!message->testFlags(MSG_PROCESSED)) {
			//All the other flags are cleared
			message->putFlags(MSG_PROCESSED);
			route(message);
		}
		outgoingMessages.put(message);
	}
}

bool Hub::acceptConnection(Socket *listener) noexcept {
	//Limited protection against flooding of new connections
	if (!temporaryConnections.hasSpace()) {
		//Clean up timed out temporary connections
		purgeTemporaryConnections();
	}
	//-----------------------------------------------------------------
	Socket *newConn = nullptr;
	try {
		newConn = listener->accept();
		if (!newConn) {
			//No more connections waiting
			return false;
		}

		//Announce the new arrival
		WH_LOG_DEBUG("A new connection %llu has arrived", newConn->getUid());
		/*
		 * Maintain this sequence to prevent resource leak
		 * and other unknown issues.
		 */
		//Activate the Connection
		if (temporaryConnections.put(newConn->getUid())) {
			putWatcher(newConn, IO_WR, 0);
			newConn->setOutputQueueLimit(ctx.outputQueueLimit);
		} else {
			throw Exception(EX_OVERFLOW);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete newConn;
	}
	//We might be having more connections waiting
	return true;
}

bool Hub::processConnection(Socket *connection) noexcept {
	try {
		//-----------------------------------------------------------------
		//First drain out all the messages
		if (connection->testEvents(IO_WRITE)
				&& connection->testFlags(WATCHER_OUT)) {
			connection->write();
		}

		//Read from the socket
		if (connection->testEvents(IO_READ)) {
			if (connection->read() == -1) {
				return disable(connection);
			}
		}
		//-----------------------------------------------------------------
		/*
		 * Congestion Control Mechanism
		 * Dynamically update on the basis of local parameters only
		 */
		unsigned int cycleLimit;
		if (ctx.throttle) {
			cycleLimit = throttle(connection);
		} else {
			cycleLimit = Twiddler::min(ctx.cycleInputLimit,
					Message::unallocated());
		}

		//-----------------------------------------------------------------
		/*
		 * Get all the messages from this connection
		 */
		unsigned int msgCount = 0;
		while (msgCount < cycleLimit) {
			Message *message = connection->getMessage();
			if (message) {
				incomingMessages.put(message);
				countReceived(message->getLength());
				msgCount++;
			} else {
				break;
			}
		}
		//-----------------------------------------------------------------
		return connection->isReady()
				|| (ctx.cycleInputLimit && (msgCount == cycleLimit));
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return disable(connection);
	}
	return false;
}

bool Hub::dropMessage(Message *message) const noexcept {
	return ctx.allowPacketDrop && !message->testFlags(MSG_PRIORITY)
			&& (message->addTTL() > ctx.messageTTL);
}

unsigned int Hub::throttle(const Socket *connection) const noexcept {
	/*
	 * [Congestion Control]: set limit on the number of messages the given
	 * connection may deliver in the current event loop
	 */
	auto available = Message::unallocated();
	//Few messages are reserved for overlay management
	if (available > ctx.reservedMessages) {
		available -= ctx.reservedMessages;
		if (!connection->testFlags(SOCKET_OVERLAY | SOCKET_PRIORITY)) {
			//A normal client connection
			auto ratio = ((double) available) / Message::poolSize();
			auto limit = (unsigned int) (ctx.cycleInputLimit * ratio);
			return Twiddler::min(limit, available);
		} else {
			//An important connection
			return Twiddler::min(ctx.cycleInputLimit, available);
		}
	} else if (connection->testFlags(SOCKET_PRIORITY)) {
		//A priority connection
		return Twiddler::min(ctx.reservedMessages, available);
	} else {
		//Everything else
		return 0;
	}
}

void Hub::countReceived(unsigned int bytes) noexcept {
	stats.msgReceived += 1;
	stats.bytesReceived += bytes;
}

void Hub::countDropped(unsigned int bytes) noexcept {
	stats.msgDropped += 1;
	stats.bytesDropped += bytes;
}

void Hub::clear() noexcept {
	running = 0;
	memset(&stats, 0, sizeof(stats));
	memset(&notifiers, 0, sizeof(notifiers));
	memset(&ctx, 0, sizeof(ctx));
	workerThread = nullptr;
}

int Hub::deleteWatchers(Watcher *w, void *arg) noexcept {
	delete w;
	return 1; // Remove the key from the hash table
}

} /* namespace wanhive */

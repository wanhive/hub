/*
 * Hub.cpp
 *
 * Base class for wanhive hubs
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Hub.h"
#include "../base/common/Logger.h"
#include "../base/Signal.h"
#include <unistd.h>

namespace wanhive {

Hub::Hub(unsigned long long uid, const char *path) noexcept :
		Identity { path }, uid { uid }, healthy { true } {
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

void Hub::periodic(unsigned int &expiration, unsigned int &interval) noexcept {
	if (prime.alarm) {
		expiration = prime.alarm->getExpiration();
		interval = prime.alarm->getInterval();
	} else {
		expiration = 0;
		interval = 0;
	}
}

void Hub::alert(unsigned long long events) {
	if (prime.event) {
		prime.event->write(events);
	} else {
		throw Exception(EX_RESOURCE);
	}
}

int Hub::track(const char *path, uint32_t mask) {
	if (prime.inotifier) {
		return prime.inotifier->add(path, mask);
	} else {
		throw Exception(EX_RESOURCE);
	}
}

void Hub::untrack(int identifier) noexcept {
	try {
		if (prime.inotifier) {
			prime.inotifier->remove(identifier);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
	}
}

void Hub::metrics(HubInfo &info) const noexcept {
	info.setUid(getUid());
	info.setUptime(uptime.elapsed());
	info.setReceived(traffic.received);
	info.setDropped(traffic.dropped);
	info.setConnections( { Socket::poolSize(), Socket::allocated() });
	info.setMessages( { Message::poolSize(), Message::allocated() });
	info.setMTU(Message::MTU);
}

bool Hub::redact() const noexcept {
	return ctx.redact;
}

bool Hub::attached(unsigned long long id) const noexcept {
	return watchers.contains(id);
}

Watcher* Hub::find(unsigned long long id) const noexcept {
	return watchers.select(id);
}

void Hub::attach(Watcher *w, uint32_t events, uint32_t flags) {
	if (w && !watchers.contains(w->getUid())) {
		add(w, events);
		watchers.insert(w);
		w->setFlags(flags);
	} else {
		throw Exception(EX_OPERATION);
	}
}

bool Hub::detach(unsigned long long id) noexcept {
	return disable(find(id));
}

Watcher* Hub::move(unsigned long long from, unsigned long long to,
		bool replace) noexcept {
	Watcher *w[2] = { nullptr, nullptr };
	if (!watchers.contains(from)) {
		return nullptr;
	} else if (watchers.move(from, to, w, replace)) {
		if (w[0] && (w[0] != w[1])) {
			//Disable the conflicting watcher
			disable(w[0]);
		}
		w[1]->setFlags(WATCHER_ACTIVE);
		return w[1];
	} else {
		disable(w[0]);
		return nullptr;
	}
}

void Hub::iterate(int (*fn)(Watcher *w, void *arg), void *arg) {
	watchers.iterate(fn, arg);
}

unsigned int Hub::reap(unsigned int target, bool force) noexcept {
	//Prepare the buffer for reading
	guests.rewind();
	auto timeout = force ? 0 : ctx.lease;

	unsigned int count = 0;
	unsigned long long id;
	while (guests.get(id)) {
		auto conn = find(id);
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
			guests.setIndex(guests.getIndex() - 1);
			break;
		}
	}
	//Prepare the buffer for adding more data towards rear
	guests.pack();
	return count;
}

bool Hub::collect(Message *message) noexcept {
	if (message && !message->isMarked() && message->validate()
			&& in.put(message)) {
		message->putFlags(MSG_WAIT_PROCESSING);
		message->setMarked();
		return true;
	} else {
		return false;
	}
}

bool Hub::forward(Message *message) noexcept {
	if (message && !message->isMarked() && out.put(message)) {
		message->putFlags(MSG_PROCESSED);
		message->setMarked();
		return true;
	} else {
		return false;
	}
}

void Hub::admit(Watcher *w) {
	if (w->getReference() == nullptr) {
		w->start();
		w->setReference(static_cast<Hub*>(this)); //:-)
	} else {
		throw Exception(EX_ARGUMENT);
	}
}

bool Hub::react(Watcher *w) noexcept {
	if (w->getReference() == this) {
		return w->callback(nullptr);
	} else {
		return false;
	}
}

void Hub::expel(Watcher *w) noexcept {
	if (!w->testFlags(WATCHER_CRITICAL)) {
		auto id = w->getUid();
		watchers.remove(id);
		w->stop();
		delete w;
		WH_LOG_DEBUG("Watcher %llu recycled", id);
	} else {
		WH_LOG_ERROR("Critical component failure, exiting.");
		exit(EXIT_FAILURE);
	}
}

void Hub::configure(void *arg) {
	try {
		uptime.now();
		Identity::initialize();
		auto &conf = Identity::getOptions();

		ctx.listen = conf.getBoolean("HUB", "listen");
		ctx.backlog = conf.getNumber("HUB", "backlog");

		::memset(ctx.name, 0, sizeof(ctx.name));
		::strncpy(ctx.name, conf.getString("HUB", "name", ""),
				sizeof(ctx.name) - 1);
		::memset(ctx.type, 0, sizeof(ctx.type));
		::strncpy(ctx.type, conf.getString("HUB", "type", ""),
				sizeof(ctx.type) - 1);

		ctx.events = conf.getNumber("HUB", "events", 4);
		ctx.expiration = conf.getNumber("HUB", "expiration");
		ctx.interval = conf.getNumber("HUB", "interval");
		ctx.semaphore = conf.getBoolean("HUB", "semaphore");
		ctx.signal = conf.getBoolean("HUB", "signal");

		ctx.connections = conf.getNumber("HUB", "connections");
		ctx.messages = conf.getNumber("HUB", "messages");
		//Take care of the "special" cases: [1, 2, power-of-two]
		if (ctx.messages == 1 || ctx.messages == 2) {
			ctx.messages = 3;
		} else if (Twiddler::isPower2(ctx.messages)) { //false for 0
			ctx.messages -= 1;
		}

		ctx.guests = conf.getNumber("HUB", "guests");
		//Take care of the special case: Hub not listening
		if (ctx.listen) {
			ctx.guests = Twiddler::min(ctx.guests, ctx.connections);
		} else {
			ctx.guests = 0;
		}
		ctx.lease = conf.getNumber("HUB", "lease");

		ctx.inward = conf.getNumber("HUB", "inward");
		ctx.outward = conf.getNumber("HUB", "outward");
		ctx.outward = Twiddler::min(ctx.outward, (Socket::OUT_QUEUE_SIZE - 1));

		ctx.throttle = conf.getBoolean("HUB", "throttle");
		ctx.reserved = conf.getNumber("HUB", "reserved");
		ctx.reserved = Twiddler::min(ctx.reserved, ctx.messages);

		ctx.policing = conf.getBoolean("HUB", "policing");
		ctx.ttl = conf.getNumber("HUB", "TTL");

		ctx.answer = conf.getDouble("HUB", "answer", 0.5);
		ctx.forward = conf.getDouble("HUB", "forward", 0);

		ctx.logging = conf.getNumber("HUB", "logging", WH_LOGLEVEL_DEBUG);
		Logger::getDefault().setLevel(ctx.logging);
		ctx.logging = Logger::getDefault().getLevel();
		ctx.redact = conf.getBoolean("OPT", "redact", true);
		//-----------------------------------------------------------------
		WH_LOG_DEBUG(
				"\nLISTEN=%s, BACKLOG=%d, SERVICE_NAME='%s', SERVICE_TYPE='%s',\n" "IO_EVENTS=%u, TIMER_EXPIRATION=%ums, TIMER_INTERVAL=%ums, SEMAPHORE=%s,\n" "SYNCHRONOUS_SIGNAL=%s, CONNECTIONS=%u, MESSAGES=%u,\n" "NEW_CONNECTIONS=%u, NEW_CONNECTION_TIMEOUT=%ums, CYCLE_IN_LIMIT=%u,\n" "OUT_QUEUE_LIMIT=%u, THROTTLE=%s, RESERVED_MESSAGES=%u, ALLOW_PACKET_DROP=%s,\n" "MESSAGE_TTL=%u, ANSWER_RATIO=%f, FORWARD_RATIO=%f, LOG_LEVEL=%s, REDACT=%s\n",
				WH_BOOLF(ctx.listen), ctx.backlog, ctx.name, ctx.type,
				ctx.events, ctx.expiration, ctx.interval,
				WH_BOOLF(ctx.semaphore), WH_BOOLF(ctx.signal), ctx.connections,
				ctx.messages, ctx.guests, ctx.lease, ctx.inward, ctx.outward,
				WH_BOOLF(ctx.throttle), ctx.reserved, WH_BOOLF(ctx.policing),
				ctx.ttl, ctx.answer, ctx.forward,
				Logger::levelString(Logger::getDefault().getLevel()),
				WH_BOOLF(ctx.redact));
		//-----------------------------------------------------------------
		/*
		 * Initialization of the core data structures
		 */
		initBuffers();
		initReactor();
		initListener();
		initAlarm();
		initEvent();
		initInotifier();
		initInterrupt();
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
		//1. Wait for the worker
		await();
		//-----------------------------------------------------------------
		//2. Disconnect: recycle all watchers
		iterate(deleteWatchers, nullptr);
		//-----------------------------------------------------------------
		//3. Clean up all the containers
		guests.clear();
		Message *msg;
		while (out.get(msg)) {
			Message::recycle(msg);
		}
		while (in.get(msg)) {
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
		WH_LOG_INFO("Shutdown complete.\n\n");
	} catch (const BaseException &e) {
		//Resource leak, do not try to recover
		WH_LOG_EXCEPTION(e);
		WH_LOG_ERROR("Resource leak, aborting.");
		abort();
	} catch (...) {
		//Resource leak, do not try to recover
		WH_LOG_EXCEPTION_U();
		WH_LOG_ERROR("Resource leak, aborting.");
		abort();
	}

	/*
	 * If we are here then we can reuse this object.
	 */
}

void Hub::maintain() noexcept {

}

bool Hub::probe(Message *message) noexcept {
	return false;
}

void Hub::route(Message *message) noexcept {

}

void Hub::onAlarm(unsigned long long uid, unsigned long long ticks) noexcept {

}

void Hub::onEvent(unsigned long long uid, unsigned long long events) noexcept {

}

void Hub::onInotification(unsigned long long uid,
		const InotifyEvent *event) noexcept {

}

void Hub::onInterrupt(unsigned long long uid, int signum) noexcept {

}

void Hub::onLogic(unsigned long long uid, const LogicEvent &event) noexcept {

}

void Hub::onStream(unsigned long long id, Sink<unsigned char> &sink,
		Source<unsigned char> &source) noexcept {

}

bool Hub::handle(Alarm *alarm) noexcept {
	try {
		unsigned long long count = 0;
		if (alarm == nullptr) {
			return false;
		} else if (alarm->testEvents(IO_CLOSE)) {
			return disable(alarm);
		} else if (alarm->testEvents(IO_READ) && alarm->read(count) == -1) {
			return disable(alarm);
		}
		//-----------------------------------------------------------------
		if (count) {
			auto uid = (alarm == prime.alarm ? 0 : alarm->getUid());
			onAlarm(uid, count);
		}
		return alarm->isReady();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return disable(alarm);
	}
}

bool Hub::handle(Event *event) noexcept {
	try {
		unsigned long long count = 0;
		if (event == nullptr) {
			return false;
		} else if (event->testEvents(IO_CLOSE)) {
			return disable(event);
		} else if (event->testEvents(IO_READ) && event->read(count) == -1) {
			return disable(event);
		}
		//-----------------------------------------------------------------
		if (count) {
			auto uid = (event == prime.event ? 0 : event->getUid());
			onEvent(uid, count);
		}
		return event->isReady();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return disable(event);
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
			auto uid = (inotifier == prime.inotifier ? 0 : inotifier->getUid());
			onInotification(uid, event);
		}
		return inotifier->isReady();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return disable(inotifier);
	}
}

bool Hub::handle(Interrupt *interrupt) noexcept {
	try {
		int signum = 0;
		if (interrupt == nullptr) {
			return false;
		} else if (interrupt->testEvents(IO_CLOSE)) {
			return disable(interrupt);
		} else if (interrupt->testEvents(IO_READ)
				&& interrupt->read(signum) == -1) {
			return disable(interrupt);
		}
		//-----------------------------------------------------------------
		if (signum > 0) {
			auto uid = (interrupt == prime.interrupt ? 0 : interrupt->getUid());
			onInterrupt(uid, signum);
		}
		return interrupt->isReady();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return disable(interrupt);
	}
}

bool Hub::handle(Logic *logic) noexcept {
	try {
		LogicEvent event;
		if (logic == nullptr) {
			return false;
		} else if (logic->testEvents(IO_CLOSE)) {
			return disable(logic);
		} else if (logic->testEvents(IO_READ) && logic->update(event) == -1) {
			return disable(logic);
		}
		//-----------------------------------------------------------------
		if (event.type != LogicEdge::NONE) {
			onLogic(logic->getUid(), event);
		}
		return logic->isReady();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return disable(logic);
	}
}

bool Hub::handle(Socket *socket) noexcept {
	if (socket == nullptr) {
		return false;
	} else if (socket->testEvents(IO_CLOSE)) {
		return disable(socket);
	} else if (socket->isType(SOCKET_LISTENER)) {
		return acceptConnection(socket);
	} else {
		return processConnection(socket);
	}
}

bool Hub::handle(Stream *stream) noexcept {
	if (stream == nullptr) {
		return false;
	} else if (stream->testEvents(IO_CLOSE)) {
		return disable(stream);
	} else {
		return processStream(stream);
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
	async(arg);
	WH_LOG_INFO("Hub %llu [PID: %d] started in %f seconds", getUid(), getpid(),
			uptime.elapsed());
}

void Hub::loop() {
	while (running) {
		poll(out.isEmpty());
		publish();
		dispatch();
		process();
		maintain();
	}
}

void Hub::initBuffers() {
	try {
		//Set up SSL/TLS
		Socket::setSSLContext(getSSLContext());
		//Initialize the connections pool
		Socket::initPool(ctx.connections);
		//Initialize the message Pool
		Message::initPool(ctx.messages);
		//Stores incoming messages for processing
		in.initialize(ctx.messages);
		//Stores messages ready for publishing
		out.initialize(ctx.messages);
		//Stores temporary connection identifiers
		guests.initialize(ctx.guests);
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Hub::initReactor() {
	try {
		Reactor::initialize(ctx.events, !ctx.signal);
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
		auto serviceName = ctx.name;
		auto isUnixSocket = false;
		NameInfo ni; //keep in scope
		if (!serviceName[0]) {
			Identity::getAddress(getUid(), ni);
			isUnixSocket = (::strcasecmp(ni.service, "unix") == 0);
			serviceName = isUnixSocket ? ni.host : ni.service;
		} else {
			isUnixSocket = (::strcasecmp(ctx.type, "unix") == 0);
		}
		//-----------------------------------------------------------------
		listener = new Socket(serviceName, ctx.backlog, isUnixSocket);
		listener->setUid(getUid());
		attach(listener, IO_READ, (WATCHER_ACTIVE | WATCHER_CRITICAL));
		prime.listener = listener;
		WH_LOG_INFO("Hub %llu listening on port: %s", getUid(), serviceName);
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete listener;
		throw;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		delete listener;
		throw Exception(EX_MEMORY);
	}
}

void Hub::initAlarm() {
	Alarm *alarm = nullptr;
	try {
		if (ctx.expiration) {
			alarm = new Alarm(ctx.expiration, ctx.interval);
			attach(alarm, IO_READ, (WATCHER_ACTIVE | WATCHER_CRITICAL));
			prime.alarm = alarm;
		} else {
			WH_LOG_DEBUG("Internal alarm disabled");
			prime.alarm = nullptr;
			return;
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete alarm;
		throw;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		delete alarm;
		throw Exception(EX_MEMORY);
	}
}

void Hub::initEvent() {
	Event *event = nullptr;
	try {
		event = new Event(ctx.semaphore);
		attach(event, IO_READ, (WATCHER_ACTIVE | WATCHER_CRITICAL));
		prime.event = event;
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete event;
		throw;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		delete event;
		throw Exception(EX_MEMORY);
	}
}

void Hub::initInotifier() {
	Inotifier *inotifier = nullptr;
	try {
		inotifier = new Inotifier();
		attach(inotifier, IO_READ, (WATCHER_ACTIVE | WATCHER_CRITICAL));
		prime.inotifier = inotifier;
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete inotifier;
		throw;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		delete inotifier;
		throw Exception(EX_MEMORY);
	}
}

void Hub::initInterrupt() {
	Interrupt *interrupt = nullptr;
	try {
		if (ctx.signal) {
			interrupt = new Interrupt();
			attach(interrupt, IO_READ, (WATCHER_ACTIVE | WATCHER_CRITICAL));
			prime.interrupt = interrupt;
		} else {
			WH_LOG_DEBUG("Synchronous signal disabled");
			prime.interrupt = nullptr;
			return;
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete interrupt;
		throw;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		delete interrupt;
		throw Exception(EX_MEMORY);
	}
}

void Hub::async(void *arg) {
	try {
		if (Job::start(arg)) {
			WH_LOG_INFO("Worker started");
		} else {
			WH_LOG_DEBUG("No worker");
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Hub::await() {
	try {
		Job::stop();
		WH_LOG_INFO("Worker stopped");
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
	auto capacity = Message::unallocated() + out.readSpace();
	//Limit on the number of queries that can be answered
	auto answerCapacity = (unsigned int) (capacity * ctx.answer);
	//Limit on the number of queries that can be forwarded
	auto forwardCapacity = (unsigned int) (capacity * ctx.forward);
	//-----------------------------------------------------------------
	Message *msg = nullptr;
	Watcher *w = nullptr;
	while (out.get(msg)) {
		//-----------------------------------------------------------------
		//Sanity check
		if (!msg->validate()) {
			Message::recycle(msg);
			continue;
		}

		//Trap the message (e.g. registration request)
		if (msg->testFlags(MSG_PROBE) && probe(msg)) {
			//Do not forward
			Message::recycle(msg);
			continue;
		}

		//Verify the destination
		if (msg->getDestination() == getUid()
				|| !(w = find(msg->getDestination()))
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
		} else if (drop(msg)) {
			//Message can be dropped
			countDropped(msg->getLength());
			Message::recycle(msg);
			continue;
		}
		//-----------------------------------------------------------------
		if (!w->publish(msg)) {
			//Recipient's queue is full, retry later
			in.put(msg);
		} else if (w->testEvents(IO_WRITE)) {
			retain(w);
		}
	}
}

void Hub::process() noexcept {
	Message *message;
	while (in.get(message)) {
		if (!message->testFlags(MSG_PROCESSED)) {
			//All the other flags are cleared
			message->putFlags(MSG_PROCESSED);
			route(message);
		}
		out.put(message);
	}
}

bool Hub::acceptConnection(Socket *listener) noexcept {
	//Limited protection against flooding of new connections
	if (!guests.hasSpace()) {
		//Clean up timed out temporary connections
		reap();
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
		if (guests.put(newConn->getUid())) {
			attach(newConn, IO_WR, 0);
			newConn->setOption(WATCHER_WRITE_BUFFER_MAX, ctx.outward);
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
		if (connection->testEvents(IO_READ) && (connection->read() == -1)) {
			return disable(connection);
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
			cycleLimit = Twiddler::min(ctx.inward, Message::unallocated());
		}

		//-----------------------------------------------------------------
		/*
		 * Get all the messages from this connection
		 */
		unsigned int msgCount = 0;
		while (msgCount < cycleLimit) {
			Message *message = connection->getMessage();
			if (message) {
				in.put(message);
				countReceived(message->getLength());
				msgCount++;
			} else {
				break;
			}
		}
		//-----------------------------------------------------------------
		return connection->isReady() || (ctx.inward && (msgCount == cycleLimit));
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return disable(connection);
	}
}

bool Hub::processStream(Stream *stream) noexcept {
	try {
		//-----------------------------------------------------------------
		//Write to the stream
		if (stream->testEvents(IO_WRITE) && stream->testFlags(WATCHER_OUT)) {
			stream->write();
		}

		//Read from the stream
		if (stream->testEvents(IO_READ) && (stream->read() == -1)) {
			return disable(stream);
		}
		//-----------------------------------------------------------------
		onStream(stream->getUid(), *stream, *stream);
		return stream->isReady();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return disable(stream);
	}
}

bool Hub::drop(Message *message) const noexcept {
	return ctx.policing && !message->testFlags(MSG_PRIORITY)
			&& (message->addHopCount() > ctx.ttl);
}

unsigned int Hub::throttle(const Socket *connection) const noexcept {
	/*
	 * [Congestion Control]: set limit on the number of messages the given
	 * connection may deliver in the current event loop
	 */
	auto available = Message::unallocated();
	//Few messages are reserved for overlay management
	if (available > ctx.reserved) {
		available -= ctx.reserved;
		if (!connection->testFlags(SOCKET_OVERLAY | SOCKET_PRIORITY)) {
			//A normal client connection
			auto ratio = ((double) available) / Message::poolSize();
			auto limit = (unsigned int) (ctx.inward * ratio);
			return Twiddler::min(limit, available);
		} else {
			//An important connection
			return Twiddler::min(ctx.inward, available);
		}
	} else if (connection->testFlags(SOCKET_PRIORITY)) {
		//A priority connection
		return Twiddler::min(ctx.reserved, available);
	} else {
		//Everything else
		return 0;
	}
}

void Hub::countReceived(unsigned int bytes) noexcept {
	traffic.received.units += 1;
	traffic.received.bytes += bytes;
}

void Hub::countDropped(unsigned int bytes) noexcept {
	traffic.dropped.units += 1;
	traffic.dropped.bytes += bytes;
}

void Hub::clear() noexcept {
	running = 0;
	memset(&traffic, 0, sizeof(traffic));
	memset(&prime, 0, sizeof(prime));
	memset(&ctx, 0, sizeof(ctx));
}

int Hub::deleteWatchers(Watcher *w, void *arg) noexcept {
	delete w;
	return 1; // Remove the key from the hash table
}

} /* namespace wanhive */

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [14.0.0] - 2025-07-03

### Added

- **Stream** class for lightweight real-time data streams.
- Single precision floating point support in the **Message** class.
- Basic data pre-processing and visualization routines.

### Changed

- **Sink** and **Source** interfaces.
- Return **Message::recycle** function's execution status as a boolean value.
- Simplify floating point type serialization/de-serialization.

### Fixed

- Explicitly specify the namespace in the **Logger** macros.

## [13.0.0] - 2025-05-15

### Added

- **wh_pkg_version** C-style function that returns the package version string.
- **wh_api_version** C-style function that returns the library version string.

### Changed

- Rename the **Hub::processAlarm** member function to **Hub::onAlarm**.
- Rename the **Hub::processEvent** member function to **Hub::onEvent**.
- Rename the **Hub::processInotification** member function to **Hub::onInotification**.
- Rename the **Hub::processInterrupt** member function to **Hub::onInterrupt**.
- Rename the **Hub::processLogic** member function to **Hub::onLogic**.
- Rename the **Hub::enableWorker** member function to **Hub::hasWorker**.

## [12.0.0] - 2025-03-18

### Added

- Token bucket implementation through the **Tokens** class.
- Parameterized database (PostgreSQL) connection management in the authentication hub.
- Hash map (**Kmap**) and hash set (**Kset**) specializations of the generic hash table (**Khash**).

### Changed

- **SecurityException::errorCode()** method now always returns zero (0).
- Rename the **Hub::fetch()** method to **Hub::find()**.
- Rename the **Hub::trapMessage()** method to **Hub::trap()**.
- Rename the **Hub::retainMessage()** method to **Hub::collect()**.
- Rename the **Hub::sendMessage()** method to **Hub::forward()**.
- Rename the **Hub::purgeTemporaryConnections()** method to **Hub::evict()**.
- Rename the **Hub::getAlarmSettings()** method to **Hub::periodic()**.
- Rename the **Hub::reportEvents()** method to **Hub::alert()**.
- Rename the **Hub::addToInotifier()** method to **Hub::track()**.
- Rename the **Hub::removeFromInotifier()** method to **Hub::untrack()**.

### Fixed

- **Configuration::map()** method should have **const** modifier.

### Removed

- Default template arguments of the hash table (**Khash**).
- **TokenBucket** class (the old token bucket implementation).

## [11.0.2] - 2024-07-26

### Fixed

- Clean up the **Reactor**'s ready list on re-initialization.

## [11.0.1] - 2024-07-08

### Fixed

- Configuration::remove() methods should set the correct status code to indicate the uncommitted changes.

## [11.0.0] - 2024-06-08

### Added

- The Configuration class now includes new methods for better management of sections and properties.
- Implementation of end-to-end local message tracing.
- Token bucket implementation.

### Changed

- The authentication hub now reuses the existing database connection to serve new requests.

### Security

- Enhanced overlay hub protection against the malicious requests (experimental feature).

## [10.0.0] - 2022-11-24

### Added

- Additional watcher flags, including the two user-defined flags.
- **NonCopyable** class which cannot be copy-constructed or copy-assigned.
- Doxygen support.

### Changed

- Simplify package structure and APIs.
- Update API documentation.
- Improve type and pointer safety.
- Improve exception handling.

### Security

- Use a secure heap for memory allocation in SRP-6a implementation.

## [9.1.1] - 2022-10-17

### Changed

- Add and update API documentation (work in progress).
- **File::duplicate()** method throws exception if old and new descriptors are identical.

### Fixed

- **Storage::sync()** should not throw exception on success.

## [9.1.0] - 2022-09-26

### Added

- **PSession::getLeader()** method to read the controlling terminal's session identifier.
- **FStat::access()** method to read the file permission bits.

### Changed

- Update authentication hub.
- Add and update API documentation (work in progress).
- **SignalAction** prevents unreliable signal operations.

### Fixed

- **Directory::read()** method should throw exception only on system error.
- Provide a third argument to the system call in **Fcntl::duplicate()** method.

## [9.0.0] - 2022-09-14

### Added

- Digital logic watcher (adapter class for GPIO line monitoring).

### Changed

- Codebase updates (backward incompatible) to improve quality and performance.

### Fixed

- Allow atomic operations only on integral scalar and pointer data types.
- Reset **Reactor**'s timeout value to default on initialization.
- Resolve the default service type (configuration value) correctly in **Hub**.

## [8.0.0] - 2022-06-06

### Added

- Clear and concise API documentation for application developers (work in progress).

### Changed

- Major package restructuring and API changes.
- Simplify the data-unit implementation.

### Fixed

- Align **Inotifier**'s read buffer correctly.
- Assign correct data type to the MessageFlag enumeration.
- **CircularBuffer::setStatus** and **StaticCircularBuffer::setStatus** methods have syntax error.
- Handle zero (immediate) timeout correctly in **Selector**.

### Security

- Upgrade to OpenSSL version 3.0 (the new LTS version).

## [7.0.1] - 2022-02-28

### Changed

- ClientHub can connect to the controller.

### Fixed

- Both **Identity::getIdentifiers** methods should exhibit the same external behavior.

### Security

- Protect sensitive data from being exposed in message logs (ClientHub and AuthenticationHub).

## [7.0.0] - 2022-02-02

### Added

- Preprocessor macros for the masking of sensitive data in message logs.

### Changed

- The data packet and protocol implementation.
- Update the installation guide.

### Fixed

- Do not clear out the Message flags while building a get-key or registration request.

## [6.0.0] - 2022-01-05

### Added

- Command-line option for message logging to the **Syslog**.
- Man page **wanhive(1)**.

### Changed

- Source-code updates and package restructuring to improve maintainability.

### Fixed

- Potential stack overflow during session key negotiation in **OverlayHub**.

### Security

- Upgrade to 3072-bit SRP-6a group for better password security.
- Upgrade to 3072-bit RSA to offer greater security during distributed authentication.

## [5.0.0] - 2021-11-18

### Added

- **Logger** can write messages to the **syslog** facility.

### Changed

- Rename the method **SignalWatcher::getSignalInfo** to **SignalWatcher::getInfo**.
- Restrict the accuracy of **Timer::hasTimedOut** method to milliseconds.
- The Hub configuration parameter **cycleInputLimit** defaults to zero (0).

### Fixed

- Return type of **Storage::readLink** should be **ssize_t**.
- Data types of **Selector** event and flag enumerations should be 32-bit unsigned int.

### Removed

- All the **timerfd** related methods from the **Timer** class.
- All the **signalfd** related methods from the **Signal** class.
- The **Signal::sigchildHandler** method.

## [4.0.0] - 2021-04-01

### Added

- Bootstrap using the hosts database/file.

### Changed

- Disable TLS/SSL on unix domain socket (directory permission is a zero-overhead solution).
- Increase the outgoing message queue size in **Socket** to improve performance.
- Update the **Inotifier::read** method to improve performance.
- Update the routing table maintenance routines in **OverlayHub**.
- Major updates in the **Endpoint** class and it's subclasses.
- Rename **Identity::loadIdentifiers** to **Identity::getIdentifiers**.
- **Hub::removeWatcher** returns boolean instead of void.
- **Hub::retainMessage** sets the Message flag to **MSG_WAIT_PROCESSING**.
- **Hub::purgeTemporaryConnections** takes an additional parameter.

### Fixed

- Potential integer overflow in **Message::packets**.

### Removed

- The **InstanceID::SIZE** public constant.

## [3.0.0] - 2021-03-02

### Added

- MessageContext structure in the MessageHeader.h file.
- FVN-1a hash (Fowler–Noll–Vo hash) method in the Twiddler class.

### Changed

- Source code restructuring and cleanup. Not backward compatible.
- **Storage::lock** and **Storage::unlock** methods work as they should.
- Improve error message handling in the **SystemException** and **SecurityException** classes.

### Security

- Plug a potential buffer overflow in the **Host::get** method.

## [2.1.2] - 2021-02-22

### Added

- Wanhive communication protocol specification (PDF document).

### Changed

- Leave the data structures in a clean state after initialization failure.
- Source code clean up.
- Update the documentation.

## [2.1.1] - 2021-01-16

### Fixed

- Replace the library function `localtime` with `localtime_r` for thread safety.
- Use type casts for narrow conversions.

## [2.1.0] - 2021-01-01

### Added

- Doubly linked list.
- **Hub::removeWatcher** method to remove a watcher from the event loop.

### Changed

- Improve the input handling in the **Configuration** class.

### Fixed

- The **Socket** class should not report a *security violation* on SSL disconnection (SSL_ERROR_ZERO_RETURN).
- Normalize the timespec value and use appropriate type casts in **Timer**.

## [2.0.0] - 2020-12-17

### Added

- The new **hosts file** format (Revision 1). The new format adds a numeric **TYPE** column. The old format remains supported.
- The **Host** class returns a randomized list of *hub identifiers of a given type*.
- Binary heap-based priority queue.

### Changed

- Improve bounds checking in **Message** and **Endpoint** classes.
- Rename the structures, methods and fields to comply with the naming convention.
- Update the documentation.

### Fixed

- Integer overflows in the Twiddler class.
- Message's origin should be immutable.

### Removed

- Public methods in the **Identity** class that modify the hosts database.

## [1.0.0] - 2020-11-25

First stable release.

### Added

- Implementation of the Wanhive protocol.

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Doubly linked list.
- **Hub::removeWatcher** method to remove a watcher from the event loop.

### Changed

- Improve the input handling in the **Configuration** class.

### Fixed

- The **Socket** class should not report a *security violation* on SSL disconnection (SSL_ERROR_ZERO_RETURN).
- Normalize the timespec value in **Timer::sleep**.

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

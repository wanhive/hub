# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- The new **hosts file** format. The new format adds a numeric **TYPE** column. The old format remains supported.
- The **Hosts** class returns a randomized list of *hub identifiers of a given type*.
- Binary heap-based priority queue.

### Changed

- Strict bounds checking in the Message class.
- Rename the structures, methods and fields to comply with the guidelines.
- Update the documentation.
- Rename and update the code of conduct file.

### Fixed

- Integer overflows in the Twiddler class.
- Message's origin should be immutable.

### Removed

- Methods in the **Identity** class that modify the hosts database.

## [1.0.0] - 2020-11-25

First stable release.

### Added

- Implementation of the Wanhive protocol.